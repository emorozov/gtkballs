/* child.c - spawn child, that works with extra privileges
 * to be able to write scores.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 */
#include <glib.h>
#include <sys/time.h>	/* select() */
#include <sys/types.h>	/* pid_t */
#include <sys/stat.h>	/* open() */
#include <fcntl.h>	/* fcntl() */
#include <signal.h>	/* sigaction() */
#include <unistd.h>     /* pipe() */
#include <string.h>     /* strerror() */
#include <errno.h>	/* errno */
#include <stdlib.h>	/* exit() */
#include <sys/wait.h>   /* waitpid() */
#include <stdio.h>      /* printf() */

volatile sig_atomic_t _child_writer_alive = 0;
gboolean _child_writer_dead_warned = 0;

int child_writer_alive(void){
   return _child_writer_alive;
}

int child_writer_dead_handler(void) {
   if (_child_writer_alive == 0 && _child_writer_dead_warned == 0) {
      _child_writer_dead_warned = 1;
      return 1;
   }
   return 0;
}

void child_process_score_writer(int chfd, gchar *score_file)
{
   fd_set rfds;
   int i;
   int fd=-1;
   gchar *buf;
   size_t sz;
   sigset_t sset;
   struct flock lockinfo;
   gchar *score_file_full = g_strconcat(LOCALSTATEDIR, score_file, NULL);

   while (1)
   {
      FD_ZERO(&rfds);
      FD_SET(chfd, &rfds);
      if (select(chfd + 1, &rfds, NULL, NULL, NULL) > 0)
      {
         if (read(chfd, &sz, sizeof(sz)) <= 0) {
            exit(0);
         }
         /* block signals before writing, to prevent possible file corruption */
         sigemptyset(&sset);
         sigaddset(&sset, SIGHUP);
         sigaddset(&sset, SIGINT);
         sigaddset(&sset, SIGQUIT);
         sigaddset(&sset, SIGTERM);
         sigprocmask(SIG_BLOCK, &sset, NULL);
         fd = open(score_file_full, O_WRONLY | O_TRUNC);
         if (fd != -1) {
            /* get write lock before writing scores */
            lockinfo.l_whence = SEEK_SET;
            lockinfo.l_start = 0;
            lockinfo.l_len = 0;

            i = 0;
            while (1) {
               lockinfo.l_type=F_WRLCK;
               if (!fcntl(fd, F_SETLK, &lockinfo)) {
                  break;
               }
               if (i >= 3) {
                  close(fd);
                  fd = -1;
               }
               i++;
            }
         }
         while (sz > 0) {
            buf=g_malloc(sz);
            read(chfd, buf, sz);
            if (fd != -1) {
               write(fd, buf, sz);
            }
            g_free(buf);
            read(chfd, &sz, sizeof(sz));
         }

         if (fd != -1) {
            close(fd);
         } else {
            /* FIXME: here should be some sort of error reporting to parent */
         }
         sigprocmask(SIG_UNBLOCK, &sset, NULL);
      }
   }
}

static void sigchld_handler(int param)
{
   pid_t ret = waitpid(0, NULL, WNOHANG);

   if (ret > 0) { /* score writer process killed by bastards! */
      _child_writer_alive = 0;
   }
}

int child_setup(gchar *score_file)
{
   pid_t pid;
   struct sigaction sact;
   int sfds[2];

   /* set up SIGCHLD handler, so we can know if our score writer process
   terminated while we run... */
   sact.sa_handler = sigchld_handler;
   sigemptyset(&sact.sa_mask);
   sact.sa_flags = 0;
#ifdef SA_RESTART
   sact.sa_flags |= SA_RESTART;
#endif
   if (sigaction(SIGCHLD, &sact, NULL) < 0) {
      printf("cannot setup SIGCHLD handler.\n");
      return -1;
   }
   if (pipe(sfds) == -1) {
      printf("pipe() failed: %s\n", strerror(errno));
      return -1;
   }
   pid = fork();
   if (pid == -1) {
      printf("cannot fork: %s\n", strerror(errno));
      return -1;
   } else if (pid == 0) {
      close(sfds[1]);
      child_process_score_writer(sfds[0], score_file);
   }
   close(sfds[0]);
   _child_writer_alive = 1;

   return sfds[1];
}

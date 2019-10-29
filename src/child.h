#ifndef __CHILD_H__
#define __CHILD_H__

/* spawn score writer process */
int child_setup(gchar *score_file);

/* check if child process alive or not */
int child_writer_alive(void);

/* check score writer process and display warning if not running */
int child_writer_dead_handler(void);

#endif

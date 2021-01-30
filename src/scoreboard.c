/* scoreboard.c - save/load scores
 *
 * This program is free software; you can redistribute it and/or
 * modif (y it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <gtk/gtk.h>
#include <glib.h>

#include "gtkballs.h"
#include "scoreboard.h"

#include "game.h"

#define SCORE_FILE "gtkballs-scores"
#define BUFFER_SIZE 1024


void free_score_board_full(struct score_board_full *bf, gint nbf)
{
   gint i;

   for (i = 0; i < nbf; i++) {
      g_free(bf[i].rules);
   }
   g_free(bf);
}


gint write_score(struct score_board *b, struct score_board_full *bf, gint nbf)
{
   gint i;
   gchar *tname, *tdate, *rules;
   char * score_file = get_config_dir_file (SCORE_FILE);
   FILE * fd;

   fd = fopen (score_file, "w");
   if (!fd) {
      g_free (score_file);
      return FALSE;
   }

   for (i = 0; i < 10; i++)
   {
      if (strlen(b[i].name)) {
         tname = g_strdup(b[i].name);
         if (!tname) {
            tname = g_strdup(_("Unknown"));
         }
         tdate = g_strdup(b[i].date);
         if (!tdate) {
            tdate = g_strdup(_("Unknown"));
         }
         rules = rules_get_as_str();
         fprintf (fd, "%s\t%i\t%s\t%s\n", tname, b[i].score, tdate, rules);
         g_free(rules);
         g_free(tdate);
         g_free(tname);
      }
   }
   for (i = 0; i < nbf; i++) {
      if (strlen(bf[i].name)) {
         fprintf (fd, "%s\t%i\t%s\t%s\n", bf[i].name, bf[i].score, bf[i].date, bf[i].rules);
      }
   }
   g_free (score_file);
   fclose (fd);

   return TRUE;
}


int score_sort(const void *a, const void *b)
{
   if (((const struct score_board *)a)->score == ((const struct score_board *)b)->score) return 0;
   if (((const struct score_board *)a)->score < ((const struct score_board *)b)->score) return 1;
   return -1;
}


gint read_score(struct score_board *b, struct score_board_full **bf, gint *nbf)
{
   FILE *fp;
   gchar buffer[BUFFER_SIZE];
   gchar **str_val, *tstr, **tstr_val;
   gint  valid, sc, fsc;
   gsize br, bw;
   char * score_file = get_config_dir_file (SCORE_FILE);

   gchar *g_rules = NULL;

   memset(b, 0, sizeof(struct score_board) * 10);

   if (!(fp = fopen(score_file, "r"))) {
      g_free (score_file);
      return TRUE;
   }

   sc = 0;
   fsc = 0;
   while (fgets(buffer, BUFFER_SIZE, fp))
   {
      g_strchomp(buffer);
      str_val = g_strsplit(buffer, "\t", 4);
      if (str_val[0] && str_val[0][0] &&
         str_val[1] && str_val[1][0] &&
         str_val[2] && str_val[2][0])
      {
         valid = 0;
         if (!str_val[3]) { /* < 2.2.0 file for mat */
            g_rules = rules_get_classic_as_str();
            valid = 1;
         } else {
            if ((valid = rules_check_str(str_val[3]))) {
               g_rules = g_strdup(str_val[3]);
            } else { /* > 2.2.0 && < 3.0.2 file for mat ? */
               tstr_val = g_strsplit(str_val[3], "\t", 5);
               if (tstr_val[0] && tstr_val[1] && tstr_val[2] &&
                  tstr_val[3] && tstr_val[4]) {
                  g_rules = rules_conv_3_0_to_str(tstr_val[0], tstr_val[1], tstr_val[2], tstr_val[3], tstr_val[4]);
                  if (g_rules) { /* yes. its < 3.0.2 for mat */
                     valid = 1;
                  } else { /* nope. just piece of shit. */
                     g_rules = g_strdup("");
                  }
               }
               g_strfreev(tstr_val);
            }
         }
         if (valid && g_ascii_strcasecmp(str_val[0], "<none>")) {
            if (rules_is_current_str(g_rules)) {
               if (g_utf8_validate(str_val[0], -1, NULL)) {
                  tstr = g_strdup(str_val[0]);
               } else {
                  tstr = g_locale_to_utf8(str_val[0], -1, &br, &bw, NULL);
               }
               if (tstr) {
                  strncpy(b[sc].name, tstr, sizeof(b[sc].name)-1);
                  g_free(tstr);
               } else {
                  strncpy(b[sc].name, _("Unknown"), sizeof(b[sc].name)-1);
               }
               b[sc].score = strtol(str_val[1], NULL, 10);
               if ((b[sc].score == INT_MIN) || (b[sc].score == INT_MAX)) {
                  b[sc].score = 0;
               }
               if (g_utf8_validate(str_val[2], -1, NULL)) {
                  tstr = g_strdup(str_val[2]);
               } else {
                  tstr = g_locale_to_utf8(str_val[2], -1, &br, &bw, NULL);
               }
               if (tstr) {
                  strncpy(b[sc].date, tstr, sizeof(b[sc].date)-1);
                  g_free(tstr);
               } else {
                  strncpy(b[sc].date, _("Unknown"), sizeof(b[sc].date)-1);
               }
               sc++;
            } else if (bf) {
               *bf = g_realloc(*bf, sizeof(struct score_board_full) * (fsc + 1));
               strncpy((*bf)[fsc].name, str_val[0], sizeof((*bf)[fsc].name)-1);
               (*bf)[fsc].score = strtol(str_val[1], NULL, 10);
               if (((*bf)[fsc].score == INT_MIN) || ((*bf)[fsc].score == INT_MAX)) {
                  (*bf)[fsc].score = 0;
               }
               strncpy((*bf)[fsc].date, str_val[2], sizeof((*bf)[fsc].date)-1);
               (*bf)[fsc].rules = g_strdup(g_rules);
               fsc++;
            }
         }
         g_free(g_rules);
      }
      g_strfreev(str_val);
   }

   g_free (score_file);
   fclose(fp);

   qsort(b, 10, sizeof(struct score_board), score_sort);

   if (nbf) {
      *nbf = fsc;
   }

   return TRUE;
}


gint insert_entry_in_score_board(struct score_board *board, struct score_board entry)
{
   gint i=0,j;

   if (entry.score <= 0) {
      return -1;
   }

   while (i < 10 && board[i].score > entry.score) {
      i++;
   }

   if (i > 9) {
      return -1;
   }

   for (j = 8;j >= i; j--) {
      strncpy(board[j + 1].name, board[j].name, sizeof(board[j + 1].name)-1);
      strncpy(board[j + 1].date, board[j].date, sizeof(board[j + 1].date)-1);
      board[j + 1].score = board[j].score;
   }

   strncpy(board[i].name, entry.name, sizeof(board[i].name));
   strncpy(board[i].date, entry.date, sizeof(board[i].date));
   board[i].score = entry.score;

   return i;
}

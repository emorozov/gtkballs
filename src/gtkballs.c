/* gtkballs.c - initialisation functions
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 */
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <gtk/gtk.h>

#include "gtkballs.h"
#include "gtkutils.h"
#include "prefs.h" /* preferences */
#include "scoreboard.h" /* read_score, score_setup */
#include "gfx.h"
#include "theme.h"
#include "inputname.h" /* input_name_dialog */
#include "game.h"
#include "mainmenu.h"
#include "mainwin.h"
#include <sys/types.h>
#include <sys/stat.h> /* mkdir */


#define P_DIR "games"
char * get_config_dir_file (const char * file)
{
   /* returns a path that must be freed with g_free */
   char * config_home, * res;
#if __MINGW32__
   config_home = getenv ("LOCALAPPDATA"); /* XP */
   if (!config_home) {
      config_home = getenv ("APPDATA");
   }
#else
   config_home = getenv ("XDG_CONFIG_HOME");
#endif
   if (config_home) {
      res = g_build_filename (config_home, P_DIR, file, NULL);
   } else {
      res = g_build_filename (g_get_home_dir(), ".config", P_DIR, file, NULL);
   }
   return res;
}


gint destroy_lines(gboolean count_score) {
   gint i = game_destroy_lines(count_score);
   if (count_score && i) {
      gint ml = rules_get_destroy();
      gint sc = game_get_score();
      gint nsc = sc + ml * 2 + (i - ml) * (i - ml) * 2 + (1 - pref_get_show_next_colors());
      mw_set_user_score(nsc);
   }
   draw_board();
   return i;
}

void undo_move(GtkWidget *widget, gpointer data) {
   game_restore_state_from_undo();
   stop_jumping_animation();
   mw_set_hi_score(game_get_hi_score());
   mw_set_user_score(game_get_score());
   menu_set_sensitive_undo(FALSE);
   draw_board();
   draw_next_balls();
}

gint random_color(void) {
   return (gint)(((gfloat)rules_get_colors()) * rand() / (RAND_MAX + 1.0));
}

gint random_cell(void) {
   return (gint)(((gfloat)rules_get_width() * rules_get_height()) * rand() / (RAND_MAX + 1.0));
}

void new_turn(gint number, gboolean first)
{
   gint i, k = 0, free_cells_number, c;
   struct score_board scoreboard[10];

   do {
      timer_start();

      k = 0;
      if (number < rules_get_next()) {
         number = rules_get_next();
      }

      if ((free_cells_number = game_count_free_cells()) <= number) {
         if (!read_score(scoreboard, NULL, NULL) ||
            game_get_score() > scoreboard[9].score) {
            input_name_dialog();
         }
         new_game();
         return;
      }

      do {
         i = random_cell();
         if (board_get_at_node(i) < 1) {
            if (first) {
               c = 1 + random_color();
               board_set_at_node(i, c);
            } else {
               board_set_at_node(i, next_get(k));
            }
            k++;
         } else {
            continue;
         }
         /* I hope that k!=0 */
         if (k <= rules_get_next()) {
            next_set(k - 1, 1 + random_color());
         }
      } while (k < ((number < free_cells_number) ? number : free_cells_number));

      if (!first) {
         draw_next_balls();
         /* if line complete, don't increase user's score--it's not his merit */
         destroy_lines(FALSE);
      }
   } while ((free_cells_number = game_count_free_cells()) == rules_get_width() * rules_get_height());
}

void new_game(void)
{
   stop_jumping_animation();
   game_init_game(NULL, NULL);
   menu_set_sensitive_undo(FALSE);
   new_turn(rules_get_destroy(), TRUE);
   mw_set_user_score(0);
   draw_next_balls();
   draw_board();
}

// ==================================================================

int main(int argc, char **argv)
{
   gint i;
   struct timeval tv;
   struct timezone tz;
   struct score_board scoreboard[10];
   gchar *err, *mapfile, * confdir;

   /* setup all i18n stuff */
#ifdef ENABLE_NLS
   bindtextdomain (PACKAGE, LOCALEDIR);
   textdomain (PACKAGE);
   bind_textdomain_codeset(PACKAGE, "UTF8");
#endif /* ENABLE_NLS */

   /* initialize random seed */
   gettimeofday(&tv, &tz);
   srand((unsigned int)tv.tv_usec);

   /* initialize gtk */
   gtk_init (&argc, &argv);

   /* Make sure confdir exists */
   confdir = get_config_dir_file (NULL);
   g_mkdir_with_parents (confdir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
   g_free (confdir);

   /* load user's preferences */
   load_preferences();

   /* load theme, fallback to default if specifed theme cannot be loaded */
   if (!(i = load_theme(pref_get_theme_name()))) {
      if (strcmp(pref_get_theme_name(), pref_get_default_theme_name()) != 0) {
         err = g_strdup_printf(_("Failed loading theme \"%s\"! Trying \"%s\"\n"), pref_get_theme_name(), pref_get_default_theme_name());
         ut_simple_message_box(err);
         g_free(err);
         pref_set_theme_name(pref_get_default_theme_name());
         i = load_theme(pref_get_theme_name());
      }
      if (!i) {
         err = g_strdup_printf(_("Failed loading theme \"%s\"! Exiting.\n"), pref_get_theme_name());
         ut_simple_message_box(err);
         g_free(err);
         return 1;
      }
   }

   /* create main application window */
   mw_create(rules_get_width() * theme_get_width(), rules_get_height() * theme_get_height());

   /* initialize and start new game */
   game_init_game(NULL, NULL);
   new_turn(rules_get_destroy(), TRUE);
   remake_board(-1, 1);

   /* read and set scores */
   read_score (scoreboard, NULL, NULL);
   mw_set_hi_score(scoreboard[0].score);
   mw_set_user_score(0);

   mapfile = g_strconcat(getenv("HOME"), G_DIR_SEPARATOR_S, ".gtkballs",
                         G_DIR_SEPARATOR_S, "accel.map", NULL);
   gtk_accel_map_load (mapfile);

   /* enter main application loop */
   gtk_main();

   gtk_accel_map_save (mapfile);

   return 0;
}

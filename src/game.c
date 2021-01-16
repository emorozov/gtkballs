/* game.c - functions related to game logic
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 */
#include <glib.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include <gtk/gtk.h>
#include "theme.h"
#include "prefs.h"
#include "gfx.h"
#include "mainmenu.h"

typedef struct _GtkbGameRules GtkbGameRules;

struct _GtkbGameRules {
   gint width;   /* width of the boards in "cells" */
   gint height;  /* height of the boards in "cells" */
   gint colors;  /* number of different colors */
   gint next;    /* number of colors appearing on the next turn */
   gint destroy; /* number of same colors in line to destroy */
};

GtkbGameRules _rules = {9, 9, 7, 3, 5};
/* "classic". 9x9 board, 7 colors, 3 new, destroy by 5 */
GtkbGameRules _classic_rules = {9, 9, 7, 3, 5};

gint _save_fmt_len = 10;
gchar _save_fmt[] = "%02u%02u%02u%02u%02u";

/* The board itself */
gint *_board = NULL;
/* copy of board on previous step. stupid. temporary. i hope =)
   (lives here since 2001-10-19. good example of "temporary" stuff  =)
*/
gint *_board_undo = NULL;

gint *_board_destroys = NULL;

/* next colors... */
gint *_next_colors = NULL;
/* copy of next colors on previous step... */
gint *_next_colors_undo = NULL;

gint _timer_limit = -1;
time_t _timer_start_time = -1;

/* current users score */
gint _score = 0;
/* current highest score */
gint _hi_score = 0;
/* copy of scores on previous step.
   also used to indicate "possibility" of undo. -1 means "cannot undo" */
gint _score_undo = -1;
gint _hi_score_undo = -1;

gint _actions_locked = 0;

gint is_actions_locked(void) {
   return _actions_locked;
}

void lock_actions(gint lock) {
   _actions_locked = lock;
   menu_set_sensitive_all(1 - lock);
}

gint rules_get_width(void) {
   return _rules.width;
}

gint rules_get_height(void) {
   return _rules.height;
}

gint rules_get_colors(void) {
   return _rules.colors;
}

gint rules_get_next(void) {
   return _rules.next;
}

gint rules_get_destroy(void) {
   return _rules.destroy;
}

gint rules_get_classic_width(void) {
   return _classic_rules.width;
}

gint rules_get_classic_height(void) {
   return _classic_rules.height;
}

gint rules_get_classic_colors(void) {
   return _classic_rules.colors;
}

gint rules_get_classic_next(void) {
   return _classic_rules.next;
}

gint rules_get_classic_destroy(void) {
   return _classic_rules.destroy;
}

void rules_set_width(gint width) {
   _rules.width = width;
}

void rules_set_height(gint height) {
   _rules.height = height;
}

void rules_set_colors(gint colors) {
   _rules.colors = colors;
}

void rules_set_next(gint next) {
   _rules.next = next;
}

void rules_set_destroy(gint destroy) {
   _rules.destroy = destroy;
}

void rules_set(gint width, gint height, gint colors, gint next, gint destroy) {
   _rules.width = width;
   _rules.height = height;
   _rules.colors = colors;
   _rules.next = next;
   _rules.destroy = destroy;
}

gboolean _rules_check_rules(GtkbGameRules r) {
   if ((r.width   < 4 || r.width   > 99)
    || (r.height  < 4 || r.height  > 99)
    || (r.colors  < 3 || r.colors  > 99)
    || (r.next    < 2 || r.next    > 99)
    || (r.destroy < 3 || r.destroy > 99))
   {
      return 0;
   }
   return 1;
}

gchar *_rules_to_str(GtkbGameRules r) {
   if (!_rules_check_rules(r)) {
      return NULL;
   }
   return g_strdup_printf(_save_fmt, r.width, r.height, r.colors, r.next, r.destroy);
}

gchar *rules_get_classic_as_str() {
   return _rules_to_str(_classic_rules);
}

gchar *rules_get_as_str() {
   return _rules_to_str(_rules);
}

gint rules_get_str_len(void) {
   return _save_fmt_len;
}

gboolean rules_check_str(gchar *rstr) {
   GtkbGameRules r;

   if (strlen(rstr) != _save_fmt_len ||
       strstr(rstr, " ") ||
       strstr(rstr, "\t") ||
       strstr(rstr, "\r") ||
       strstr(rstr, "\n") ||
       sscanf(rstr, _save_fmt, &r.width, &r.height, &r.colors, &r.next, &r.destroy) != 5 ||
       !_rules_check_rules(r))
   {
      return 0;
   }
   return 1;
}

gboolean rules_get_from_str(gchar *s, gint *width, gint *height, gint *colors, gint *next, gint *destroy) {
   if (!rules_check_str(s)) {
      return 0;
   }
   sscanf(s, _save_fmt, width, height, colors, next, destroy);
   return 1;
}

gchar *rules_conv_3_0_to_str(gchar *w, gchar *h, gchar *c, gchar *n, gchar *d) {
   GtkbGameRules r;

   if (!w[0] || !h[0] || !c[0] || !n[0] || !d[0]) {
      return NULL;
   }
   r.width   = strtol(w, NULL, 10);
   r.height  = strtol(h, NULL, 10);
   r.colors  = strtol(c, NULL, 10);
   r.next    = strtol(n, NULL, 10);
   r.destroy = strtol(d, NULL, 10);

   return _rules_to_str(r);
}

gboolean rules_is_current_str(gchar *r) {
   gboolean rval = 0;
   gchar *cur = _rules_to_str(_rules);

   if (strcmp(r, cur) == 0) {
      rval = 1;
   }
   g_free(cur);

   return rval;
}

gint game_get_score(void) {
   return _score;
}

void game_set_score(gint score) {
   _score = score;
}

gint game_get_hi_score(void) {
   return _hi_score;
}

void game_set_hi_score(gint score) {
   _hi_score = score;
}

void game_save_state_for_undo(void) {
   memcpy(_board_undo, _board, sizeof(gint) * _rules.width * _rules.height);
   memcpy(_next_colors_undo, _next_colors, sizeof(gint) * _rules.next);
   _score_undo = _score;
   _hi_score_undo = _hi_score;
}

void game_restore_state_from_undo(void) {
   if (_score_undo == -1 || _hi_score_undo == -1) {
      /* cannot undo */
      return;
   }
   memcpy(_board, _board_undo, sizeof(gint) * _rules.width * _rules.height);
   memcpy(_next_colors, _next_colors_undo, sizeof(gint) * _rules.next);
   _score = _score_undo;
   _hi_score = _hi_score_undo;
}

void game_init_game(gint *balls, gint *nextballs)
{
   if (_board) {
      g_free(_board);
   }
   if (_board_undo) {
      g_free(_board_undo);
   }
   if (_board_destroys) {
      g_free(_board_destroys);
   }
   if (_next_colors) {
      g_free(_next_colors);
   }
   if (_next_colors_undo) {
      g_free(_next_colors_undo);
   }
   _board = g_malloc0(sizeof(gint) * _rules.width * _rules.height);
   _board_destroys = g_malloc0(sizeof(gint) * _rules.width * _rules.height);
   if (balls) {
      memcpy(_board, balls, sizeof(gint) * _rules.width * _rules.height);
   }
   _board_undo = g_malloc0(sizeof(gint) * _rules.width * _rules.height);
   _next_colors = g_malloc0(sizeof(gint) * _rules.next);
   if (nextballs) {
      memcpy(_next_colors, nextballs, sizeof(gint) * _rules.next);
   }
   _next_colors_undo = g_malloc0(sizeof(gint) * _rules.next);
   _score = 0;
   _score_undo = -1;
   _hi_score_undo = -1;
}

gint game_count_free_cells(void) {
   gint i, counter = 0;
   gint *bp = _board;

   for (i = 0; i < _rules.width * _rules.height; i++) {
      if (*bp++ == 0)
         counter++;
   }
   return counter;
}

gint *game_get_board_as_int_arr(void) {
   gint *b;

   b = g_malloc(sizeof(gint) * _rules.width * _rules.height);
   memcpy(b, _board, sizeof(gint) * _rules.width * _rules.height);
   return b;
}

gint *game_get_next_as_int_arr(void) {
   gint *n;

   n = g_malloc(sizeof(gint) * _rules.next);
   memcpy(n, _next_colors, sizeof(gint) * _rules.next);
   return n;
}

gint board_get_at_node(gint node) {
   if (node >= _rules.width * _rules.height) {
      return 0;
   }
   return _board[node];
}

gint board_get_at_xy(gint x, gint y) {
   if (x >= _rules.width || y > _rules.height) {
      return 0;
   }
   return _board[y * _rules.width + x];
}

gint board_get_destroy_at_xy(gint x, gint y) {
   if (x >= _rules.width || y > _rules.height) {
      return 0;
   }
   return _board_destroys[y * _rules.width + x];
}

void board_set_at_node(gint node, gint col) {
   if (node >= _rules.width * _rules.height) {
      return;
   }
   _board[node] = col;
}

void board_set_at_xy(gint x, gint y, gint col) {
   if (x >= _rules.width || y > _rules.height) {
      return;
   }
   _board[y * _rules.width + x] = col;
}

gint next_get(gint num) {
   if (num >= _rules.next) {
      return 0;
   }
   return _next_colors[num];
}

void next_set(gint num, gint col) {
   if (num >= _rules.next) {
      return;
   }
   _next_colors[num] = col;
}

void timer_start(void) {
   _timer_start_time = time(NULL);
}

gboolean timer_is_running(void) {
   if (_timer_start_time == -1 || _timer_limit <= 0) {
      return 0;
   }
   return 1;
}

gboolean timer_is_expired(void) {
   time_t nowt = time(NULL);

   if (nowt - _timer_start_time >= _timer_limit) {
      return 1;
   }
   return 0;
}

gint timer_get_remaining(void) {
   time_t nowt = time (NULL);

   return _timer_limit + _timer_start_time - nowt;
}

void timer_set_limit(gint limit) {
   _timer_limit =limit;
}

gint timer_get_limit(void) {
   return _timer_limit;
}

struct gtkb_animarray {
   gint color, x, y, phase, time;
};

int animsort(const void *a, const void *b)
{
   if (((const struct gtkb_animarray *)a)->time == ((const struct gtkb_animarray *)b)->time) {
      return 0;
   }
   if (((const struct gtkb_animarray *)a)->time > ((const struct gtkb_animarray *)b)->time) {
      return 1;
   }
   return -1;
}


gint game_destroy_lines(gboolean count_score)
{
   gint x, y, i, j;
   gint *del, have_del = 0;

   if (rules_get_width() < rules_get_destroy() &&
      rules_get_height() < rules_get_destroy()) { /* destroy is impossible */
         return 0;
   }

   del = g_malloc0(rules_get_width() * rules_get_height() * sizeof(gint));

   for (y = 0; y < rules_get_height(); y++)
   {
      for (x = 0; x < rules_get_width(); x++)
      {
         if (board_get_at_xy(x, y) != 0) {
            /* horizontal */
            if (rules_get_width() - x >= rules_get_destroy()) {
               for (i = 1;
                  i < rules_get_width() - x &&
                  board_get_at_xy(x + i, y) == board_get_at_xy(x, y);
                  i++);
               if (i >= rules_get_destroy()) {
                  have_del = 1;
                  for (j = 0; j < i; j ++) {
                     del[y * rules_get_width() + x + j] = 1;
                  }
               }
            }
            /* vertical */
            if (rules_get_height() - y >= rules_get_destroy()) {
               for (i = 1;
                  i < rules_get_height() - y &&
                  board_get_at_xy(x, y + i) == board_get_at_xy(x, y);
                  i++);
               if (i >= rules_get_destroy()) {
                  have_del = 1;
                  for (j = 0; j < i; j ++) {
                     del[(y + j) * rules_get_width() + x] = 1;
                  }
               }
            }
            /* diagonal left -> right */
            if (rules_get_width() - x >= rules_get_destroy() &&
               rules_get_height() - y >= rules_get_destroy()) {
               for (i = 1;
                  i < rules_get_width() - x &&
                  i < rules_get_height() - y &&
                  board_get_at_xy(x + i, y + i) == board_get_at_xy(x, y);
                  i++);
               if (i >= rules_get_destroy()) {
                  have_del = 1;
                  for (j = 0; j < i; j ++) {
                     del[(y + j) * rules_get_width() + x + j] = 1;
                  }
               }
            }
            /* diagonal right -> left */
            if (x + 1 >= rules_get_destroy() &&
               rules_get_height() - y >= rules_get_destroy()) {
               for (i = 1;
                  i <= x &&
                  i < rules_get_height() - y &&
                  board_get_at_xy(x - i, y + i) == board_get_at_xy(x, y);
                  i++);
               if (i >= rules_get_destroy()) {
                  have_del = 1;
                  for (j = 0; j < i; j ++) {
                     del[(y + j) * rules_get_width() + x - j] = 1;
                  }
               }
            }
         }
      }
   }

   i = 0;
   if (have_del)
   {
      if (pref_get_show_destroy())
      {
         gint animcadres = 0, animpos = 0, animtime;
         struct gtkb_animarray *animarray;
         struct timeval tvs, tve;

         animarray = g_new0(struct gtkb_animarray, rules_get_width() * rules_get_height() * gtkbTheme->maxdestphases);
         for (y = 0; y < rules_get_height(); y++)
         {
            for (x = 0; x < rules_get_width(); x++)
            {
               if (del[y * rules_get_width() + x] == 1) {
                  gint color = board_get_at_xy(x, y);

                  animcadres += gtkbTheme->balls[color - 1].destroyphases + 1;
                  for (j = 0, animtime = 0; j <= gtkbTheme->balls[color - 1].destroyphases; j++) {
                     if (j != gtkbTheme->balls[color - 1].destroyphases) {
                        animarray[animpos].color = color;
                        animarray[animpos].phase = j;
                        animtime += gtkbTheme->balls[color - 1].destroydelays[j];
                     } else {
                        animarray[animpos].color = 0;
                        animarray[animpos].phase = 0;
                     }
                     animarray[animpos].x = x;
                     animarray[animpos].y = y;
                     animarray[animpos].time = animtime;
                     animpos++;
                  }
               }
            }
         }
         qsort(animarray, animcadres, sizeof(struct gtkb_animarray), animsort);
         lock_actions(1);
         draw_board();
         for (animtime = 0, i = 0; i < animcadres;)
         {
            gettimeofday(&tvs, NULL);
            gint isav = i;
            for (; animtime == animarray[i].time && i < animcadres; i++) {
               draw_ball(animarray[i].color, animarray[i].x, animarray[i].y, 0, animarray[i].phase + 1);
               _board_destroys[animarray[i].y * rules_get_width() + animarray[i].x] = animarray[i].phase + 1;
            }
            do {
               gtk_main_iteration_do(0);
               gettimeofday(&tve, NULL);
            } while((tve.tv_sec  - tvs.tv_sec) * 1000000 + tve.tv_usec - tvs.tv_usec < (animarray[i].time - animtime) * 1000);
            animtime = animarray[isav].time;
         }
         g_free(animarray);
         memset(_board_destroys, 0, rules_get_width() * rules_get_height() * sizeof(gint));
         lock_actions(0);
      }

      for (i = 0, y = 0; y < rules_get_height(); y++) {
         for (x = 0; x < rules_get_width(); x++) {
            if (del[y * rules_get_width() + x] == 1) {
               i++;
               board_set_at_xy(x, y, 0);
            }
         }
      }
   }

   g_free(del);
   return i;
}

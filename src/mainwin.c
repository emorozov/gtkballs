/* mainwin.c all stuff related to main programm window
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 */
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#include "gtkballs.h" /* _() */
#include "gfx.h" /* move_pointer*, pointer_pressed */
#include "theme.h" /* gtkb_theme_get_coord_at_* */
#include "gtkutils.h"
#include "mainmenu.h" /* menu_* */
#include "game.h"

GtkWindow * main_window;

/* Score labels */
GtkWidget *_hi_score_label = NULL;
GtkWidget *_user_score_label = NULL;

/* box for next balls */
GtkWidget *_small_balls_box;

/* board */
GtkWidget *_drawing_area = NULL;

GtkWidget *mw_get_da(void) {
   return _drawing_area;
}

/* Backing pixmap for drawing area */
cairo_surface_t * pixsurf = NULL;


/* add child widget to "next balls" display area */
void mw_small_balls_add(GtkWidget *child) {
   gtk_box_pack_start(GTK_BOX(_small_balls_box), child, FALSE, FALSE, 0);
}

void mw_show_hide_next_balls(gboolean show) {
   if (show) {
      gtk_widget_show_all(_small_balls_box);
   } else {
      gtk_widget_hide(_small_balls_box);
   }
}

/* set and draw High Score */
void mw_set_hi_score(gint score) {
   gchar *str;

   game_set_hi_score(score);
   str = g_strdup_printf(_("Hi-score: %i"), score);
   gtk_label_set_text(GTK_LABEL(_hi_score_label), str);
   g_free(str);
}

/* set and draw Users Score */
void mw_set_user_score(gint score) {
   gchar *str;

   game_set_score(score);
   if (score > game_get_hi_score()) {
      mw_set_hi_score(score);
   }
   str = g_strdup_printf(_("Your score: %i"), score);
   gtk_label_set_text(GTK_LABEL(_user_score_label), str);
   g_free(str);
}

gboolean _countdown_timer(gpointer data)
{
   gchar *text;
   GtkLabel *label = data;
   gint trem;

   if (!timer_is_running()) {
      gtk_label_set_text(label, _("Time is unlimited"));
      return TRUE;
   }
   if (timer_is_expired()) {
      new_turn(rules_get_next(), FALSE);
   }
   trem = timer_get_remaining();
   text = g_strdup_printf(_("Remaining time: %02d:%02d"), trem / 60, trem % 60);
   gtk_label_set_text(label, text);
   g_free(text);

   return TRUE;
}

gint _user_action_event(GtkWidget *w, GdkEvent *ev)
{
   if (ev->type == GDK_MOTION_NOTIFY) {
      move_pointer_to(gtkb_theme_get_coord_at_x(ev->motion.x),
      gtkb_theme_get_coord_at_y(ev->motion.y));
   }

   if (ev->type == GDK_KEY_PRESS) {
      if (ev->key.keyval == GDK_KEY_Left || ev->key.keyval == GDK_KEY_KP_Left) {
         move_pointer(DIR_LEFT);
      } else if (ev->key.keyval == GDK_KEY_Right || ev->key.keyval == GDK_KEY_KP_Right) {
         move_pointer(DIR_RIGHT);
      } else if (ev->key.keyval == GDK_KEY_Up || ev->key.keyval == GDK_KEY_KP_Up) {
         move_pointer(DIR_UP);
      } else if (ev->key.keyval == GDK_KEY_Down || ev->key.keyval == GDK_KEY_KP_Down) {
         move_pointer(DIR_DOWN);
      } else if (ev->key.keyval == GDK_KEY_KP_Home) {
         move_pointer(DIR_UP_LEFT);
      } else if (ev->key.keyval == GDK_KEY_KP_Page_Up) {
         move_pointer(DIR_UP_RIGHT);
      } else if (ev->key.keyval == GDK_KEY_KP_End) {
         move_pointer(DIR_DOWN_LEFT);
      } else if (ev->key.keyval == GDK_KEY_KP_Page_Down) {
         move_pointer(DIR_DOWN_RIGHT);
      } else if (ev->key.keyval == GDK_KEY_Return ||
         ev->key.keyval == GDK_KEY_KP_Space ||
         ev->key.keyval == GDK_KEY_KP_Enter ||
         ev->key.keyval == GDK_KEY_space) {
         if (is_actions_locked()) {
            return FALSE;
         }
         pointer_pressed(-1, -1);
      }
   } else if (ev->type == GDK_BUTTON_PRESS && ev->button.button == 1) {
      if (is_actions_locked()) {
         return FALSE;
      }
      pointer_pressed(gtkb_theme_get_coord_at_x(ev->button.x),
      gtkb_theme_get_coord_at_y(ev->button.y));
   }

   return FALSE;
}


static void main_window_destroy_cb (GtkWidget * w, gpointer user_data)
{
   if (pixsurf) {
      cairo_surface_destroy (pixsurf);
      pixsurf = NULL;
   }
   gtkb_theme_free_handler (NULL, NULL);
   gtk_main_quit ();
}


void mw_create(gint da_width, gint da_height)
{
   GtkWidget * mainwin;
   GtkWidget * menubar;
   GtkWidget * vbox, * hbox, * hbox1, * drawing_area_box;
   GtkWidget * timer_label;
   GdkPixbuf * icon;
   GError *error = NULL;

   mainwin = ut_window_new (_("GtkBalls"), "GtkBalls_Main", "GtkBalls", FALSE, FALSE, FALSE, 0);
   main_window = GTK_WINDOW (mainwin);
   g_signal_connect (G_OBJECT (mainwin), "destroy", G_CALLBACK(main_window_destroy_cb), mainwin);

   icon = gdk_pixbuf_new_from_file (DATADIR "/gtkballs/gtkballs_16x16.png", &error);
   if (icon) {
      gtk_window_set_icon (GTK_WINDOW(mainwin), icon);
   } else {
      g_error_free (error);
   }

   vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 5);
   gtk_container_add (GTK_CONTAINER(mainwin), vbox);

   menu_get_main (mainwin, &menubar);
   menu_set_sensitive_undo (FALSE);
   gtk_box_pack_start (GTK_BOX(vbox), menubar, FALSE, TRUE, 0);

   hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
   _hi_score_label = gtk_label_new ("");
   gtk_box_pack_start (GTK_BOX(hbox), _hi_score_label, FALSE, FALSE, 5);
   _user_score_label = gtk_label_new("");
   gtk_box_pack_end (GTK_BOX(hbox), _user_score_label, FALSE, FALSE, 5);
   gtk_box_pack_start (GTK_BOX(vbox), GTK_WIDGET(hbox), FALSE, FALSE, 0);

   _small_balls_box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
   gtk_box_set_homogeneous (GTK_BOX (_small_balls_box), TRUE);
   gtk_box_pack_start (GTK_BOX(hbox), _small_balls_box, TRUE, FALSE, 0);

   hbox1 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
   timer_label = gtk_label_new (NULL);
   gtk_box_pack_start (GTK_BOX(hbox1), timer_label, TRUE, TRUE, 5);
   g_timeout_add(250, _countdown_timer, timer_label);
   gtk_box_pack_start (GTK_BOX(vbox), GTK_WIDGET(hbox1), FALSE, FALSE, 0);

   drawing_area_box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
   gtk_box_pack_start (GTK_BOX(vbox), drawing_area_box, FALSE, FALSE, 10);
   _drawing_area = gtk_drawing_area_new();
   gtk_widget_set_size_request (_drawing_area, da_width, da_height);
   gtk_box_pack_start (GTK_BOX(drawing_area_box), _drawing_area, TRUE, FALSE, 10);
   gtk_widget_set_events (_drawing_area, GDK_EXPOSURE_MASK | GDK_BUTTON_PRESS_MASK | GDK_KEY_PRESS_MASK | GDK_POINTER_MOTION_MASK);
   g_signal_connect (G_OBJECT(_drawing_area), GTKCOMPAT_DRAW_SIGNAL, G_CALLBACK(boardw_draw_event), NULL);
   g_signal_connect (G_OBJECT(_drawing_area), "button_press_event", G_CALLBACK(_user_action_event), NULL);
   g_signal_connect (G_OBJECT(_drawing_area), "motion_notify_event", G_CALLBACK(_user_action_event), NULL);
   /* FIXME: imho catching keypress on whole window is stupid... */
   g_signal_connect (G_OBJECT(mainwin), "key_press_event", G_CALLBACK(_user_action_event), NULL);

   gtk_widget_show_all (mainwin);
}

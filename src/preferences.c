/* preferences.c - preferences dialog
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 */
#include <gtk/gtk.h>
#include <string.h>

#include "gtkballs.h"
#include "gtkutils.h"
#include "prefs.h"
#include "gfx.h"
#include "theme.h"
#include "game.h"
#include "mainwin.h"

enum
{
   PR_DIALOG,
   PR_SHOW_NEXT,
   PR_SHOW_PATH,
   PR_SHOW_PAWS,
   PR_SHOW_ANIM,
   PR_SHOW_HL,
   PR_HL_DR,
   PR_HL_DG,
   PR_HL_DB,
   PR_TIME_LIMIT,
   PR_TIME_VALUE,
   PR_THEME_LIST,
   PR_SIZE
};

void parent_toggled(GtkToggleButton *togglebutton, gpointer data) {
   gtk_widget_set_sensitive(GTK_WIDGET(data), gtk_toggle_button_get_active(togglebutton));
}

void preferences_destroy_cb(GtkWidget *widget, gpointer data) {
   g_free(data);
}

void preferences_cancel(GtkWidget *widget, gpointer data) {
   GtkWidget **buttons = data;
   gtk_widget_destroy(buttons[PR_DIALOG]);
}

gboolean fix_draw_next_balls(gpointer data) {
   draw_next_balls();
   return FALSE;
}


void preferences_apply(GtkWidget *widget, gpointer data)
{
   GtkWidget **buttons = data;
   GtkTreeModel *model;
   GtkTreeIter iter;
   gchar *msg, *save, *themename;

   gtk_tree_selection_get_selected(gtk_tree_view_get_selection(GTK_TREE_VIEW(buttons[PR_THEME_LIST])), &model, &iter);
   gtk_tree_model_get(model, &iter, 0, &themename, -1);

   if (strcmp(themename, pref_get_theme_name()) != 0) {
      save = g_strdup(pref_get_theme_name());
      pref_set_theme_name(themename);
      if (!load_theme(pref_get_theme_name())) {
         msg = g_strconcat(_("Failed loading theme \""), pref_get_theme_name(), "\"!\n", NULL);
         ut_simple_message_box(msg);
         g_free(msg);
         pref_set_theme_name(save);
      } else {
         set_jump_phase(0);
         remake_board(0, 1);
         draw_next_balls();
      }
      g_free(save);
   }
   g_free(themename);

   pref_set_show_next_colors(gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(buttons[PR_SHOW_NEXT])));
   mw_show_hide_next_balls(pref_get_show_next_colors());
   g_timeout_add(50, fix_draw_next_balls, NULL);
   pref_set_show_path(gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(buttons[PR_SHOW_PATH])));
   pref_set_show_footprints(gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(buttons[PR_SHOW_PAWS])));
   pref_set_show_destroy (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(buttons[PR_SHOW_ANIM])));

   pref_set_show_highlight (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(buttons[PR_SHOW_HL])));

   prefs_set_hl_dr(gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON(buttons[PR_HL_DR])));
   prefs_set_hl_dg(gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON(buttons[PR_HL_DG])));
   prefs_set_hl_db(gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON(buttons[PR_HL_DB])));

   gtkb_make_hl_pixmap (gtkbTheme);

   redraw_pointer();
   if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(buttons[PR_TIME_LIMIT]))) {
      timer_start();
      timer_set_limit(gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(buttons[PR_TIME_VALUE])));
   } else {
      timer_set_limit(-1);
   }
   msg = save_preferences();
   if (msg) {
      ut_simple_message_box(msg);
      g_free(msg);
   }
}


void preferences_ok(GtkWidget *widget, gpointer data) {
   GtkWidget **buttons = data;
   preferences_apply(widget, buttons);
   gtk_widget_destroy(buttons[PR_DIALOG]);
}



void preferences_dialog (void)
{
   GtkWidget ** buttons;
   GtkWidget * frame;
   GtkWidget * big_vbox, * vbox, * buttons_box;
   GtkWidget * theme_scrolled_window;
   GtkWidget * separator;
   gint i, st;
   GtkListStore * store;
   GtkTreeIter iter;
   GtkTreePath * path;
   GtkCellRenderer * renderer;
   GtkTreeViewColumn * column;
   gchar * pathstr;
   gchar ** themelist;

   if (!(themelist = get_available_themes())) {
      ut_simple_message_box(_("No themes available! =(\n"));
      return;
   }

   buttons = g_malloc (PR_SIZE * sizeof(GtkWidget));

   buttons[PR_DIALOG] = ut_window_new(_("Preferences"), "GtkBalls_Preferences", "GtkBalls", TRUE, TRUE, TRUE, 5);
   g_signal_connect (G_OBJECT(buttons[PR_DIALOG]), "destroy", G_CALLBACK(preferences_destroy_cb), buttons);

   big_vbox = gtk_vbox_new (FALSE, 0);
   gtk_container_add (GTK_CONTAINER(buttons[PR_DIALOG]), big_vbox);

   frame = gtk_frame_new (_("Preferences"));
   gtk_box_pack_start (GTK_BOX(big_vbox), frame, FALSE, FALSE, 0);

   vbox = gtk_vbox_new (FALSE, 0);
   gtk_container_set_border_width (GTK_CONTAINER(vbox), 5);
   gtk_container_add (GTK_CONTAINER(frame), vbox);

   buttons[PR_SHOW_NEXT] = ut_check_button_new(_("Show colors that will appear on next turn"), pref_get_show_next_colors(), vbox);
   buttons[PR_SHOW_PATH] = ut_check_button_new(_("Show path of the ball"), pref_get_show_path(), vbox);
   buttons[PR_SHOW_PAWS] = ut_check_button_new(_("Show footprints of the ball"), pref_get_show_footprints(), vbox);
   buttons[PR_SHOW_ANIM] = ut_check_button_new(_("Show animation of disappearing of the ball"), pref_get_show_destroy(), vbox);
   buttons[PR_SHOW_HL]   = ut_check_button_new(_("Highlight \"active\" cell"), pref_get_show_highlight(), vbox);

   buttons[PR_HL_DR]     = ut_spin_button_new(_("Highlight red value: "), -128, 128, prefs_get_hl_dr(), vbox);
   buttons[PR_HL_DG]     = ut_spin_button_new(_("Highlight green value: "), -128, 128, prefs_get_hl_dg(), vbox);
   buttons[PR_HL_DB]     = ut_spin_button_new(_("Highlight blue value: "), -128, 128, prefs_get_hl_db(), vbox);
   g_signal_connect (G_OBJECT(buttons[PR_SHOW_HL]), "toggled", G_CALLBACK(parent_toggled), buttons[PR_HL_DR]);
   g_signal_connect (G_OBJECT(buttons[PR_SHOW_HL]), "toggled", G_CALLBACK(parent_toggled), buttons[PR_HL_DG]);
   g_signal_connect (G_OBJECT(buttons[PR_SHOW_HL]), "toggled", G_CALLBACK(parent_toggled), buttons[PR_HL_DB]);
   gtk_widget_set_sensitive (buttons[PR_HL_DR], pref_get_show_highlight());
   gtk_widget_set_sensitive (buttons[PR_HL_DG], pref_get_show_highlight());
   gtk_widget_set_sensitive (buttons[PR_HL_DB], pref_get_show_highlight());

   buttons[PR_TIME_LIMIT] = ut_check_button_new(_("Enable time limit"), timer_get_limit() > 0 ? 1 : 0, vbox);
   buttons[PR_TIME_VALUE] = ut_spin_button_new(_("Time limit (seconds): "), 1, 3600, timer_get_limit() > 0 ? timer_get_limit() : 60, vbox);
   g_signal_connect (G_OBJECT(buttons[PR_TIME_LIMIT]), "toggled", G_CALLBACK(parent_toggled), buttons[PR_TIME_VALUE]);
   gtk_widget_set_sensitive (buttons[PR_TIME_VALUE], timer_get_limit() > 0 ? 1 : 0);

   theme_scrolled_window = gtk_scrolled_window_new (NULL, NULL);
   gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW(theme_scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
   gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW(theme_scrolled_window), GTK_SHADOW_ETCHED_IN);
   gtk_box_pack_start (GTK_BOX(vbox), theme_scrolled_window, FALSE, FALSE, 5);

   store = gtk_list_store_new (1, G_TYPE_STRING);
   for (i = 0, st = 0; themelist[i] != NULL; i++) {
      gtk_list_store_append (store, &iter);
      gtk_list_store_set (store, &iter, 0, themelist[i], -1);
      if (!strcmp(themelist[i], pref_get_theme_name())) {
         st = i;
      }
      g_free (themelist[i]);
   }
   g_free (themelist);

   buttons[PR_THEME_LIST] = gtk_tree_view_new_with_model (GTK_TREE_MODEL(store));
   gtk_widget_set_size_request (buttons[PR_THEME_LIST], -1, 150);
   g_object_unref (G_OBJECT(store));
   gtk_tree_view_set_rules_hint (GTK_TREE_VIEW(buttons[PR_THEME_LIST]), TRUE);
   gtk_tree_view_set_search_column (GTK_TREE_VIEW(buttons[PR_THEME_LIST]), 0);
   gtk_tree_selection_set_mode (gtk_tree_view_get_selection(GTK_TREE_VIEW(buttons[PR_THEME_LIST])), GTK_SELECTION_BROWSE);
   gtk_container_add (GTK_CONTAINER(theme_scrolled_window), buttons[PR_THEME_LIST]);

   renderer = gtk_cell_renderer_text_new ();
   column = gtk_tree_view_column_new_with_attributes (_("Select Theme"), renderer, "text", 0, NULL);
   gtk_tree_view_column_set_sort_column_id (column, 0);
   gtk_tree_view_append_column (GTK_TREE_VIEW(buttons[PR_THEME_LIST]), column);

   pathstr = g_strdup_printf ("%u", st);
   if ((path = gtk_tree_path_new_from_string (pathstr))) {
      gtk_tree_selection_select_path (gtk_tree_view_get_selection(GTK_TREE_VIEW(buttons[PR_THEME_LIST])), path);
      gtk_tree_view_set_cursor (GTK_TREE_VIEW(buttons[PR_THEME_LIST]), path, NULL, FALSE);
      gtk_tree_view_scroll_to_cell (GTK_TREE_VIEW(buttons[PR_THEME_LIST]), path, NULL, TRUE, 0, 0);
      gtk_tree_path_free (path);
   }
   g_free (pathstr);

   separator = gtk_hseparator_new ();
   gtk_box_pack_start (GTK_BOX(big_vbox), separator, FALSE, FALSE, 5);

   buttons_box = gtk_hbutton_box_new ();
   gtk_button_box_set_layout (GTK_BUTTON_BOX(buttons_box), GTK_BUTTONBOX_SPREAD);
   gtk_box_pack_start (GTK_BOX(big_vbox), buttons_box, TRUE, TRUE, 0);

   gtk_widget_grab_default (ut_button_new_stock(GTK_STOCK_OK, preferences_ok, buttons, buttons_box));
   ut_button_new_stock (GTK_STOCK_CANCEL, preferences_cancel, buttons, buttons_box);
   ut_button_new_stock (GTK_STOCK_APPLY, preferences_apply, buttons, buttons_box);

   gtk_widget_show_all (buttons[PR_DIALOG]);
}

/* rulesdialog.c - game rules settings dialog
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 */
#include <gtk/gtk.h>

#include "gtkballs.h"    /* _() */
#include "gtkutils.h"
#include "theme.h"       /* gtkb_theme_get_balls_num() */
#include "prefs.h" /* save_preferences(); */
#include "gfx.h"
#include "game.h"        /* rules stuff */

struct _gtkb_rules_dialog {
   GtkWidget *xrange, *yrange, *crange, *nrange, *drange;
};

struct _gtkb_rules_dialog _rd;

static void rules_ok (void)
{
   gint oldnext = rules_get_next();
   gchar *msg;

   rules_set_width (gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(_rd.xrange)));
   rules_set_height (gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(_rd.yrange)));
   rules_set_colors (gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(_rd.crange)));
   rules_set_next (gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(_rd.nrange)));
   rules_set_destroy (gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(_rd.drange)));

   reinit_board (NULL, NULL, 0, oldnext);

   msg = save_preferences ();
   if(msg) {
      ut_simple_message_box (msg);
      g_free (msg);
   }
}


#define BTN_CLASSIC_RULES 100

static void rules_dialog_response (GtkDialog * dlg, int response, gpointer user_data)
{
   if (response == BTN_CLASSIC_RULES) {
      gtk_spin_button_set_value (GTK_SPIN_BUTTON(_rd.xrange), rules_get_classic_width());
      gtk_spin_button_set_value (GTK_SPIN_BUTTON(_rd.yrange), rules_get_classic_height());
      gtk_spin_button_set_value (GTK_SPIN_BUTTON(_rd.crange), rules_get_classic_colors());
      gtk_spin_button_set_value (GTK_SPIN_BUTTON(_rd.nrange), rules_get_classic_next());
      gtk_spin_button_set_value (GTK_SPIN_BUTTON(_rd.drange), rules_get_classic_destroy());
      g_signal_stop_emission_by_name (dlg, "response");
      return;
   }
   if (response == GTK_RESPONSE_OK) {
      rules_ok ();
   }
   gtk_widget_destroy (GTK_WIDGET (dlg));
}

void rules_dialog (void)
{
   GtkWidget * dialog;
   GtkWidget * frame;
   GtkWidget * big_vbox, * vbox, * btn;

   dialog = gtkutil_dialog_new (_("Game rules"), main_window, TRUE, &big_vbox);
   vbox   = gtkutil_frame_vbox (_("Game rules"), big_vbox);

   _rd.xrange = ut_spin_button_new (_("Board width"), 4, 99, rules_get_width(), vbox);
   _rd.yrange = ut_spin_button_new (_("Board height"), 4, 99, rules_get_height(), vbox);
   _rd.crange = ut_spin_button_new (_("Number of different objects"),
                                    3, gtkb_theme_get_balls_num(), rules_get_colors(), vbox);
   _rd.nrange = ut_spin_button_new (_("Number of 'next' objects"), 2, 99, rules_get_next(), vbox);
   _rd.drange = ut_spin_button_new (_("How many balls at line eliminate it"),
                                    3, 99, rules_get_destroy(), vbox);

   gtk_dialog_add_button (GTK_DIALOG (dialog), _("Classic rules"), BTN_CLASSIC_RULES);
   gtk_dialog_add_button (GTK_DIALOG (dialog), "gtk-ok",     GTK_RESPONSE_OK);
   gtk_dialog_add_button (GTK_DIALOG (dialog), "gtk-cancel", GTK_RESPONSE_CANCEL);

   gtk_widget_grab_default (gtk_dialog_get_widget_for_response (GTK_DIALOG (dialog), GTK_RESPONSE_OK));
   g_signal_connect (dialog, "response", G_CALLBACK (rules_dialog_response), NULL);
   gtk_widget_show_all (dialog);
}

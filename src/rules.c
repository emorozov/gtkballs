/* rules.c - display dialog box with information about game rules
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 */
#include <gtk/gtk.h>

#include "gtkballs.h" /* _() */
#include "gtkutils.h"

/* Show dialog box with game rules*/
void show_rules (GtkWidget *widget, gpointer data)
{
   GtkWidget * dialog;
   GtkWidget * main_vbox, * vbox, * hbox;
   GtkWidget * label;

   dialog = gtkutil_dialog_new (_("Rules"), main_window, TRUE, &main_vbox);
   vbox   = gtkutil_frame_vbox (_("Rules"), main_vbox);

   hbox = gtk_hbox_new (FALSE, 0);
   gtk_box_pack_start (GTK_BOX (vbox), hbox, TRUE, TRUE, 0);

   label = gtk_label_new (_("The standard play area of GtkBalls is a 9x9\n" \
                         "grid (it can be changed through \"Rules\"\n" \
                         "option in ingame menu). When GtkBalls is\n" \
                         "first started a number of balls are placed\n" \
                         "in a random manner on the grid. Balls are\n" \
                         "in different colors (and/or shape). You may\n" \
                         "move balls on the grid by selecting them and\n" \
                         "selecting an empty square on the grid as\n" \
                         "destination for the ball you selected. Balls\n" \
                         "will only move if they are not blocked by\n" \
                         "other balls. Each time you move a ball,\n" \
                         "unless you score some points, new balls\n" \
                         "appear in a random manner on the grid. If\n" \
                         "the grid is full then the game is lost.\n" \
                         "However this is bound to happen, your goal\n" \
                         "is to make the highest score. In order to do\n" \
                         "this you must make lines in a vertical,\n" \
                         "horizontal or diagonal way with balls of the\n" \
                         "same color. By default a line must have at\n" \
                         "least five balls of the same color (it can\n" \
                         "also be changed through \"Rules\" option in\n" \
                         "ingame menu). Making a line of minimum amount\n" \
                         "of the balls, specifed in rules, will earn you\n" \
                         "twice the points for your score. If you make\n" \
                         "lines over minimum amount of the balls it will\n" \
                         "earn you even more points. In order to help you\n" \
                         "decide which ball you are going to move, there\n" \
                         "is an indicator on top of the grid that shows\n" \
                         "what colors the next balls that will appear on\n" \
                         "the grid will be."));
   gtk_box_pack_start (GTK_BOX(hbox), label, TRUE, TRUE, 5);

   gtk_dialog_add_button (GTK_DIALOG (dialog), "gtk-close", GTK_RESPONSE_OK);
   g_signal_connect_swapped (dialog, "response", G_CALLBACK (gtk_widget_destroy), dialog);

   gtk_widget_show_all (dialog);
}

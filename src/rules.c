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
void show_rules(GtkWidget *widget, gpointer data) {
    GtkWidget *window;
    GtkWidget *vbox, *hbox, *button_box;
    GtkWidget *label;
    GtkWidget *frame;
    GtkWidget *separator;
    GtkWidget *ok_button;

    window = ut_window_new(_("Rules"), "GtkBalls_Rules", "GtkBalls", TRUE, FALSE, FALSE, 5);

    vbox = gtk_vbox_new(FALSE, 0);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    frame = gtk_frame_new(_("Rules"));
    gtk_box_pack_start(GTK_BOX(vbox), frame, TRUE, TRUE, 3);

    hbox = gtk_hbox_new(FALSE, 0);
    gtk_container_set_border_width(GTK_CONTAINER(hbox), 10);
    gtk_container_add(GTK_CONTAINER(frame), hbox);

    label = gtk_label_new(_("The standard play area of GtkBalls is a 9x9\n" \
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
    gtk_box_pack_start(GTK_BOX(hbox), label, TRUE, TRUE, 5);

    separator = gtk_hseparator_new();
    gtk_box_pack_start(GTK_BOX(vbox), separator, FALSE, FALSE, 5);

    button_box = gtk_hbox_new(FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), button_box, TRUE, TRUE, 2);

    ok_button = ut_button_new_stock_swap(GTK_STOCK_CLOSE, gtk_widget_destroy, window, button_box);

    gtk_widget_grab_default(ok_button);
    gtk_widget_show_all(window);
}

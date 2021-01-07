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

void rules_ok(GtkWidget *widget, gpointer data) {
    gint oldnext = rules_get_next();
    gchar *msg;

    rules_set_width(gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(_rd.xrange)));
    rules_set_height(gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(_rd.yrange)));
    rules_set_colors(gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(_rd.crange)));
    rules_set_next(gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(_rd.nrange)));
    rules_set_destroy(gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(_rd.drange)));

    reinit_board(NULL, NULL, 0, oldnext);

    if (data) {
        gtk_widget_destroy(GTK_WIDGET(data));
    }
    msg = save_preferences();
    if (msg) {
        ut_simple_message_box(msg);
        g_free(msg);
    }
}

void rules_classic(void) {
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(_rd.xrange), rules_get_classic_width());
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(_rd.yrange), rules_get_classic_height());
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(_rd.crange), rules_get_classic_colors());
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(_rd.nrange), rules_get_classic_next());
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(_rd.drange), rules_get_classic_destroy());
}

void rules_dialog(void) {
    GtkWidget *dialog;
    GtkWidget *frame;
    GtkWidget *big_vbox, *vbox, *buttons_box;
    GtkWidget *separator;
    GtkWidget *ok_button, *cancel_button, *classic_button;


    dialog = ut_window_new(_("Game rules"), "GtkBalls_Rules", "GtkBalls", TRUE, TRUE, TRUE, 5);

    big_vbox = gtk_vbox_new(FALSE, 5);
    gtk_container_add(GTK_CONTAINER(dialog), big_vbox);

    frame = gtk_frame_new(_("Game rules"));
    gtk_box_pack_start(GTK_BOX(big_vbox), frame, FALSE, FALSE, 0);

    vbox = gtk_vbox_new(FALSE, 0);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 5);
    gtk_container_add(GTK_CONTAINER(frame), vbox);

    _rd.xrange = ut_spin_button_new(_("Board width"), 4, 99, rules_get_width(), vbox);
    _rd.yrange = ut_spin_button_new(_("Board height"), 4, 99, rules_get_height(), vbox);
    _rd.crange = ut_spin_button_new(_("Number of different objects"),
                                    3, gtkb_theme_get_balls_num(), rules_get_colors(), vbox);
    _rd.nrange = ut_spin_button_new(_("Number of 'next' objects"), 2, 99, rules_get_next(), vbox);
    _rd.drange = ut_spin_button_new(_("How many balls at line eliminate it"),
                                    3, 99, rules_get_destroy(), vbox);

    classic_button = ut_button_new(_("Classic rules"), rules_classic, dialog, big_vbox);

    separator = gtk_hseparator_new();
    gtk_box_pack_start(GTK_BOX(big_vbox), separator, FALSE, FALSE, 5);

    buttons_box = gtk_hbutton_box_new();
    gtk_button_box_set_layout(GTK_BUTTON_BOX(buttons_box), GTK_BUTTONBOX_SPREAD);
    gtk_box_pack_start(GTK_BOX(big_vbox), buttons_box, TRUE, TRUE, 0);

    ok_button = ut_button_new_stock(GTK_STOCK_OK, rules_ok, dialog, buttons_box);
    cancel_button = ut_button_new_stock_swap(GTK_STOCK_CANCEL, gtk_widget_destroy, dialog, buttons_box);

    gtk_widget_grab_default(ok_button);
    gtk_widget_show_all(dialog);
}

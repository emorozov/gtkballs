/* halloffame.c - display dialog box with high scores
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 */
#include <gtk/gtk.h>

#include "gtkballs.h"
#include "gtkutils.h"
#include "scoreboard.h"

void show_hall_of_fame(GtkWidget *widget, gpointer data, struct score_board b[10]) {
    GtkWidget *hall_of_fame;
    GtkWidget *frame, *sw, *tv;
    GtkWidget *vbox, *button_box;
    GtkWidget *close_button;
    gint i;
    struct score_board *bs = b;
    struct score_board bn[10];
    GtkListStore *store;
    GtkTreeIter iter;
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;
    gchar *str;
    GtkTreePath *path;

    if (data == NULL) {
        if (!read_score(bn, NULL, NULL)) {
            ut_simple_message_box(_("Unable to read score.\n"));
            return;
        }
        bs = bn;
    }
    hall_of_fame = ut_window_new(_("Hall of Fame"), "GtkBalls_Scores", "GtkBalls", TRUE, TRUE, FALSE, 5);

    vbox = gtk_vbox_new(FALSE, 0);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 1);
    gtk_container_add(GTK_CONTAINER(hall_of_fame), vbox);

    frame = gtk_frame_new(_("Hall of Fame"));
    gtk_box_pack_start(GTK_BOX(vbox), frame, TRUE, TRUE, 0);

    sw = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_set_border_width(GTK_CONTAINER(sw), 5);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(sw), GTK_POLICY_NEVER, GTK_POLICY_NEVER);
    gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(sw), GTK_SHADOW_ETCHED_IN);
    gtk_container_add(GTK_CONTAINER(frame), sw);

    store = gtk_list_store_new(3, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
    for (i = 0; bs[i].score && i < 10; i++) {
        gtk_list_store_append(store, &iter);
        str = g_strdup_printf("%d", bs[i].score);
        gtk_list_store_set(store, &iter, 0, bs[i].name, 1, str, 2, bs[i].date, -1);
        g_free(str);
    }
    if (i == 0) {
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter, 0, _("No scores"), 1, "", 2, "", -1);
    }

    tv = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
    gtk_tree_view_set_rules_hint(GTK_TREE_VIEW(tv), TRUE);
    gtk_tree_selection_set_mode(gtk_tree_view_get_selection(GTK_TREE_VIEW(tv)), GTK_SELECTION_BROWSE);
    g_object_unref(G_OBJECT(store));
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(_("Name"), renderer, "text", 0, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tv), column);
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(_("Score"), renderer, "text", 1, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tv), column);
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(_("Date"), renderer, "text", 2, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tv), column);
    gtk_container_add(GTK_CONTAINER(sw), tv);

    if (data) {
        str = g_strdup_printf("%u", (gint) data - 1);
        if ((path = gtk_tree_path_new_from_string(str))) {
            gtk_tree_selection_select_path(gtk_tree_view_get_selection(GTK_TREE_VIEW(tv)), path);
            gtk_tree_view_set_cursor(GTK_TREE_VIEW(tv), path, NULL, FALSE);
            gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(tv), path, NULL, TRUE, 0, 0);
            gtk_tree_path_free(path);
        }
        g_free(str);
    }

    button_box = gtk_hbox_new(TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), button_box, FALSE, FALSE, 4);

    close_button = ut_button_new_stock_swap(GTK_STOCK_CLOSE, gtk_widget_destroy, hall_of_fame, button_box);

    gtk_widget_grab_focus(close_button);
    gtk_widget_grab_default(close_button);
    gtk_widget_show_all(hall_of_fame);
}

void show_hall_of_fame_cb(void) {
    show_hall_of_fame(NULL, NULL, NULL);
}

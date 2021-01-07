/* savedialog.c - save/load game dialog
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 */
#include <gtk/gtk.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "gtkballs.h"  /* _(), game_*() */
#include "gtkutils.h"
#include "savegame.h"
#include "theme.h"     /* gtkb_theme_get_balls_num() */
#include "gfx.h"       /* reinit_board */
#include "game.h"

gchar *_selected_save_load = NULL;

void do_load_game(GtkWidget *widget, gpointer data) {
    gint score, *board = NULL, *next = NULL, oldnext;
    gchar *errormsg = NULL;
    gchar *rules = NULL;
    gint w, h, c, n, d;

    if (!_selected_save_load) {
        ut_simple_message_box(_("No game selected for load.\n"));
        return;
    }
    if (!parse_save_game(_selected_save_load, &rules, &score, &board, &next)) {
        errormsg = g_strdup_printf(_("Cannot load game from:\n%s\n"), _selected_save_load);
    } else {
        rules_get_from_str(rules, &w, &h, &c, &n, &d);
        if (c > gtkb_theme_get_balls_num()) {
            errormsg = g_strdup_printf(
                    _("Not enough balls(%d) in current theme.\nWe need %d balls.\nLoad another theme and try again."),
                    gtkb_theme_get_balls_num(), c);
            g_free(rules);
            g_free(board);
            g_free(next);
        }
    }
    g_free(_selected_save_load);
    if (errormsg) {
        ut_simple_message_box(errormsg);
        g_free(errormsg);
        return;
    }
    oldnext = rules_get_next();
    rules_set(w, h, c, n, d);
    g_free(rules);
    reinit_board(board, next, score, oldnext);
    g_free(board);
    g_free(next);
    if (data) {
        gtk_widget_destroy(GTK_WIDGET(data));
    }
}

void do_delete_game(GtkWidget *widget, GtkWidget *treeview) {
    GtkTreeModel *model;
    GtkTreeIter iter, nextiter;
    GValue value = {0,};

    if (gtk_tree_selection_get_selected(gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview)),
                                        &model, &iter)) {
        gtk_tree_model_get_value(model, &iter, 1, &value);
        if (g_value_get_string(&value)) {
            unlink(g_value_get_string(&value));
            gtk_list_store_remove(GTK_LIST_STORE(model), &iter);
            if (iter.stamp == 0) {
                if (gtk_tree_model_get_iter_first(model, &iter)) {
                    do {
                        nextiter = iter;
                    } while (gtk_tree_model_iter_next(model, &iter));
                    iter = nextiter;
                }
            }
            if (iter.stamp != 0) {
                gtk_tree_selection_select_iter(gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview)), &iter);
            }
        }
        g_value_unset(&value);
    }
}

void do_save_game(GtkWidget *widget, gpointer data) {
    gchar *fname = NULL, *errormsg, *rules;
    gint *b, *n;

    if (_selected_save_load) {
        /* TODO: alert stupid user about erasing file... */
        unlink(_selected_save_load);
        g_free(_selected_save_load);
    }

    rules = rules_get_as_str();
    b = game_get_board_as_int_arr();
    n = game_get_next_as_int_arr();
    fname = save_game(rules, game_get_score(), b, n);
    g_free(n);
    g_free(b);
    g_free(rules);

    if (fname != NULL) {
        errormsg = g_strdup_printf(_("Cannot save game to:\n%s\n%s"), fname, strerror(errno));
        ut_simple_message_box(errormsg);
        g_free(fname);
        g_free(errormsg);
    }
    if (data) {
        gtk_widget_destroy(GTK_WIDGET(data));
    }
}

void save_row_activated_cb(GtkTreeView *treeview, gpointer arg1, GtkTreeViewColumn *arg2, gpointer data) {
    do_save_game(GTK_WIDGET(treeview), data);
}

void load_row_activated_cb(GtkTreeView *treeview, gpointer arg1, GtkTreeViewColumn *arg2, gpointer data) {
    do_load_game(GTK_WIDGET(treeview), data);
}

void sl_row_activated(GtkTreeSelection *selection, GtkTreeModel *model) {
    GtkTreeIter iter;
    GValue value = {0,};

    if (gtk_tree_selection_get_selected(selection, NULL, &iter)) {
        if (_selected_save_load) {
            g_free(_selected_save_load);
        }
        gtk_tree_model_get_value(model, &iter, 1, &value);
        if (g_value_get_string(&value)) {
            _selected_save_load = g_strdup((gchar *) g_value_get_string(&value));
        } else {
            _selected_save_load = NULL;
        }
        g_value_unset(&value);
    }
}

void free_gamelist(gchar **gamelist, gint num) {
    gint i;

    for (i = 0; i < num * 2; i++) {
        g_free(gamelist[i]);
    }
    g_free(gamelist);
}

gint game_compare_func(GtkTreeModel *model, GtkTreeIter *a, GtkTreeIter *b, gpointer user_data) {
    GValue value_a = {0,};
    GValue value_b = {0,};
    gchar *str_a, *str_b;
    gint r = 0, i;
    gint sort_pos[] = {6, 7, 8, 9, 3, 4, 0, 1, 11, 12, 14, 15, 17, 18};

    gtk_tree_model_get_value(model, a, 0, &value_a);
    gtk_tree_model_get_value(model, b, 0, &value_b);
    if ((str_a = (gchar *) g_value_get_string(&value_a)) && (str_b = (gchar *) g_value_get_string(&value_b))) {
        for (i = 0; i < sizeof(sort_pos) / sizeof(gint); i++) {
            if (str_a[sort_pos[i]] != str_b[sort_pos[i]]) {
                r = str_a[sort_pos[i]] < str_b[sort_pos[i]] ? 1 : -1;
                break;
            }
        }
    }
    g_value_unset(&value_a);
    g_value_unset(&value_b);

    return r;
}

void save_load_game_dialog(gboolean is_save) {
    GtkListStore *store;
    GtkTreeIter iter;
    GtkWidget *treeview;
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;
    GtkTreePath *path;
    GtkWidget *window, *swindow;
    GtkWidget *vbox, *button_box;
    GtkWidget *ok_button, *cancel_button, *delete_button;
    gint i, num;
    gchar **gamelist, str1[20], *str2;

    _selected_save_load = NULL;

    num = get_saved_games(&gamelist);
    if (!is_save && !num) {
        ut_simple_message_box(_("No saved games found.\n"));
        return;
    }

    window = ut_window_new(is_save ? _("Save game") : _("Load game"),
                           is_save ? "GtkBalls_Save" : "GtkBalls_Load",
                           "GtkBalls", TRUE, TRUE, TRUE, 5);

    vbox = gtk_vbox_new(FALSE, 0);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 1);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    swindow = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(swindow), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(swindow), GTK_SHADOW_ETCHED_IN);
    gtk_box_pack_start(GTK_BOX(vbox), swindow, TRUE, TRUE, 0);

    store = gtk_list_store_new(3, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
    for (i = 0; i < num; i++) {
        gtk_list_store_append(store, &iter);
        memcpy(str1, gamelist[i * 2], 19 * sizeof(gchar));
        str1[19] = '\0';
        str2 = g_strndup(gamelist[i * 2] + 21 * sizeof(gchar),
                         (strlen(gamelist[i * 2]) - 22) * sizeof(gchar));
        gtk_list_store_set(store, &iter, 0, str1, 1, (GValue *) gamelist[i * 2 + 1], 2, str2, -1);
        g_free(str2);
    }
    if (is_save) {
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter, 0, _("Empty"), -1);
    }
    free_gamelist(gamelist, num);

    treeview = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
    gtk_tree_view_set_rules_hint(GTK_TREE_VIEW(treeview), TRUE);
    gtk_tree_view_set_search_column(GTK_TREE_VIEW(treeview), 0);
    gtk_tree_selection_set_mode(gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview)), GTK_SELECTION_BROWSE);
    g_signal_connect(G_OBJECT(gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview))),
                     "changed", G_CALLBACK(sl_row_activated), store);
    gtk_container_add(GTK_CONTAINER(swindow), treeview);

    renderer = gtk_cell_renderer_text_new();
    g_object_set(G_OBJECT(renderer), "xpad", 5, NULL);
    column = gtk_tree_view_column_new_with_attributes(_("Date"), renderer, "text", 0, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);

    gtk_tree_view_column_set_sort_column_id(column, 0);
    gtk_tree_view_column_set_sort_order(column, GTK_SORT_DESCENDING);
    gtk_tree_sortable_set_sort_func(GTK_TREE_SORTABLE(store), 0, game_compare_func, NULL, NULL);
    gtk_tree_view_column_clicked(column);

    renderer = gtk_cell_renderer_text_new();
    g_object_set(G_OBJECT(renderer), "xalign", 1.0, NULL);
    g_object_set(G_OBJECT(renderer), "xpad", 5, NULL);
    column = gtk_tree_view_column_new_with_attributes(_("Score"), renderer, "text", 2, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);

    if (iter.stamp == store->stamp) {
        if (is_save) {
            path = gtk_tree_model_get_path(GTK_TREE_MODEL(store), &iter);
        } else {
            path = gtk_tree_path_new_from_string("0");
        }
        if (path) {
            gtk_tree_selection_select_path(gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview)), path);
            gtk_tree_view_set_cursor(GTK_TREE_VIEW(treeview), path, NULL, FALSE);
            gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(treeview), path, NULL, TRUE, 0, 0);
            gtk_tree_path_free(path);
        }
    }
    g_object_unref(G_OBJECT(store));

    button_box = gtk_hbox_new(TRUE, 10);
    if (is_save) {
        ok_button = ut_button_new(_("Save game"), do_save_game, window, button_box);
        g_signal_connect(G_OBJECT(treeview), "row_activated", G_CALLBACK(save_row_activated_cb), window);
    } else {
        ok_button = ut_button_new(_("Load game"), do_load_game, window, button_box);
        g_signal_connect(G_OBJECT(treeview), "row_activated", G_CALLBACK(load_row_activated_cb), window);
    }
    delete_button = ut_button_new(_("Delete game"), do_delete_game, treeview, button_box);

    cancel_button = ut_button_new_stock_swap(GTK_STOCK_CANCEL, gtk_widget_destroy, window, button_box);
    gtk_box_pack_start(GTK_BOX(vbox), button_box, FALSE, FALSE, 4);

    gtk_widget_grab_default(ok_button);
    gtk_widget_grab_focus(ok_button);

    gtk_window_set_default_size(GTK_WINDOW(window), -1, 300);
    gtk_widget_show_all(window);
}

void save_game_cb(GtkWidget *widget, gpointer data) {
    save_load_game_dialog(TRUE);
}

void load_game_cb(GtkWidget *widget, gpointer data) {
    save_load_game_dialog(FALSE);
}

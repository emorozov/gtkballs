/* inputname.c - dialog for entering users name
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 */
#include <gtk/gtk.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#include "gtkballs.h"
#include "gtkutils.h"
#include "scoreboard.h"
#include "halloffame.h"
#include "game.h"

static GtkWidget *_dialog = NULL;

static gchar _last_player_name[15] = "";
static gint _saved_score = 0;

void read_entry(GtkWidget *widget, gpointer data) {
    struct score_board current_entry;
    time_t current_time;
    struct tm *timeptr;
    gint pos, fbn;
    gsize br, bw;
    gchar *tstr = NULL;
    struct score_board b[10];
    struct score_board_full *bf = NULL;

    tstr = (gchar *) gtk_entry_get_text(GTK_ENTRY(data));
    strncpy(current_entry.name, tstr && *tstr ? tstr : _("Anonymous"), sizeof(current_entry.name));
    strncpy(_last_player_name, current_entry.name, sizeof(_last_player_name));
    current_entry.score = _saved_score;
    current_time = time(NULL);
    timeptr = localtime(&current_time);
    if (!timeptr) {
        ut_simple_message_box(_("Unable to determine current date.\n"));
        strncpy(current_entry.date, _("Unknown"), sizeof(current_entry.date));
    } else {
        if (!strftime(current_entry.date, sizeof(current_entry.date), _("%a %b %d %H:%M %Y"), timeptr)) {
            ut_simple_message_box(_("Unable to determine current date.\n"));
            strncpy(current_entry.date, _("Unknown"), sizeof(current_entry.date));
        } else {
            tstr = g_locale_to_utf8(current_entry.date, -1, &br, &bw, NULL);
            if (!tstr) {
                strncpy(current_entry.date, _("Unknown"), sizeof(current_entry.date));
            } else {
                strncpy(current_entry.date, tstr, sizeof(current_entry.date));
                g_free(tstr);
            }
        }
    }

    if (!read_score(b, &bf, &fbn)) {
        ut_simple_message_box(_("Unable to read score.\n"));
    } else {
        pos = insert_entry_in_score_board(b, current_entry);
        if (!write_score(b, bf, fbn)) {
            ut_simple_message_box(_("Unable to save score.\n"));
        }
        /*FIXME: free bf here */
        free_score_board_full(bf, fbn);
        if (pos != -1) {
            pos++;
            show_hall_of_fame(NULL, (gpointer) pos, b);
        }
    }

    gtk_widget_destroy(_dialog);
    /* show scores to let user see if (s)he's on top ;) */
}

void input_name_dialog(void) {
    GtkWidget *prompt_label;
    GtkWidget *name;
    GtkWidget *button;
    gchar *s;

    /* we have to save score, because they will be set to 0 in new_game() */
    _saved_score = game_get_score();

    _dialog = gtk_dialog_new();
    gtk_window_set_wmclass(GTK_WINDOW(_dialog), "GtkBalls_Inputname", "GtkBalls");
    gtk_container_set_border_width(GTK_CONTAINER(GTK_DIALOG(_dialog)->vbox), 2);

    prompt_label = gtk_label_new(_("Enter your name"));
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(_dialog)->vbox), prompt_label, TRUE, TRUE, 0);

    name = gtk_entry_new();
    gtk_entry_set_max_length(GTK_ENTRY(name), 14);
    g_signal_connect(G_OBJECT(name), "activate", G_CALLBACK(read_entry), name);

    /* restore the last player's name */
    if (!(*_last_player_name)) {
        if ((s = getenv("USER"))) {
            strncpy(_last_player_name, s, sizeof(_last_player_name));
        }
    }
    gtk_entry_set_text(GTK_ENTRY(name), _last_player_name);
    gtk_editable_select_region(GTK_EDITABLE(name), 0, -1);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(_dialog)->vbox), name, TRUE, TRUE, 5);

    button = ut_button_new_stock(GTK_STOCK_OK, read_entry, name, GTK_DIALOG(_dialog)->action_area);

    gtk_widget_grab_focus(name);

    gtk_widget_grab_default(button);
    gtk_widget_show_all(_dialog);
}

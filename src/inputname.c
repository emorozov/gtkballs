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

static gchar _last_player_name[15] = "";
static gint _saved_score = 0;

static void read_entry (GtkEntry *entry, gpointer data)
{
   struct score_board current_entry;
   time_t current_time;
   struct tm *timeptr;
   gint pos, fbn;
   gsize br, bw;
   gchar *tstr = NULL;
   struct score_board b[10];
   struct score_board_full *bf = NULL;

   tstr = (gchar*)gtk_entry_get_text (entry);
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
      ut_simple_message_box (_("Unable to read score.\n"));
   } else {
      pos = insert_entry_in_score_board(b, current_entry);
      if (!write_score(b, bf, fbn)) {
         ut_simple_message_box(_("Unable to save score.\n"));
      }
      /*FIXME: free bf here */
      free_score_board_full(bf, fbn);
      if (pos != -1) {
         pos++;
         show_hall_of_fame(NULL, (gpointer)pos, b);
      }
   }

   gtk_widget_destroy (GTK_WIDGET (data));
   /* show scores to let user see if (s)he's on top ;) */
}

static void input_name_response (GtkDialog * dlg, int response, gpointer user_data)
{
   if (response == GTK_RESPONSE_OK) {
      read_entry (GTK_ENTRY (user_data), dlg);
   }
}

static void input_name_esc (GtkDialog * dlg, gpointer data)
{
   g_signal_stop_emission_by_name (dlg, "close"); // ignore Esc
}

void input_name_dialog(void)
{
   GtkWidget * dialog, * prompt_label, * vbox;
   GtkWidget * name;
   GtkWidget * button;
   gchar * s;

   /* we have to save score, because they will be set to 0 in new_game() */
   _saved_score = game_get_score();

   dialog = gtkutil_dialog_new (NULL, main_window, TRUE, &vbox);

   prompt_label = gtk_label_new(_("Enter your name"));
   gtk_box_pack_start (GTK_BOX(vbox), prompt_label, TRUE, TRUE, 0);

   name = gtk_entry_new();
   gtk_entry_set_max_length(GTK_ENTRY(name), 14);
   g_signal_connect(G_OBJECT(name), "activate", G_CALLBACK(read_entry), dialog);

   /* restore the last player's name */
   if (!(*_last_player_name)) {
      if ((s = getenv("USER"))) {
         strncpy(_last_player_name, s, sizeof(_last_player_name));
      }
   }
   gtk_entry_set_text(GTK_ENTRY(name), _last_player_name);
   gtk_editable_select_region(GTK_EDITABLE(name), 0, -1);
   gtk_box_pack_start (GTK_BOX(vbox), name, TRUE, TRUE, 5);

   gtk_dialog_add_button (GTK_DIALOG (dialog), "gtk-ok", GTK_RESPONSE_OK);
   g_signal_connect (dialog, "response", G_CALLBACK (input_name_response), name);
   g_signal_connect (dialog, "close", G_CALLBACK (input_name_esc), name);

   gtk_widget_show_all (dialog);
}

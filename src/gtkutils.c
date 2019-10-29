/* gtkutils.c some wrappers/utilities for gtk+ toolkit
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 */
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

gboolean ut_key_pressed_cb(GtkWidget *widget, GdkEventKey *event) {
	if(widget && event && event->keyval == GDK_Escape) {
                gtk_widget_destroy(widget);
                return TRUE;
        }
        return FALSE;
}

GtkWidget *ut_window_new(gchar *title, gchar *wmname, gchar *wmclass,
			 gboolean escaable, gboolean modal, gboolean resizable,
			 gint border) {
        GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        gtk_window_set_modal(GTK_WINDOW(window), modal);
        gtk_window_set_resizable(GTK_WINDOW(window), resizable);
  	gtk_window_set_title(GTK_WINDOW(window), title);
        gtk_window_set_wmclass(GTK_WINDOW(window), wmname, wmclass);
        if(escaable) {
		g_signal_connect(G_OBJECT(window), "key_press_event", G_CALLBACK(ut_key_pressed_cb), NULL);
        }
        gtk_container_set_border_width(GTK_CONTAINER(window), border);
        return window;
}

GtkWidget *ut_check_button_new(gchar *label, gboolean active, GtkWidget *parent) {
  	GtkWidget *button;

	button = gtk_check_button_new_with_label(label);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), active);
  	gtk_box_pack_start(GTK_BOX(parent), button, FALSE, FALSE, 0);

        return button;
}

GtkWidget *ut_button_new(gchar *label, gpointer func, gpointer func_data, GtkWidget *parent) {
  	GtkWidget *button;

        button = gtk_button_new_with_label(label);
	g_signal_connect(G_OBJECT(button), "clicked", func, func_data);
  	gtk_box_pack_start(GTK_BOX(parent), button, TRUE, TRUE, 0);
        GTK_WIDGET_SET_FLAGS(button, GTK_CAN_DEFAULT);

        return button;
}

GtkWidget *ut_button_new_stock(const gchar *stock_id, gpointer func, gpointer func_data, GtkWidget *parent) {
  	GtkWidget *button;

  	button = gtk_button_new_from_stock(stock_id);
  	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(func), func_data);
  	gtk_box_pack_start(GTK_BOX(parent), button, TRUE, TRUE, 0);
        GTK_WIDGET_SET_FLAGS(button, GTK_CAN_DEFAULT);

        return button;
}

GtkWidget *ut_button_new_stock_swap(const gchar *stock_id, gpointer func, gpointer func_data, GtkWidget *parent) {
  	GtkWidget *button;

  	button = gtk_button_new_from_stock(stock_id);
  	g_signal_connect_swapped(G_OBJECT(button), "clicked", G_CALLBACK(func), func_data);
  	gtk_box_pack_start(GTK_BOX(parent), button, TRUE, TRUE, 0);
        GTK_WIDGET_SET_FLAGS(button, GTK_CAN_DEFAULT);

        return button;
}

GtkWidget *ut_spin_button_new(gchar *label, gint min, gint max, gint val, GtkWidget *parent) {
        GtkObject *adj;
	GtkWidget *button, *hbox, *labelw;

  	hbox = gtk_hbox_new(FALSE, 0);
  	gtk_box_pack_start(GTK_BOX(parent), hbox, TRUE, TRUE, 2);

  	labelw = gtk_label_new(label);
  	gtk_box_pack_start(GTK_BOX(hbox), labelw, FALSE, FALSE, 5);

        adj = gtk_adjustment_new(val, min, max, 1, 10, 10);
        button = gtk_spin_button_new(GTK_ADJUSTMENT(adj), 1, 0);
	gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(button), TRUE);
  	gtk_box_pack_end(GTK_BOX(hbox), button, FALSE, FALSE, 5);

        return button;
}

GtkWidget *ut_spin_button_start_new(gchar *label, gint min, gint max, gint val, GtkWidget *parent) {
        GtkObject *adj;
	GtkWidget *button, *hbox, *labelw;

  	hbox = gtk_hbox_new(FALSE, 0);
  	gtk_box_pack_start(GTK_BOX(parent), hbox, TRUE, TRUE, 2);

  	labelw = gtk_label_new(label);
  	gtk_box_pack_start(GTK_BOX(hbox), labelw, FALSE, FALSE, 5);

        adj = gtk_adjustment_new(val, min, max, 1, 10, 10);
        button = gtk_spin_button_new(GTK_ADJUSTMENT(adj), 1, 0);
	gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(button), TRUE);
  	gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, FALSE, 5);

        return button;
}

/* shows simple message box */
void ut_simple_message_box(gchar *message) {
  	GtkWidget *dialog = gtk_message_dialog_new(NULL, 0, GTK_MESSAGE_INFO, GTK_BUTTONS_CLOSE, message);
 	gtk_dialog_run(GTK_DIALOG(dialog));
 	gtk_widget_destroy(dialog);
}

/* shows simple message box */
void ut_simple_message_box_with_title(gchar *message, gchar *title) {
  	GtkWidget *dialog = gtk_message_dialog_new(NULL, 0, GTK_MESSAGE_INFO, GTK_BUTTONS_CLOSE, message);
  	gtk_window_set_title(GTK_WINDOW(dialog), title);
 	gtk_dialog_run(GTK_DIALOG(dialog));
 	gtk_widget_destroy(dialog);
}


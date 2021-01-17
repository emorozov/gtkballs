/* gtkutils.c some wrappers/utilities for gtk+ toolkit
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 */
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

gboolean ut_key_pressed_cb(GtkWidget *widget, GdkEventKey *event)
{
   if(widget && event && event->keyval == GDK_Escape) {
      gtk_widget_destroy(widget);
      return TRUE;
   }
   return FALSE;
}


GtkWidget *ut_window_new(gchar *title, gchar *wmname, gchar *wmclass,
                         gboolean escaable, gboolean modal, gboolean resizable,
                         gint border)
{
   GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
   gtk_window_set_modal(GTK_WINDOW(window), modal);
   gtk_window_set_resizable(GTK_WINDOW(window), resizable);
   gtk_window_set_title(GTK_WINDOW(window), title);
   gtk_window_set_role(GTK_WINDOW(window), wmname);
   if(escaable) {
      g_signal_connect(G_OBJECT(window), "key_press_event", G_CALLBACK(ut_key_pressed_cb), NULL);
   }
   gtk_container_set_border_width(GTK_CONTAINER(window), border);
   return window;
}


GtkWidget * gtkutil_dialog_new (char * title,
                                GtkWindow * parent,
                                gboolean resizable,
                                GtkWidget ** main_vbox) /* out */
{
   GtkWidget * dialog;
   dialog = gtk_dialog_new ();
   gtk_window_set_title (GTK_WINDOW (dialog), title);
   gtk_container_set_border_width (GTK_CONTAINER (dialog), 4); /* padding */
   if (parent) {
      gtk_window_set_transient_for (GTK_WINDOW (dialog), GTK_WINDOW (parent));
      gtk_window_set_modal (GTK_WINDOW (dialog), TRUE);
      gtk_window_set_destroy_with_parent (GTK_WINDOW (dialog), TRUE);
      gtk_window_set_position (GTK_WINDOW (dialog), GTK_WIN_POS_CENTER_ON_PARENT);
      gtk_window_set_skip_pager_hint (GTK_WINDOW (dialog), TRUE);
      gtk_window_set_skip_taskbar_hint (GTK_WINDOW (dialog), TRUE);
   }
   if (!resizable) {
      // no need to call this if TRUE, unexpected behavior in GTK3 IIRC
      gtk_window_set_resizable (GTK_WINDOW (dialog), FALSE);
   }

   if (main_vbox) {
      *main_vbox = gtk_dialog_get_content_area (GTK_DIALOG (dialog));
      /* padding */
      gtk_container_set_border_width (GTK_CONTAINER (*main_vbox), 4);
   }

   return dialog;
}


GtkWidget * gtkutil_frame_vbox (char * label, GtkWidget * parent_box)
{
   GtkWidget * frame;
   frame = gtk_frame_new (label);
   gtk_box_pack_start (GTK_BOX (parent_box), frame, FALSE, FALSE, 0);

   GtkWidget * frame_vbox = gtk_vbox_new (FALSE, 5);
   gtk_container_add (GTK_CONTAINER (frame), frame_vbox);
   /* padding */
   gtk_container_set_border_width (GTK_CONTAINER (frame_vbox), 5);

   return frame_vbox;
}


GtkWidget *ut_check_button_new(gchar *label, gboolean active, GtkWidget *parent)
{
   GtkWidget *button;

   button = gtk_check_button_new_with_label(label);
   gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), active);
   gtk_box_pack_start(GTK_BOX(parent), button, FALSE, FALSE, 0);

   return button;
}


GtkWidget *ut_button_new(gchar *label, gpointer func, gpointer func_data, GtkWidget *parent)
{
   GtkWidget *button;

   button = gtk_button_new_with_label(label);
   g_signal_connect(G_OBJECT(button), "clicked", func, func_data);
   gtk_box_pack_start(GTK_BOX(parent), button, TRUE, TRUE, 0);
   gtk_widget_set_can_default (button, TRUE);

   return button;
}


GtkWidget *ut_button_new_stock(const gchar *stock_id, gpointer func, gpointer func_data, GtkWidget *parent)
{
   GtkWidget *button;

   button = gtk_button_new_from_stock(stock_id);
   g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(func), func_data);
   gtk_box_pack_start(GTK_BOX(parent), button, TRUE, TRUE, 0);
   gtk_widget_set_can_default (button, TRUE);

   return button;
}


GtkWidget *ut_button_new_stock_swap(const gchar *stock_id, gpointer func, gpointer func_data, GtkWidget *parent)
{
   GtkWidget *button;

   button = gtk_button_new_from_stock(stock_id);
   g_signal_connect_swapped(G_OBJECT(button), "clicked", G_CALLBACK(func), func_data);
   gtk_box_pack_start(GTK_BOX(parent), button, TRUE, TRUE, 0);
   gtk_widget_set_can_default (button, TRUE);

   return button;
}


GtkWidget *ut_spin_button_new(gchar *label, gint min, gint max, gint val, GtkWidget *parent)
{
   GtkAdjustment *adj;
   GtkWidget *button, *hbox, *labelw;

   hbox = gtk_hbox_new(FALSE, 0);
   gtk_box_pack_start(GTK_BOX(parent), hbox, TRUE, TRUE, 0);

   labelw = gtk_label_new(label);
   gtk_box_pack_start(GTK_BOX(hbox), labelw, FALSE, FALSE, 5);

   adj = GTK_ADJUSTMENT (gtk_adjustment_new (val, min, max, 1, 10, 0));
   button = gtk_spin_button_new(adj, 1, 0);
   gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(button), TRUE);
   gtk_box_pack_end(GTK_BOX(hbox), button, FALSE, FALSE, 5);

   return button;
}


/* shows simple message box */
void ut_simple_message_box(gchar *message)
{
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


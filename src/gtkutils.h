#ifndef __GTKUTILS_H
#define __GTKUTILS_H
gboolean  ut_key_pressed_cb	(GtkWidget *widget, GdkEventKey *event);

GtkWidget *ut_window_new (gchar *title,
                          gchar *wmname,
                          gchar *wmclass,
                          gboolean escaable,
                          gboolean modal,
                          gboolean resizable,
                          gint border);

GtkWidget * gtkutil_dialog_new (char * title,
                                GtkWindow * parent,
                                gboolean resizable,
                                GtkWidget ** main_vbox); /* out */

GtkWidget * gtkutil_frame_vbox (char * label, GtkWidget * parent_box);

GtkWidget *ut_check_button_new (gchar *label,
                                gboolean active,
                                GtkWidget *parent);

GtkWidget *ut_button_new (gchar *label,
                          gpointer func,
                          gpointer func_data,
                          GtkWidget *parent);

GtkWidget *ut_spin_button_new (gchar *label,
                               gint min,
                               gint max,
                               gint val,
                               GtkWidget *parent);

void ut_simple_message_box (gchar *message);

void ut_simple_message_box_with_title (gchar *message, gchar *title);

#endif

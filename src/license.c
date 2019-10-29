/* license.c - display dialog box with GNU GPL.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 */
#include <gtk/gtk.h>
#include <sys/types.h>	/* stat() */
#include <sys/stat.h>	/* stat() */
#include <unistd.h>	/* stat() */
#include <fcntl.h>	/* open() */

#include "gtkballs.h"   /* _() */
#include "gtkutils.h"

void gnu_license_dialog(gpointer data) {
  	GtkWidget *window;
  	GtkWidget *swindow;
  	GtkWidget *vbox,*hbox,*button_box;
  	GtkWidget *license;
        GtkTextBuffer *buffer;
        GtkTextIter iter;
  	GtkWidget *separator;
  	GtkWidget *ok_button;
  	struct stat buf;
  	int fd;
  	gchar *license_buf;
        gchar *license_file=DATADIR "/gtkballs/COPYING";

  	if(stat(license_file, &buf)<0) {
      		ut_simple_message_box(_("Can't stat license file"));
      		return;
  	}
  
  	license_buf=(gchar *)g_malloc(buf.st_size+1);
  	if((fd=open(license_file, O_RDONLY))<0) {
      		ut_simple_message_box(_("Can't open license file"));
      		g_free(license_buf);
      		return;
    	}
  	read(fd, license_buf, buf.st_size);
        license_buf[buf.st_size]='\0';
  	close(fd);

	window=ut_window_new(_("GNU Public License"), "GtkBalls_License", "GtkBalls", TRUE, TRUE, TRUE, 5);

  	vbox=gtk_vbox_new(FALSE, 0);
  	gtk_container_set_border_width(GTK_CONTAINER(vbox), 5);
  	gtk_container_add(GTK_CONTAINER(window), vbox);

  	hbox=gtk_hbox_new(FALSE, 0);
  	gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, TRUE, 0);

        swindow=gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_set_size_request(swindow, -1, 250);
        gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(swindow), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
        gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(swindow), GTK_SHADOW_ETCHED_IN);
  	gtk_box_pack_start(GTK_BOX(hbox), swindow, TRUE, TRUE, 0);

  	license=gtk_text_view_new();
        gtk_text_view_set_editable(GTK_TEXT_VIEW(license), FALSE);
        gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(license), FALSE);
  	gtk_container_add(GTK_CONTAINER(swindow), license);

        buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(license));
        gtk_text_buffer_insert_at_cursor(buffer, license_buf, -1);
  	g_free(license_buf);

        gtk_text_buffer_get_iter_at_offset(buffer, &iter, 0);
        gtk_text_buffer_place_cursor(buffer, &iter);

  	separator=gtk_hseparator_new();
  	gtk_box_pack_start(GTK_BOX(vbox), separator, FALSE, FALSE, 5);

	button_box=gtk_hbox_new(TRUE, 0);
  	gtk_box_pack_start(GTK_BOX(vbox), button_box, FALSE, TRUE, 0);

        ok_button=ut_button_new_stock_swap(GTK_STOCK_CLOSE, gtk_widget_destroy, window, button_box);

        gtk_widget_grab_default(ok_button);
        gtk_widget_grab_focus(ok_button);
	gtk_widget_show_all(window);
}

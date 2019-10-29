/* about.c - display dialog box with information about program
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 */
#include <gtk/gtk.h>

#include "gtkballs.h" /* _(), VERSION from config.h */
#include "gtkutils.h"
#include "license.h" /* gnu_license_dialog() */

/* Show about dialog box */
void about(GtkWidget *widget, gpointer data) {
  	GtkWidget *window;
  	GtkWidget *vbox, *hbox, *buttons_box, *ok_button;
  	GtkWidget *pixmap;
  	gchar     strbuf[1024];

	window = ut_window_new(_("About"), "GtkBalls_About", "GtkBalls", TRUE, TRUE, FALSE, 5);

  	vbox = gtk_vbox_new(FALSE, 0);
  	gtk_container_add(GTK_CONTAINER(window), vbox);

  	hbox = gtk_hbox_new(FALSE, 0);
  	gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, TRUE, 0);
  
        pixmap = gtk_image_new_from_file(DATADIR "/gtkballs/gtkballs-logo.png");
  	gtk_box_pack_start(GTK_BOX(hbox), pixmap, TRUE, TRUE, 4);

  	g_snprintf(strbuf, sizeof(strbuf), _("GtkBalls %s\n" \
					     "Copyright (C) 2001-2002 drF_ckoff <dfo@antex.ru>\n" \
					     "Copyright (C) 1998-1999 Eugene Morozov\n" \
					     "<jmv@online.ru>, <roshiajin@yahoo.com>\n\n" \
		       		       	     "GtkBalls comes with ABSOLUTELY NO WARRANTY;\n" \
		       		       	     "for details press \"Show license\" button.\n" \
      		       		       	     "This is free software and you are welcome to\n" \
				       	     "redistribute it under certain conditions;\n" \
				       	     "press \"Show license\" button for details."), VERSION);
  	gtk_box_pack_start(GTK_BOX(hbox), gtk_label_new(strbuf), TRUE, TRUE, 2);
  	gtk_box_pack_start(GTK_BOX(vbox), gtk_hseparator_new(), FALSE, FALSE, 5);

  	buttons_box = gtk_hbutton_box_new();
  	gtk_box_pack_start(GTK_BOX(vbox), buttons_box, TRUE, TRUE, 2);
        gtk_button_box_set_layout(GTK_BUTTON_BOX(buttons_box), GTK_BUTTONBOX_SPREAD);

        ok_button = ut_button_new_stock_swap(GTK_STOCK_CLOSE, gtk_widget_destroy, window, buttons_box);
        ut_button_new(_("Show license"), gnu_license_dialog, window, buttons_box);
        gtk_widget_grab_default(ok_button);
  	gtk_widget_show_all(window);
}

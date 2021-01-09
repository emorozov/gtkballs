/* about.c - display dialog box with information about program
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 */
#include <gtk/gtk.h>
#include "gtkballs.h" /* _(), VERSION from config.h */

/* Show about dialog box */
void about (GtkWidget *widget, gpointer data)
{
   GtkWidget * w;
   GdkPixbuf * logo;
   const gchar * authors[] =
   {
      "Eugene Morozov <jmv@online.ru>",
      "drF_ckoff <dfo@antex.ru>",
      NULL
   };
    /* TRANSLATORS: Replace this string with your names, one name per line. */
    gchar * translators = _("Translated by");

   logo = gdk_pixbuf_new_from_file (DATADIR "/gtkballs/gtkballs-logo.png", NULL);

   w = g_object_new (GTK_TYPE_ABOUT_DIALOG,
                     "version",      VERSION,
                     "program-name", "GtkBalls",
                     "copyright",    "Copyright (C) 1998-2021",
                     "comments",     "Clone of Lines - logic game about balls",
                     "license",      "This program is free software; you can redistribute it and/or\nmodify it under the terms of the GNU General Public License\nas published by the Free Software Foundation; either version 2\nof the License, or (at your option) any later version.\n\nThis program is distributed in the hope that it will be useful,\nbut WITHOUT ANY WARRANTY; without even the implied warranty of\nMERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\nGNU General Public License for more details.\n\nYou should have received a copy of the GNU General Public License\nalong with this program; if not, write to the Free Software\nFoundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.",
                     "website",      "https://github.com/emorozov/gtkballs",
                     "authors",      authors,
                     "logo",         logo,
                     "translator-credits", translators,
                     NULL);
   if (logo) {
      g_object_unref (logo);
   }
   gtk_container_set_border_width (GTK_CONTAINER (w), 2);
   gtk_window_set_transient_for (GTK_WINDOW (w), GTK_WINDOW (main_window));
   gtk_window_set_modal (GTK_WINDOW (w), TRUE);
   gtk_window_set_position (GTK_WINDOW (w), GTK_WIN_POS_CENTER_ON_PARENT);

   g_signal_connect_swapped (w, "response",
                             G_CALLBACK (gtk_widget_destroy), w);
   gtk_widget_show_all (GTK_WIDGET (w));
}

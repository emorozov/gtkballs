/* theme.c - functions related to theme handling
 *
 * This program is free software; you can redistribute it and/or
 * modif (y it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 */
#include <gtk/gtk.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>

#include "theme.h"

#include "themerc.h"
#include "game.h"
#include "gtkutils.h" /* ut_simple_message... */
#include "prefs.h"

GtkbTheme *gtkbTheme=NULL;

/* some file/dir names */
gchar *INSTALLPATH=DATADIR "/gtkballs/themes/";
gchar *THEMEPREFIX="/.gtkballs/themes/";

#define DELTA(d, h) \
        d = h < 0 ? (d > -h ? d + h : 0) : (d + h < 255 ? d + h : 255)


void hilight_pixbuff8(GdkPixbuf *pb, gint dr, gint dg, gint db)
{
   /* pb created with 8b/rgb without alpha */
   gint i;
   gint nc = gdk_pixbuf_get_n_channels(pb);
   gint delta = nc - 3;
   gint w = gdk_pixbuf_get_width(pb);
   gint l = w * gdk_pixbuf_get_height(pb);
   gint d = gdk_pixbuf_get_rowstride(pb) - w * nc;
   guchar *data = gdk_pixbuf_get_pixels(pb);

   for (i = 0; i < l; i++) {
      if (i && (i%w == 0)) {
         data += d;
      }
      DELTA(*data, dr);
      data++;
      DELTA(*data, dg);
      data++;
      DELTA(*data, db);
      data += delta + 1;
   }
}


gchar *find_theme_path(gchar *themename)
{
   gchar *homedir;
   gchar *themepath;
   struct stat buf;

   if ((homedir = getenv("HOME")) &&
      (themepath = g_strconcat(homedir, THEMEPREFIX, themename, G_DIR_SEPARATOR_S, NULL))) {
      if (!stat(themepath, &buf) && S_ISDIR(buf.st_mode)) {
         return themepath;
      }
      g_free (themepath);
   }
   if ((themepath = g_strconcat(INSTALLPATH, themename, G_DIR_SEPARATOR_S, NULL))) {
      if (!stat(themepath, &buf) && S_ISDIR(buf.st_mode)) {
         return themepath;
      }
      g_free (themepath);
   }
   return NULL;
}


gint gtkb_load_pixmap(GtkbPixmap *pixmap, gchar *path, gchar *pixmapname)
{
   gchar *fname;
   GError *error = NULL;
   gint rv = 1;

   if (!(fname = g_strconcat(path, pixmapname, NULL))) {
      return 0;
   }
   if (pixmap->pixbuf) {
      g_object_unref(pixmap->pixbuf);
   }
   pixmap->pixbuf = gdk_pixbuf_new_from_file(fname, &error);
   if (!pixmap->pixbuf) {
      ut_simple_message_box(error->message);
      g_error_free (error);
      rv = 0;
   } else {
      pixmap->xsize = gdk_pixbuf_get_width (pixmap->pixbuf);
      pixmap->ysize = gdk_pixbuf_get_height (pixmap->pixbuf);
   }
   g_free (fname);
   return rv;
}

void gtkb_pixmap_free(GtkbPixmap pixmap) {
   if (pixmap.pixbuf) {
      g_object_unref(pixmap.pixbuf);
   }
}


void gtkb_theme_free(GtkbTheme *theme)
{
   gint i, j;

   if (!theme) {
      return;
   }
   gtkb_pixmap_free(theme->emptycell);
   gtkb_pixmap_free(theme->hemptycell);
   for (i = 0; i < 8; i++) {
      gtkb_pixmap_free(theme->paws[i]);
   }
   if (theme->balls) {
      for (i = 0; i < theme->numballs; i++) {
         gtkb_pixmap_free(theme->balls[i].ball);
         gtkb_pixmap_free(theme->balls[i].small);
         if (theme->balls[i].jump) {
            for (j = 0; j < theme->balls[i].jumpphases; j++) {
               gtkb_pixmap_free(theme->balls[i].jump[j]);
            }
            g_free (theme->balls[i].jump);
            if (theme->balls[i].jumpdelays) {
               g_free (theme->balls[i].jumpdelays);
            }
         }
         if (theme->balls[i].destroy) {
            for (j = 0; j < theme->balls[i].destroyphases; j++) {
               gtkb_pixmap_free(theme->balls[i].destroy[j]);
            }
            g_free (theme->balls[i].destroy);
            if (theme->balls[i].destroydelays) {
               g_free (theme->balls[i].destroydelays);
            }
         }
      }
   }
   g_free (theme->balls);
   g_free (theme);
}


/* warning! tmpname will be free()'d! */
gint gtkb_load_pixmap_from(gchar **trc, gchar *themepath, gchar *tmpname, GtkbPixmap *pixmap)
{
   gchar *val;
   gint ret;

   val = trc_get_str(trc, tmpname);
   g_free (tmpname);
   if (val == NULL) {
      return 0;
   }
   ret = gtkb_load_pixmap(pixmap, themepath, val);
   g_free (val);

   return ret;
}

#define CHECKRET(ret, cond) \
   if (ret == cond) { \
      gtkb_theme_free(theme); \
      trc_close(trc); \
      return NULL; \
   }

void gtkb_make_hl_pixmap(GtkbTheme *theme) {
   if (theme->hemptycell.pixbuf) {
      gtkb_pixmap_free(theme->hemptycell);
   }
   theme->hemptycell.pixbuf = gdk_pixbuf_copy(theme->emptycell.pixbuf);
   theme->hemptycell.xsize = theme->emptycell.xsize;
   theme->hemptycell.ysize = theme->emptycell.ysize;
   hilight_pixbuff8(theme->hemptycell.pixbuf, prefs_get_hl_dr(), prefs_get_hl_dg(), prefs_get_hl_db());
}


GtkbTheme *gtkb_load_theme(gchar *themepath)
{
   gchar **trc, *opt;
   gchar *paws[] = {"down_up", "left_right", "up_down", "right_left",
                    "down_right", "down_left", "up_right", "up_left", NULL};
   gint  i, j, ret;
   GtkbTheme *theme;

   opt = g_strconcat(themepath, "themerc", NULL);
   trc = trc_open(opt); /* open theme description file */
   g_free (opt);
   if (!trc) {
      return NULL;
   }
   theme = g_new0(GtkbTheme, 1);

   /* find and load "empty cell" pixmap. */
   opt = trc_get_str(trc, "cell");
   CHECKRET(opt, NULL);
   ret = gtkb_load_pixmap(&theme->emptycell, themepath, opt);
   g_free (opt);
   CHECKRET(ret, 0);
/*
   theme->hemptycell.pixbuf = gdk_pixbuf_copy(theme->emptycell.pixbuf);
   theme->hemptycell.xsize = theme->emptycell.xsize;
   theme->hemptycell.ysize = theme->emptycell.ysize;
   CHECKRET(theme->hemptycell.pixbuf, NULL);
   hilight_pixbuff8(theme->hemptycell.pixbuf, prefs_get_hl_dr(), prefs_get_hl_dg(), prefs_get_hl_db());
*/
   gtkb_make_hl_pixmap(theme);

   /* find and load "footprints" pixmaps. */
   for (i = 0; paws[i]; i++) {
      CHECKRET(gtkb_load_pixmap_from(trc, themepath, g_strconcat("paw.", paws[i], NULL),
               &theme->paws[i]), 0);
   }

   /* query number of available balls in theme */
   theme->numballs = trc_get_uint(trc, "ball.numbers");
   CHECKRET(theme->numballs, -1);
   if (theme->numballs < rules_get_colors()) {
      CHECKRET(0, 0); /* yes, i know. its ugly =) */
   }
   theme->balls = g_new0(GtkbBall, theme->numballs);

   /* find and load all balls data. */
   for (i = 0; i < theme->numballs; i++) {
      CHECKRET(gtkb_load_pixmap_from(trc, themepath, g_strdup_printf("ball.%d.still", i + 1),
               &theme->balls[i].ball), 0);
      CHECKRET(gtkb_load_pixmap_from(trc, themepath, g_strdup_printf("ball.%d.small", i + 1),
               &theme->balls[i].small), 0);

      opt = g_strdup_printf("ball.%d.jump.numbers", i + 1);
      theme->balls[i].jumpphases = trc_get_uint(trc, opt);
      g_free (opt);
      CHECKRET(theme->balls[i].jumpphases, -1);
      if (theme->balls[i].jumpphases < 2) {
         CHECKRET(0, 0); /* yes, i know. its ugly =) */
      }
      theme->balls[i].jump = g_new0(GtkbPixmap, theme->balls[i].jumpphases);
      theme->balls[i].jumpdelays = g_new0(gint, theme->balls[i].jumpphases);

      for (j = 0; j < theme->balls[i].jumpphases; j++) {
         CHECKRET(gtkb_load_pixmap_from(trc, themepath,
                  g_strdup_printf("ball.%d.jump.%d", i + 1, j + 1),
                  &theme->balls[i].jump[j]), 0);
         opt = g_strdup_printf("ball.%d.jump.%d.usec", i + 1, j + 1);
         theme->balls[i].jumpdelays[j] = trc_get_uint(trc, opt);
         g_free (opt);
         CHECKRET(theme->balls[i].jumpdelays[j], -1);
      }

      opt = g_strdup_printf("ball.%d.destroy.numbers", i + 1);
      theme->balls[i].destroyphases = trc_get_uint(trc, opt);
      g_free (opt);
      CHECKRET(theme->balls[i].destroyphases, -1);
      if (theme->balls[i].destroyphases < 2) {
         CHECKRET(0, 0); /* yes, i know. its ugly =) */
      }
      if (theme->balls[i].destroyphases > theme->maxdestphases) {
         theme->maxdestphases = theme->balls[i].destroyphases;
      }
      theme->balls[i].destroy = g_new0(GtkbPixmap, theme->balls[i].destroyphases);
      theme->balls[i].destroydelays = g_new0(gint, theme->balls[i].destroyphases);
      for (j = 0; j < theme->balls[i].destroyphases; j++) {
         CHECKRET(gtkb_load_pixmap_from(trc, themepath,
                  g_strdup_printf("ball.%d.destroy.%d", i + 1, j + 1),
                  &theme->balls[i].destroy[j]), 0);
         opt = g_strdup_printf("ball.%d.destroy.%d.usec", i + 1, j + 1);
         theme->balls[i].destroydelays[j] = trc_get_uint(trc, opt);
         g_free (opt);
         CHECKRET(theme->balls[i].destroydelays[j], -1);
      }
   }
   trc_close(trc);

   return theme;
}


gint load_theme(gchar *themename)
{
   gchar *themepath;
   GtkbTheme *theme;

   if (!(themepath = find_theme_path(themename))) {
      return 0;
   }

   theme = gtkb_load_theme(themepath);
   g_free (themepath);

   if (!theme) {
      return 0;
   }

   gtkb_theme_free(gtkbTheme);
   gtkbTheme = theme;

   return 1;
}


gint gtkb_theme_free_handler(GtkWidget *widget, gpointer data)
{
   gtkb_theme_free(gtkbTheme);
   gtkbTheme = NULL;
   return 0;
}


gint gtkb_theme_get_balls_num(void)
{
   return gtkbTheme ? gtkbTheme->numballs : 0;
}


/* returns board coordinate of the pointer */
gint gtkb_theme_get_coord_at_x(gint x)
{
   if (gtkbTheme) {
      return (x - 1) / gtkbTheme->emptycell.xsize;
   }
   return -1;
}


gint gtkb_theme_get_coord_at_y(gint y)
{
   if (gtkbTheme) {
      return (y - 1) / gtkbTheme->emptycell.ysize;
   }
   return -1;
}

gint theme_get_width(void) {
   return gtkbTheme->emptycell.xsize;
}

gint theme_get_height(void) {
   return gtkbTheme->emptycell.xsize;
}


/* find all available themes. */
gchar **get_available_themes(void)
{
   DIR *directory;
   struct dirent *dir_entry;
   struct stat entry_stat;
   gchar *entry, *currdir, *hdir, *rcentry;
   gint i, j, num, flag;
   gchar  **tlist = NULL;

   if (getenv("HOME")) {
      hdir = g_strconcat(getenv("HOME"), THEMEPREFIX, NULL);
   } else {
      hdir = g_strdup("./"); /* FIXME: does it work on non unix os? */
   }

   for (j = 0, currdir = INSTALLPATH, num = 0; j < 2; j++, currdir = hdir)
   {
      if (!(directory = opendir(currdir))) {
         continue;
      }
      while((dir_entry = readdir(directory)))
      {
         if (!strncmp(dir_entry->d_name, ".", 2) ||
            !strncmp(dir_entry->d_name, "..", 3)) {
            continue;
         }
         entry = g_strconcat(currdir, dir_entry->d_name, NULL);
         if (!stat(entry, &entry_stat) && S_ISDIR(entry_stat.st_mode)) {
            rcentry = g_strconcat(entry, G_DIR_SEPARATOR_S, "themerc", NULL);
            if (!stat(rcentry, &entry_stat)) {
               flag = 0;
               for (i=0; i < num && !flag; i++) {
                  if (!strcmp(tlist[i], dir_entry->d_name)) {
                     flag++;
                  }
               }
               if (!flag) {
                  num++;
                  tlist = g_realloc(tlist, num * sizeof(gchar *));
                  tlist[num-1] = g_strdup(dir_entry->d_name);
               }
            }
            g_free (rcentry);
         }
         g_free (entry);
      }
      closedir(directory);
   }

   g_free (hdir);
   tlist = g_realloc(tlist, (num + 1) * sizeof(gchar *));
   tlist[num] = NULL;
   return tlist;
}

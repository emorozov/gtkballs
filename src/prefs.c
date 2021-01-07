/* prefs.c - preferences handling functions
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 */
#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "game.h"
#include "gtkballs.h" /* for _() */

#define BUFFER_SIZE 1024
#define CONFIG_FILE_NAME ".gtkballsrc"

gint _show_next_colors = TRUE;
gint _show_path = TRUE;
gint _show_footprints = TRUE;
gint _show_destroy = TRUE;
gint _show_highlight = TRUE;
gint _hl_dr = 64;
gint _hl_dg = 64;
gint _hl_db = 64;

/* name of default theme. TODO: make "internal" theme, so, when
 * theme name is NULL we can use it... */
gchar _default_theme_name[] = "Default";

/* This is the name of theme to use */
gchar *_theme_name = NULL;

gint pref_get_show_next_colors(void) {
    return _show_next_colors;
}

void pref_set_show_next_colors(gint val) {
    _show_next_colors = val;
}

gint pref_get_show_path(void) {
    return _show_path;
}

void pref_set_show_path(gint val) {
    _show_path = val;
}

gint pref_get_show_footprints(void) {
    return _show_footprints;
}

void pref_set_show_footprints(gint val) {
    _show_footprints = val;
}

gint pref_get_show_destroy(void) {
    return _show_destroy;
}

void pref_set_show_destroy(gint val) {
    _show_destroy = val;
}

gint pref_get_show_highlight(void) {
    return _show_highlight;
}

void pref_set_show_highlight(gint val) {
    _show_highlight = val;
}

gint prefs_get_hl_dr(void) {
    return _hl_dr;
}

void prefs_set_hl_dr(gint dr) {
    _hl_dr = dr;
}

gint prefs_get_hl_dg(void) {
    return _hl_dg;
}

void prefs_set_hl_dg(gint dg) {
    _hl_dg = dg;
}

gint prefs_get_hl_db(void) {
    return _hl_db;
}

void prefs_set_hl_db(gint db) {
    _hl_db = db;
}

gchar *pref_get_theme_name(void) {
    return _theme_name;
}

gchar *pref_get_default_theme_name(void) {
    return _default_theme_name;
}

void pref_set_theme_name(gchar *name) {
    if (_theme_name) {
        g_free(_theme_name);
    }
    _theme_name = g_strdup(name);
}

gchar *find_rc_file(void) {
    gchar *rc_file = NULL;

    if (getenv("HOME")) {
        rc_file = g_strdup_printf("%s/%s", getenv("HOME"), CONFIG_FILE_NAME);
    } else { /* unable to find $HOME, assuming current directory */
        rc_file = g_strdup(CONFIG_FILE_NAME);
    }

    return rc_file;
}

/* converts string to TRUE/FALSE. "true", "yes" or "1" is TRUE, otherwise - FALSE */
gboolean pref_str_to_bool(gchar *val) {
    if (!g_ascii_strcasecmp(val, "true") ||
        !g_ascii_strcasecmp(val, "yes") ||
        !g_ascii_strcasecmp(val, "1")) {
        return TRUE;
    }
    return FALSE;
}

/* converts boolean to string
   returns _pointer_ to "yes" or "no". returned string should NOT be free()'d! */
gchar *pref_bool_to_str(gboolean val) {
    return val ? "yes" : "no";
}

/* we use very lame preferences file format: ``property=value\n''.
   so - there must be no whitespaces on begin/end of line =) */
void load_preferences(void) {
    FILE *fp;
    gchar *rc_file;
    gchar buffer[BUFFER_SIZE];
    gchar **prop_val;

    if (!_theme_name) {
        _theme_name = g_strdup(_default_theme_name);
    }
    rc_file = find_rc_file();
    if ((fp = fopen(rc_file, "r"))) {
        while (fgets(buffer, BUFFER_SIZE, fp)) {
            g_strchomp(buffer);
            prop_val = g_strsplit(buffer, "=", 2);
            if (prop_val[0] && prop_val[0][0] && prop_val[1] && prop_val[1][0]) {
                if (!g_ascii_strcasecmp(prop_val[0], "show_hints")) {
                    _show_next_colors = pref_str_to_bool(prop_val[1]);
                } else if (!g_ascii_strcasecmp(prop_val[0], "show_path")) {
                    _show_path = pref_str_to_bool(prop_val[1]);
                } else if (!g_ascii_strcasecmp(prop_val[0], "show_footprints")) {
                    _show_footprints = pref_str_to_bool(prop_val[1]);
                } else if (!g_ascii_strcasecmp(prop_val[0], "show_destroy")) {
                    _show_destroy = pref_str_to_bool(prop_val[1]);
                } else if (!g_ascii_strcasecmp(prop_val[0], "show_highlight")) {
                    _show_highlight = pref_str_to_bool(prop_val[1]);
                } else if (!g_ascii_strcasecmp(prop_val[0], "theme_name")) {
                    if (_theme_name) {
                        g_free(_theme_name);
                    }
                    _theme_name = g_strdup(prop_val[1]);
                } else if (!g_ascii_strcasecmp(prop_val[0], "rules_xsize")) {
                    rules_set_width(atoi(prop_val[1]));
                } else if (!g_ascii_strcasecmp(prop_val[0], "rules_ysize")) {
                    rules_set_height(atoi(prop_val[1]));
                } else if (!g_ascii_strcasecmp(prop_val[0], "rules_colors")) {
                    rules_set_colors(atoi(prop_val[1]));
                } else if (!g_ascii_strcasecmp(prop_val[0], "rules_next")) {
                    rules_set_next(atoi(prop_val[1]));
                } else if (!g_ascii_strcasecmp(prop_val[0], "rules_destroy")) {
                    rules_set_destroy(atoi(prop_val[1]));
                } else if (!g_ascii_strcasecmp(prop_val[0], "time_limit")) {
                    timer_set_limit(atoi(prop_val[1]));
                } else if (!g_ascii_strcasecmp(prop_val[0], "highlight_dr")) {
                    prefs_set_hl_dr(atoi(prop_val[1]));
                } else if (!g_ascii_strcasecmp(prop_val[0], "highlight_dg")) {
                    prefs_set_hl_dg(atoi(prop_val[1]));
                } else if (!g_ascii_strcasecmp(prop_val[0], "highlight_db")) {
                    prefs_set_hl_db(atoi(prop_val[1]));
                }
            }
            g_strfreev(prop_val);
        }
        fclose(fp);
    }
    g_free(rc_file);
}

/* writes ``property=value\n'' to fp.
   why i made it separate function? for "feature purposes" =) */
void write_pref_string(FILE *fp, gchar *property, gchar *value) {
    fprintf(fp, "%s=%s\n", property, value);
}

void write_pref_int(FILE *fp, gchar *property, gint value) {
    fprintf(fp, "%s=%d\n", property, value);
}

gchar *save_preferences(void) {
    FILE *fp;
    gchar *rc_file/*, *err*/;
    gchar *ret = NULL;

    rc_file = find_rc_file();
    if ((fp = fopen(rc_file, "w"))) {
        write_pref_string(fp, "show_hints", pref_bool_to_str(_show_next_colors));
        write_pref_string(fp, "show_path", pref_bool_to_str(_show_path));
        write_pref_string(fp, "show_footprints", pref_bool_to_str(_show_footprints));
        write_pref_string(fp, "show_destroy", pref_bool_to_str(_show_destroy));
        write_pref_string(fp, "show_highlight", pref_bool_to_str(_show_highlight));
        write_pref_string(fp, "theme_name", _theme_name);
        write_pref_int(fp, "rules_xsize", rules_get_width());
        write_pref_int(fp, "rules_ysize", rules_get_height());
        write_pref_int(fp, "rules_colors", rules_get_colors());
        write_pref_int(fp, "rules_next", rules_get_next());
        write_pref_int(fp, "rules_destroy", rules_get_destroy());
        write_pref_int(fp, "time_limit", timer_get_limit());
        write_pref_int(fp, "highlight_dr", prefs_get_hl_dr());
        write_pref_int(fp, "highlight_dg", prefs_get_hl_dg());
        write_pref_int(fp, "highlight_db", prefs_get_hl_db());
        fclose(fp);
    } else {
        ret = g_strdup_printf(_("Can't write to %s: %s"), rc_file, strerror(errno));
    }
    g_free(rc_file);
    return ret;
}

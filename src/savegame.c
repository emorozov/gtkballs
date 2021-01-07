/* savegame.c - save/load game etc
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 */
#include <gtk/gtk.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <time.h>
#include <stdlib.h>

#include "game.h"

gint parse_save_game(gchar *sgame, gchar **rules, gint *score, gint **board, gint **next) {
    struct stat buf;
    gchar *sdata, *psdata, *srules;
    FILE *f;
    gint i, val;
    gint rw, rh, rc, rn, rd, rlen;

    if (stat(sgame, &buf) != 0 || !S_ISREG(buf.st_mode) || buf.st_size < 20 ||
        (f = fopen(sgame, "r")) == NULL) {
        return 0;
    }

    sdata = g_malloc(buf.st_size);

    if (fread(sdata, 1, buf.st_size, f) != buf.st_size) {
        g_free(sdata);
        return 0;
    }

    fclose(f);

    rlen = rules_get_str_len();
    srules = g_malloc(rlen + 1);
    srules[rlen] = 0;
    memcpy(srules, sdata, rlen);

    if (!rules_get_from_str(srules, &rw, &rh, &rc, &rn, &rd) ||
        sscanf(sdata + rlen, "%010u", score) != 1 ||
        buf.st_size != rlen + 10 + rw * rh * 2 + rn * 2) {
        printf("[%s]\n", srules);
        g_free(srules);
        g_free(sdata);
        return 0;
    }

    g_free(srules);

    *board = g_malloc(rw * rh * sizeof(gint));
    *next = g_malloc(rn * sizeof(gint));

    psdata = sdata + rlen + 10;
    for (i = 0; i < rw * rh; i++, psdata += 2) {
        if (sscanf(psdata, "%02d", &val) != 1 || val < 0 || val > rc) {
            g_free(*next);
            g_free(*board);
            g_free(sdata);
            return 0;
        }
        (*board)[i] = val;
    }

    for (i = 0; i < rn; i++, psdata += 2) {
        if (sscanf(psdata, "%02d", &val) != 1 || val < 0 || val > rc) {
            g_free(*next);
            g_free(*board);
            g_free(sdata);
            return 0;
        }
        (*next)[i] = val;
    }

    *rules = g_strndup(sdata, rlen);
    g_free(sdata);

    return 1;
}

/* check that save game name is in form YYYY-MM-DD-HHMMSS.sav and have correct content
   return string "DD.MM.YYYY HH:MM:SS (score)" on success, NULL on failure */
gchar *is_valid_save_game(gchar *name, gchar *path) {
    guint i, y, m, d, h, min, s;
    gint score, *board, *next;
    gchar *sgame;
    gchar *rules;

    if ((i = sscanf(name, "%04u-%02u-%02u-%02u%02u%02u", &y, &m, &d, &h, &min, &s)) != 6 ||
        !m || m > 12 || !d || d > 31 || h > 23 || min > 59 || s > 61 ||
        (strcmp(name + strlen(name) - 4, ".sav") != 0)) {
        return NULL;
    }

    sgame = g_strconcat(path, G_DIR_SEPARATOR_S, name, NULL);

    i = parse_save_game(sgame, &rules, &score, &board, &next);
    g_free(sgame);

    if (!i) return NULL;

    g_free(rules);
    g_free(board);
    g_free(next);

    return g_strdup_printf("%02d.%02d.%04d %02d:%02d:%02d (%d)",
                           d, m, y, h, min, s, score);
/* FIXME: return rules back
        return g_strdup_printf("%02d.%02d.%04d %02d:%02d:%02d (%d [%dx%d %d %d %d])",
		               d, m, y, h, min, s, score,
		               rules.xsize, rules.ysize, rules.colors, rules.next, rules.destroy);
*/
}

gint get_saved_games(gchar ***gamelist) {
    gchar *datapath;
    struct stat buf;
    struct dirent *dir_entry;
    DIR *directory;
    gchar **games = NULL, *game;
    gint num = 0;

    datapath = g_strconcat(getenv("HOME"), G_DIR_SEPARATOR_S, ".gtkballs", NULL);
    if (stat(datapath, &buf) != 0) { /* no ~/.gtkballs */
        if (mkdir(datapath, 0700) != 0) { /* and cannot create it... */
            g_free(datapath);
            return -1;
        }
    } else if (!S_ISDIR(buf.st_mode)) { /* ~/.gtkballs is not a directory */
        g_free(datapath);
        return -1;
    }
    if ((directory = opendir(datapath))) {
        while ((dir_entry = readdir(directory))) {
            if ((game = is_valid_save_game(dir_entry->d_name, datapath)) != NULL) {
                num++;
                games = g_realloc(games, sizeof(gchar * ) * num * 2);
                games[(num - 1) * 2] = game;
                games[(num - 1) * 2 + 1] = g_strdup_printf("%s%s%s", datapath, G_DIR_SEPARATOR_S, dir_entry->d_name);
            }
        }
        closedir(directory);
    }
    g_free(datapath);
    *gamelist = games;
    return num;
}

gchar *save_game(gchar *rules, gint score, gint *board, gint *nextcolors) {
    FILE *f;
    gchar *fname;
    time_t nowtime;
    gchar ftime[] = "0000-00-00-000000";
    gint i;

    nowtime = time(NULL);
    strftime(ftime, sizeof(ftime), "%Y-%m-%d-%H%M%S", localtime(&nowtime));
    fname = g_strconcat(getenv("HOME"), G_DIR_SEPARATOR_S, ".gtkballs",
                        G_DIR_SEPARATOR_S, ftime, ".sav", NULL);
    if ((f = fopen(fname, "w")) != NULL) {
        chmod(fname, 0600);
        /* TODO: check for errors ! */
        fprintf(f, rules);
        fprintf(f, "%010d", score);
        for (i = 0; i < rules_get_width() * rules_get_height(); i++) {
            fprintf(f, "%02d", board[i]);
        }
        for (i = 0; i < rules_get_next(); i++) {
            fprintf(f, "%02d", nextcolors[i]);
        }
        fclose(f);
    } else {
        return fname;
    }

    g_free(fname);

    return NULL;
}

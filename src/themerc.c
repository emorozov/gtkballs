/* themerc.c - functions that deals with theme rc file
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 */
/* please do NOT translate error messages... =/ */
#include <glib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/* read contents of fname into gchar * */
gchar **trc_open(gchar *fname) {
    gint fd;
    struct stat fds;
    ssize_t rb;
    gchar *rc, **rcs;

    if ((fd = open(fname, O_RDONLY)) == -1) {
        fprintf(stderr, "%s: open() %s failed: %s\n", __FUNCTION__, fname, strerror(errno));
        return NULL;
    }
    if ((fstat(fd, &fds)) == -1) {
        close(fd);
        fprintf(stderr, "%s: fstat() failed: %s\n", __FUNCTION__, strerror(errno));
        return NULL;
    }
    if (!(fds.st_size)) {
        close(fd);
        fprintf(stderr, "%s: zero length file.\n", __FUNCTION__);
        return NULL;
    }
    if (!(rc = malloc(fds.st_size + 1))) {
        close(fd);
        fprintf(stderr, "%s: malloc() failed: cannot alloc %d bytes\n", __FUNCTION__, (int) fds.st_size);
        return NULL;
    }
    if ((rb = read(fd, rc, fds.st_size)) != fds.st_size) {
        free(rc);
        close(fd);
        if (rb == -1) {
            fprintf(stderr, "%s: read() failed: %s\n", __FUNCTION__, strerror(errno));
        } else {
            fprintf(stderr, "%s: read() reads less bytes than expected =/\n", __FUNCTION__);
        }
        return NULL;
    }
    rc[fds.st_size] = 0;
    close(fd);
    rcs = g_strsplit(rc, "\n", 0);
    free(rc);
    return rcs;
}

/* free rc... */
void trc_close(gchar **rcs) {
    if (rcs) {
        g_strfreev(rcs);
    }
}

/* return string value for given parameter. if not found retun NULL */
gchar *trc_get_str(gchar **rcs, gchar *param) {
    gint i;
    gchar **strval, *val;

    if (!rcs) {
        fprintf(stderr, "%s called with uninitialised rcs. strange. \n", __FUNCTION__);
        return NULL;
    }
    if (!param) {
        fprintf(stderr, "%s called with NULL param. strange. \n", __FUNCTION__);
        return NULL;
    }
    for (i = 0; rcs[i]; i++) {
        if (rcs[i][0] && rcs[i][0] != '#' && (strval = g_strsplit(rcs[i], "=", 2)) && strval[0] && strval[1]) {
            if (!(strcmp(g_strstrip(strval[0]), param))) {
                val = g_strdup(g_strstrip(strval[1]));
                g_strfreev(strval);
                return val;
            }
            g_strfreev(strval);
        }
    }
    return NULL;
}

/* return unsigned integer value for given parameter. if not found retun -1 */
gint trc_get_uint(gchar **rcs, gchar *param) {
    gchar *val;
    gint ret;

    if (!(val = trc_get_str(rcs, param))) return -1;
    ret = atoi(val);
    g_free(val);
    return ret;
}

/* scoreboard.c - save/load scores
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <gtk/gtk.h>
#include <glib.h>

#include "gtkballs.h"
#include "scoreboard.h"

#include "child.h" /* child_writer_alive, child_setup */
#include "game.h"

#define BUFFER_SIZE 1024

int _score_fd = -1;

gint score_setup(void) {
	_score_fd = child_setup(SCORE_FILE);
        return _score_fd;
}

void free_score_board_full(struct score_board_full *bf, gint nbf) {
        gint i;

        for(i = 0; i < nbf; i++) {
                g_free(bf[i].rules);
        }
        g_free(bf);
}

gint write_score(struct score_board *b, struct score_board_full *bf, gint nbf) {
  	gint i;
  	gchar *buf, *tname, *tdate, *rules;
        size_t sz;

        if(child_writer_alive() == 0) {
                return FALSE;
        }

  	for(i = 0; i < 10; i++) {
    		if(strlen(b[i].name)) {
                        tname = g_strdup(b[i].name);
                        if(!tname) {
                                tname = g_strdup(_("Unknown"));
                        }
                        tdate = g_strdup(b[i].date);
                        if(!tdate) {
                                tdate = g_strdup(_("Unknown"));
                        }
                        rules = rules_get_as_str();
      			buf = g_strdup_printf("%s\t%i\t%s\t%s\n", tname, b[i].score, tdate, rules);
                        sz = strlen(buf);
    			write(_score_fd, &sz, sizeof(sz));
    			write(_score_fd, buf, strlen(buf));
                        g_free(rules);
                        g_free(tdate);
                        g_free(tname);
                	g_free(buf);
    		}
  	}
  	for(i = 0; i < nbf; i++) {
    		if(strlen(bf[i].name)) {
      			buf = g_strdup_printf("%s\t%i\t%s\t%s\n", bf[i].name, bf[i].score, bf[i].date, bf[i].rules);
                        sz = strlen(buf);
    			write(_score_fd, &sz, sizeof(sz));
    			write(_score_fd, buf, strlen(buf));
                	g_free(buf);
   		}
        }
        sz = 0;
    	write(_score_fd, &sz, sizeof(sz));

  	return TRUE;
}

int score_sort(const void *a, const void *b) {
        if(((const struct score_board *)a)->score == ((const struct score_board *)b)->score) return 0;
        if(((const struct score_board *)a)->score < ((const struct score_board *)b)->score) return 1;
	return -1;
}

gint read_score(struct score_board *b, struct score_board_full **bf, gint *nbf) {
  	FILE *fp;
  	gchar buffer[BUFFER_SIZE];
  	gchar **str_val, *tstr, **tstr_val;
        gint  valid, sc, fsc;
        gsize br, bw;
	struct flock lockinfo;

        gchar *g_rules = NULL;

        memset(b, 0, sizeof(struct score_board) * 10);

  	if(!(fp = fopen(LOCALSTATEDIR SCORE_FILE, "r"))) {
		return FALSE;
        }

    	do {
  		lockinfo.l_whence = SEEK_SET;
  		lockinfo.l_start = 0;
	  	lockinfo.l_len = 0;
    		lockinfo.l_type = F_WRLCK;
		fcntl(fileno(fp), F_GETLK, &lockinfo);
        } while(lockinfo.l_type != F_UNLCK);

        sc = 0;
        fsc = 0;
      	while(fgets(buffer, BUFFER_SIZE, fp)) {
        	g_strchomp(buffer);
        	str_val = g_strsplit(buffer, "\t", 4);
        	if(str_val[0] && str_val[0][0] &&
		   str_val[1] && str_val[1][0] &&
		   str_val[2] && str_val[2][0]) {
                        valid = 0;
                   	if(!str_val[3]) { /* < 2.2.0 file format */
                                g_rules = rules_get_classic_as_str();
                                valid = 1;
                   	} else {
                                if((valid = rules_check_str(str_val[3]))) {
                                	g_rules = g_strdup(str_val[3]);
                                } else { /* > 2.2.0 && < 3.0.2 file format ? */
                                        tstr_val = g_strsplit(str_val[3], "\t", 5);
                                        if(tstr_val[0] && tstr_val[1] && tstr_val[2] &&
                                           tstr_val[3] && tstr_val[4]) {
                                		g_rules = rules_conv_3_0_to_str(tstr_val[0], tstr_val[1], tstr_val[2], tstr_val[3], tstr_val[4]);
                                                if(g_rules) { /* yes. its < 3.0.2 format */
                                                	valid = 1;
                                                } else { /* nope. just piece of shit. */
                                                        g_rules = g_strdup("");
                                                }
                                        }
                                        g_strfreev(tstr_val);
                                }
                   	}
                        if(valid && g_ascii_strcasecmp(str_val[0], "<none>")) {
                                if(rules_is_current_str(g_rules)) {
                                        if(g_utf8_validate(str_val[0], -1, NULL)) {
                                                tstr = g_strdup(str_val[0]);
                                        } else {
	                                        tstr = g_locale_to_utf8(str_val[0], -1, &br, &bw, NULL);
                                        }
                                        if(tstr) {
                                		strncpy(b[sc].name, tstr, sizeof(b[sc].name));
                                                g_free(tstr);
                                        } else {
                                		strncpy(b[sc].name, _("Unknown"), sizeof(b[sc].name));
                                        }
    					b[sc].score = strtol(str_val[1], NULL, 10);
    					if((b[sc].score == LONG_MIN) || (b[sc].score == LONG_MAX)) {
      						b[sc].score = 0;
                                	}
                                        if(g_utf8_validate(str_val[2], -1, NULL)) {
                                                tstr = g_strdup(str_val[2]);
                                        } else {
	                                        tstr = g_locale_to_utf8(str_val[2], -1, &br, &bw, NULL);
                                        }
                                        if(tstr) {
                                		strncpy(b[sc].date, tstr, sizeof(b[sc].date));
                                                g_free(tstr);
                                        } else {
                                		strncpy(b[sc].date, _("Unknown"), sizeof(b[sc].date));
                                        }
                			sc++;
                                } else if(bf) {
                                        *bf = g_realloc(*bf, sizeof(struct score_board_full) * (fsc + 1));
                                	strncpy((*bf)[fsc].name, str_val[0], sizeof((*bf)[fsc].name));
    					(*bf)[fsc].score = strtol(str_val[1], NULL, 10);
    					if(((*bf)[fsc].score == LONG_MIN) || ((*bf)[fsc].score == LONG_MAX)) {
      						(*bf)[fsc].score = 0;
                                	}
                                	strncpy((*bf)[fsc].date, str_val[2], sizeof((*bf)[fsc].date));
                                        (*bf)[fsc].rules = g_strdup(g_rules);
                                        fsc++;
                                }
                        }
                        g_free(g_rules);
        	}
                g_strfreev(str_val);
        }
        fclose(fp);

        qsort(b, 10, sizeof(struct score_board), score_sort);

        if(nbf) {
        	*nbf = fsc;
        }

  	return TRUE;
}

gint insert_entry_in_score_board(struct score_board *board, struct score_board entry) {
  	gint i=0,j;

  	if(entry.score <= 0) {
    		return -1;
        }

  	while(i < 10 && board[i].score > entry.score) {
    		i++;
        }

  	if(i > 9) {
    		return -1;
        }

  	for(j = 8;j >= i; j--) {
      		strncpy(board[j + 1].name, board[j].name, sizeof(board[j + 1].name));
      		strncpy(board[j + 1].date, board[j].date, sizeof(board[j + 1].date));
      		board[j + 1].score = board[j].score;
    	}

  	strncpy(board[i].name, entry.name, sizeof(board[i].name));
  	strncpy(board[i].date, entry.date, sizeof(board[i].date));
  	board[i].score = entry.score;

  	return i;
}

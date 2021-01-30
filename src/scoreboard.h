#ifndef __SCOREBOARD_H__
#define __SCOREBOARD_H__

struct score_board {
   gchar name[30];
   gint  score;
   gchar date[60];
};

struct score_board_full {
   gchar name[30];
   gint  score;
   gchar date[60];
   gchar *rules;
};

void free_score_board_full(struct score_board_full *bf, gint nbf);
gint write_score(struct score_board *, struct score_board_full *, gint);
gint read_score(struct score_board *, struct score_board_full **, gint *);
gint insert_entry_in_score_board(struct score_board *board,
             struct score_board entry);

#endif /* __SCOREBOARD_H__ */

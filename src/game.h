#ifndef __GAME_H__
#define __GAME_H__

gint rules_get_width(void);
gint rules_get_height(void);
gint rules_get_colors(void);
gint rules_get_next(void);
gint rules_get_destroy(void);

gint rules_get_classic_width(void);
gint rules_get_classic_height(void);
gint rules_get_classic_colors(void);
gint rules_get_classic_next(void);
gint rules_get_classic_destroy(void);

void rules_set_width(gint width);
void rules_set_height(gint height);
void rules_set_colors(gint colors);
void rules_set_next(gint next);
void rules_set_destroy(gint destroy);
void rules_set(gint width, gint height, gint colors, gint next, gint destroy);

gchar *rules_get_classic_as_str();
gchar *rules_get_as_str();
gint rules_get_str_len(void);
gboolean rules_get_from_str(gchar *s, gint *width, gint *height, gint *colors, gint *next, gint *destroy);
gboolean rules_check_str(gchar *rstr);
gchar *rules_conv_3_0_to_str(gchar *w, gchar *h, gchar *c, gchar *n, gchar *d);
gboolean rules_is_current_str(gchar *r);

gint game_get_score(void);
void game_set_score(gint score);
gint game_get_hi_score(void);
void game_set_hi_score(gint score);
gint *game_get_board_as_int_arr(void);
gint *game_get_next_as_int_arr(void);
void game_save_state_for_undo(void);
void game_restore_state_from_undo(void);
void game_init_game(gint *balls, gint *nextballs);

gint game_count_free_cells(void);

gint board_get_at_node(gint node);
gint board_get_at_xy(gint x, gint y);

gint board_get_destroy_at_xy(gint x, gint y);

void board_set_at_node(gint node, gint col);
void board_set_at_xy(gint x, gint y, gint col);

gint next_get(gint num);
void next_set(gint num, gint col);

void timer_start(void);
gboolean timer_is_running(void);
gboolean timer_is_expired(void);
gint timer_get_remaining(void);

void timer_set_limit(gint limit);
gint timer_get_limit(void);

gint game_destroy_lines(gboolean count_score);

gint is_actions_locked(void);
void lock_actions(gint lock);

#endif

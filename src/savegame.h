#ifndef __SAVEGAME_H__
#define __SAVEGAME_H__

gint  parse_save_game(gchar *sgame, gchar **rules, gint *score, gint **board, gint **next);
gint  get_saved_games(gchar ***gamelist);
gchar *save_game(gchar *rules, gint score, gint *board, gint *nextcolors);

#endif /* __SAVEGAME_H__ */

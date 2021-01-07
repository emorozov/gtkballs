#ifndef __GTKBALLS__H
#define __GTKBALLS__H

#include <locale.h>
#include <libintl.h>
#define _(String) gettext (String)
#ifdef gettext_noop
#  define N_(String) gettext_noop (String)
#else
#  define N_(String) (String)
#endif

void new_game(void);
void new_turn(gint number, gboolean first);
void undo_move(GtkWidget *widget, gpointer data);
gint destroy_lines(gboolean count_score);

#endif /* __GTKBALLS__H */

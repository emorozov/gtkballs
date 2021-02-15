#ifndef __GTKBALLS__H
#define __GTKBALLS__H

#include <config.h>
#include "gtkcompat.h"

extern GtkWindow * main_window;

#ifdef ENABLE_NLS
#  include <locale.h>
#  include <libintl.h>
#    define _(String) gettext (String)
#    ifdef gettext_noop
#        define N_(String) gettext_noop (String)
#    else
#        define N_(String) (String)
#    endif
#else                           /* ENABLE_NLS not defined */
/* Stubs that do something close enough.  */
#    define textdomain(String) (String)
#    define gettext(String) (String)
#    define dgettext(Domain,Message) (Message)
#    define dcgettext(Domain,Message,Type) (Message)
#    define bindtextdomain(Domain,Directory) (Domain)
#    define _(String) (String)
#    define N_(String) (String)
#endif                          /* ENABLE_NLS */

char * get_config_dir_file (const char * file);
void new_game(void);
void new_turn(gint number, gboolean first);
void undo_move(GtkWidget *widget, gpointer data);
gint destroy_lines(gboolean count_score);

#endif /* __GTKBALLS__H */

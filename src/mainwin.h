#ifndef __MAINWIN_H__
#define __MAINWIN_H__

extern cairo_surface_t * pixsurf;

void mw_create(gint da_width, gint da_height);
void mw_small_balls_add(GtkWidget *child);
void mw_show_hide_next_balls(gboolean show);
void mw_set_hi_score(gint score);
void mw_set_user_score(gint score);

GtkWidget *mw_get_da(void);

#endif

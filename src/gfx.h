#ifndef __GFX__H
#define __GFX__H

typedef enum {
   DIR_LEFT,
   DIR_RIGHT,
   DIR_UP,
   DIR_DOWN,
   DIR_UP_LEFT,
   DIR_DOWN_LEFT,
   DIR_UP_RIGHT,
   DIR_DOWN_RIGHT
} Direction;

void draw_next_balls(void);
void draw_ball(gint ballcolor, gint x, gint y, gint jumpnum, gint destroynum);
void redraw_ball(gint x, gint y);
void redraw_pointer(void);
void draw_board(void);
/* Refill the screen from the backing pixmap */
gboolean boardw_draw_event (GtkWidget *widget, gpointer compat, gpointer data);
void remake_board(gint numoldchilds, gboolean isnextvalid);

void reinit_board(gint *newboard, gint *newnext, gint score, gint oldnext);

void stop_jumping_animation(void);

void set_jump_phase(gint p);

void move_pointer_to(gint x, gint y);
void move_pointer(Direction dir);
void pointer_pressed(gint x, gint y);

#endif

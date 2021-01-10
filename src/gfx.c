/* gfx.c - (almost) all stuff related to drawing
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 */
#include <gtk/gtk.h>
#include <sys/types.h>
#include <sys/time.h>

#include "gtkballs.h"
#include "scoreboard.h" /* read_score */
#include "prefs.h" /* pref_get_*() */
#include "path.h"
#include "theme.h"
#include "gfx.h"
#include "mainmenu.h" /* menu_set_sensitive_undo */
#include "mainwin.h"
#include "game.h"

GdkPixmap *_pixmap=NULL;

/* x and y of cell "under pointer" */
gint _pointer_x = 0, _pointer_y = 0;

/* Balls that shows what colors will appear on next turn */
GtkWidget **_small_balls = NULL;

/* Number of node and color of the jumping ball */
gint _jumping_ball = 0;

gint _animation_in_progress = FALSE;

/* jumping animation phase */
volatile gint _phase = 0;

/* jumping animation timer tag */
guint _timer_tag = 0;

gboolean have_path(gint source_ball, gint target_ball);
void move_ball(gint source_ball, gint target_ball);

gint xy_to_cell_number(gint x, gint y) {
	return find_node_of_x_y(x, y, rules_get_width());
}

gint get_jump_phase(gint x, gint y) {
        if(!_animation_in_progress || xy_to_cell_number(x, y) != _jumping_ball)
                return 0;
        return _phase;
}

void set_jump_phase(gint p) {
        if(!_animation_in_progress)
                return;
        _phase = p;
}

gint get_destroy_phase(gint x, gint y) {
        return board_get_destroy_at_xy(x, y);
}

void update_rectangle(gint x, gint y, gint w, gint h) {
  	GtkWidget *widget = mw_get_da();

        if(!widget || !_pixmap) {
                return;
        }
  	gdk_draw_drawable (gtk_widget_get_window (widget),
			  widget->style->fg_gc[gtk_widget_get_state(widget)],
			  _pixmap, x, y, x, y, w, h);
}

void draw_ball_no_update(gint ballcolor, gint x, gint y, gint jumpnum, gint destroynum) {
  	GtkWidget *widget = mw_get_da();
        GdkGC *gc = widget->style->fg_gc[gtk_widget_get_state(widget)];
        gint cxs = gtkbTheme->emptycell.xsize;
        gint cys = gtkbTheme->emptycell.ysize;
        GtkbPixmap *obj;
        gint xr, yr;

        if(!widget || !_pixmap) {
                return;
        }

        if(pref_get_show_highlight() && x == _pointer_x && y == _pointer_y) {
                obj = &gtkbTheme->hemptycell;
        } else {
                obj = &gtkbTheme->emptycell;
	}
	gdk_draw_pixbuf (_pixmap,
	                 gc,
	                 obj->pixbuf,
	                 0, 0,
	                 x * cxs, y * cys,
	                 cxs, cys,
	                 GDK_RGB_DITHER_NONE, 0, 0);

        if(ballcolor > 0) { 		/* ball */
	        if(!jumpnum && !destroynum) { 	/* still ball */
        	        obj = &gtkbTheme->balls[ballcolor - 1].ball;
	        } else if(jumpnum) { 		/* jumping ball */
		 	obj = &gtkbTheme->balls[ballcolor - 1].jump[jumpnum - 1];
		} else { 			/* disappearing ball */
			obj = &gtkbTheme->balls[ballcolor - 1].destroy[destroynum - 1];
	        }
        } else if(ballcolor < 0) { 	/* paw */
                obj = &gtkbTheme->paws[-1 - ballcolor];
        } else { 			/* empty cell */
                return;
        }

	xr = x * cxs + (cxs - obj->xsize) / 2;
        yr = y * cys + (cys - obj->ysize) / 2;
	gdk_draw_pixbuf (_pixmap,
	                 gc,
	                 obj->pixbuf,
	                 0, 0,
	                 xr, yr,
	                 obj->xsize, obj->ysize,
	                 GDK_RGB_DITHER_NONE, 0, 0);
}

void draw_ball(gint ballcolor, gint x, gint y, gint jumpnum, gint destroynum) {
        gint cxs = gtkbTheme->emptycell.xsize;
        gint cys = gtkbTheme->emptycell.ysize;

  	draw_ball_no_update(ballcolor, x, y, jumpnum, destroynum);
  	update_rectangle(x * cxs, y * cys, cxs, cys);
}

void redraw_ball(gint x, gint y) {
	draw_ball(board_get_at_xy(x, y), x, y, get_jump_phase(x, y), get_destroy_phase(x, y));
}

void redraw_pointer(void) {
	draw_ball(board_get_at_xy(_pointer_x, _pointer_y),
                  _pointer_x, _pointer_y,
                  get_jump_phase(_pointer_x, _pointer_y),
                  get_destroy_phase(_pointer_x, _pointer_y));
}

void draw_board(void) {
  	gint i, j;

        if(!mw_get_da()) {
                return;
        }
    	for(j = 0; j < rules_get_height(); j++) {
  		for(i = 0; i < rules_get_width(); i++) {
      			if(!_animation_in_progress || ((xy_to_cell_number(i, j)) != _jumping_ball)) {
              			draw_ball_no_update(board_get_at_xy(i, j), i, j, 0, 0);
                        }
                }
        }
      	update_rectangle(0, 0, -1, -1);
}

gint inc_with_limit(gint val, gint lim) {
        if(val < lim)
                return val + 1;
        return lim;
}

gint dec_with_limit(gint val, gint lim) {
        if(val > lim)
                return val - 1;
        return lim;
}

gboolean animate_ball(gpointer data) {
  	GtkWidget *widget = GTK_WIDGET(data);
  	int x, y, bc = board_get_at_node(_jumping_ball);

  	find_x_y_of_the_node(&x, &y, _jumping_ball, rules_get_width(), rules_get_height());
  	if(widget != NULL) {
    		draw_ball(bc, x, y, _phase + 1, 0);
        }

  	++_phase;
        if(_phase >= gtkbTheme->balls[bc - 1].jumpphases) {
  		_phase = 0;
        }
	_timer_tag = g_timeout_add(gtkbTheme->balls[bc - 1].jumpdelays[_phase], animate_ball, widget);

  	return FALSE;
}

void stop_jumping_animation(void) {
        if(_animation_in_progress) {
                g_source_remove(_timer_tag);
        	_animation_in_progress = FALSE;
        }
}

void move_pointer_to(gint x, gint y) {
        gint i, jp, dp, xc = _pointer_x, yc = _pointer_y;

        if(x >= rules_get_width() || y >= rules_get_height() || x < 0 || y < 0)
                /* "boundary check" */
                return;
        if(_pointer_x == x && _pointer_y == y)
                return;

        _pointer_x = x;
	_pointer_y = y;
        for(i = 0; i < 2; i++) {
                if(i) {
                        xc = _pointer_x;
                        yc = _pointer_y;
                }
                jp = get_jump_phase(xc, yc);
                dp = get_destroy_phase(xc, yc);
                draw_ball(board_get_at_xy(xc, yc), xc, yc, jp, dp);
        }
}

void move_pointer(Direction dir) {
        gint xn = _pointer_x, yn = _pointer_y;

        if(dir == DIR_LEFT || dir == DIR_UP_LEFT || dir == DIR_DOWN_LEFT)
                xn = dec_with_limit(_pointer_x, 0);
        if(dir == DIR_RIGHT || dir == DIR_UP_RIGHT || dir == DIR_DOWN_RIGHT)
                xn = inc_with_limit(_pointer_x, rules_get_width() - 1);
        if(dir == DIR_UP || dir == DIR_UP_LEFT || dir == DIR_UP_RIGHT)
                yn = dec_with_limit(_pointer_y, 0);
        if(dir == DIR_DOWN || dir == DIR_DOWN_LEFT || dir == DIR_DOWN_RIGHT)
                yn = inc_with_limit(_pointer_y, rules_get_height() - 1);

        if(xn == _pointer_x && yn == _pointer_y)
                return;

        move_pointer_to(xn, yn);
}

void pointer_pressed(gint x, gint y) {
        gint xn, yn, node, bc;

        if(x == -1 || y == -1) {
                x = _pointer_x;
                y = _pointer_y;
        }
        node = xy_to_cell_number(x, y);

        if(_animation_in_progress && node == _jumping_ball) {
                /* already selected ball clicked */
                return;
        }

	if(board_get_at_node(node) > 0) {
        	/* cell with ball pressed */
	  	if(_animation_in_progress) {
                        /* another ball already jumping, stop him */
			stop_jumping_animation();
                        find_x_y_of_the_node(&xn, &yn, _jumping_ball, rules_get_width(), rules_get_height());
                	draw_ball(board_get_at_node(_jumping_ball), xn, yn, 0, 0);
                }
                /* select ball at x,y and start it jumping */
	  	_jumping_ball = node;
	  	bc = board_get_at_node(node);
	  	_animation_in_progress = TRUE;
                _phase = 0;
	  	_timer_tag = g_timeout_add(gtkbTheme->balls[bc - 1].jumpdelays[_phase], animate_ball, mw_get_da());
        } else if(_animation_in_progress && have_path(_jumping_ball, node)) {
                /* cell without ball pressed while ball selected */
                game_save_state_for_undo();
		menu_set_sensitive_undo(TRUE);
                stop_jumping_animation();
	  	move_ball(_jumping_ball, node);
	  	if(!destroy_lines(TRUE)) {
			new_turn(rules_get_next(), FALSE);
                }
                timer_start();
    	}
}

void reinit_board(gint *newboard, gint *newnext, gint score, gint oldnext) {
        struct score_board sb[10];
        gint cells = rules_get_width() * rules_get_height() - 1;

	stop_jumping_animation();
        if(_pointer_x >= rules_get_width()) {
                _pointer_x = rules_get_width() - 1;
        }
        if(_pointer_y >= rules_get_height()) {
        	_pointer_y = rules_get_height() - 1;
        }
        if(rules_get_destroy() > cells) {
                rules_set_destroy(cells);
        }
        if(rules_get_next() > cells) {
                rules_set_next(cells);
        }
        game_init_game(newboard, newnext);
  	read_score(sb, NULL, NULL);
        mw_set_hi_score(score > sb[0].score ? score : sb[0].score);
        mw_set_user_score(score);
        remake_board(oldnext, newnext ? 1 : 0);
        if(!newboard) {
        	new_game();
        } else {
        	menu_set_sensitive_undo(FALSE);
                draw_next_balls();
        }
}

void remake_board(gint numoldchilds, gboolean isnextvalid) {
  	gint  cxs, cys, i;

        if(numoldchilds && numoldchilds != rules_get_next()) {
                if(_small_balls) {
        		for(i = 0; i < numoldchilds; i++) {
                		gtk_widget_destroy(_small_balls[i]);
        		}
                }
        	_small_balls = g_realloc(_small_balls, rules_get_next() * sizeof(GtkWidget *));
        	for(i = 0; i < rules_get_next(); i++) {
                        if(isnextvalid) {
                		_small_balls[i] = gtk_image_new_from_pixbuf(gtkbTheme->balls[next_get(i) - 1].small.pixbuf);
                        } else {
                		_small_balls[i] = gtk_image_new_from_pixbuf(gtkbTheme->balls[0].small.pixbuf);
                        }
                        mw_small_balls_add(_small_balls[i]);
        	}
                mw_show_hide_next_balls(pref_get_show_next_colors());
        }

        cxs = gtkbTheme->emptycell.xsize;
        cys = gtkbTheme->emptycell.ysize;
  	gtk_widget_set_size_request(mw_get_da(), rules_get_width() * cxs, rules_get_height() * cys);
  	if(_pixmap) {
		g_object_unref(_pixmap);
  	}
  	_pixmap = gdk_pixmap_new(mw_get_da()->window, rules_get_width() * cxs, rules_get_height() * cys, -1);
        draw_board();
}

void draw_next_balls(void) {
        gint i;

	for(i = 0; i < rules_get_next(); i++) {
		gtk_image_set_from_pixbuf(GTK_IMAGE(_small_balls[i]), gtkbTheme->balls[next_get(i) - 1].small.pixbuf);
        }
}

/* Refill the screen from the backing pixmap */
gint expose_event(GtkWidget *widget, GdkEventExpose *event) {
	GdkRectangle *rects;
  	int n_rects;
  	int i;

  	gdk_region_get_rectangles(event->region, &rects, &n_rects);

  	for(i = 0; i < n_rects; i++) {
		gdk_draw_drawable (gtk_widget_get_window (widget), 
				  widget->style->fg_gc[gtk_widget_get_state(widget)],
 				  _pixmap, rects[i].x, rects[i].y, rects[i].x, rects[i].y,
			  	  rects[i].width, rects[i].height);
    	}

  	g_free (rects);

  	return FALSE;
}

void find_pawnum_and_direction(gint pawx, gint pawy, gint x, gint y, gint *pawnum, gint *direction) {
	if(pawy < y) {
        	*pawnum = 2;
                if(*direction == 1) *pawnum = 6;
        	if(*direction == 3) *pawnum = 7;
        	*direction = 2; /* up */
	} else if(pawy > y) {
        	*pawnum = 0;
                if(*direction == 1) *pawnum = 4;
                if(*direction == 3) *pawnum = 5;
	        *direction = 0; /* down */
	} else if(pawx < x) {
        	*pawnum = 1;
                if(*direction == 0) *pawnum = 4;
                if(*direction == 2) *pawnum = 6;
	        *direction = 1; /* left */
	} else if(pawx > x) {
        	*pawnum = 3;
                if(*direction == 0) *pawnum = 5;
                if(*direction == 2) *pawnum = 7;
	        *direction = 3; /* right */
        }
}

gint find_direction(gint pawx, gint pawy, gint x, gint y) {
	if(pawy < y) return 2; /* up */
	if(pawy > y) return 0; /* down */
        if(pawx < x) return 1; /* left */
        if(pawx > x) return 3; /* right */
        return -1; /* should never happens */
}

/* FIXME: remake it properly =/ */
gboolean have_path(gint source_ball, gint target_ball) {
  	gint nodes[rules_get_width() * rules_get_height()];
  	gint path[rules_get_width() * rules_get_height()];
        gint *np = nodes;
        gint i;

        for(i = 0; i < rules_get_height() * rules_get_width(); i++) {
                *np++ = board_get_at_node(i) > 0 ? -1 : 0;
        }
  	nodes[source_ball] = nodes[target_ball] = 0;
  	if(!find_path(nodes, source_ball, target_ball, path, rules_get_width(), rules_get_height())) {
		return 0;
        }
	return 1;
}

void move_ball(gint source_ball, gint target_ball) {
  	gint nodes[rules_get_width() * rules_get_height()];
  	gint path[rules_get_width() * rules_get_height()];
	gint pawx = -1, pawy = -1;
  	gint i, j, k, phase, blah;
  	gint x, y, color, tbx, tby, bc = board_get_at_node(_jumping_ball);
  	gint direction, pawnum;
        gint *np = nodes;
        struct timeval tvs, tve;

        g_assert(board_get_at_node(target_ball) == 0);
  	find_x_y_of_the_node(&tbx, &tby, target_ball, rules_get_width(), rules_get_height());

        for(blah = 0; blah < rules_get_height() * rules_get_width(); blah++) {
                *np++ = board_get_at_node(blah) > 0 ? -1 : 0;
        }

  	nodes[source_ball] = nodes[target_ball] = 0;
  	g_assert(find_path(nodes, source_ball, target_ball, path, rules_get_width(), rules_get_height()) == 1);

  	find_x_y_of_the_node(&x, &y, source_ball, rules_get_width(), rules_get_height());
  	color = board_get_at_node(source_ball);
        board_set_at_node(source_ball, 0);
  	phase = 0;

      	draw_ball(0, x, y, 0, 0);
  	if(pref_get_show_path()) {
		lock_actions(1);
      		for(k = path[0] - 1; k; k--) {
                        gettimeofday(&tvs, NULL);
	  		find_x_y_of_the_node(&i, &j, path[k], rules_get_width(), rules_get_height());
	  		if(k == path[0] - 1) {
	      			/* x and y are the coordinates of starting position */
	      			pawx = x;
	      			pawy = y;
	  		} else {
	      			find_x_y_of_the_node(&pawx, &pawy, path[k + 1], rules_get_width(), rules_get_height());
	  		}
          		if(k != path[0] - 1) {
                                find_pawnum_and_direction(pawx, pawy, i, j, &pawnum, &direction);
          		} else {
                		pawnum = direction = find_direction(pawx, pawy, i, j);
          		}
	  		if(pref_get_show_footprints()) {
                                board_set_at_xy(pawx, pawy, -1 - pawnum);
                                draw_ball(-1 - pawnum, pawx, pawy, 0, 0);
                        }
                        board_set_at_xy(i, j, bc);
            		draw_ball(bc, i, j, 0, 0);
                        do {
                        	gtk_main_iteration_do(0);
                        	gettimeofday(&tve, NULL);
                        } while((tve.tv_sec  - tvs.tv_sec) * 1000000 + tve.tv_usec - tvs.tv_usec < 100000);
                        board_set_at_xy(i, j, 0);
            		draw_ball(0, i, j, 0, 0);
      		}
		lock_actions(0);
      		if(k == path[0] - 1) {
	  		/* x and y are the coordinates of starting position */
	  		pawx = x;
	  		pawy = y;
      		} else {
	  		find_x_y_of_the_node(&pawx, &pawy, path[k + 1], rules_get_width(), rules_get_height());
      		}
      		if (pref_get_show_footprints()) {
                	find_pawnum_and_direction(pawx, pawy, tbx, tby, &pawnum, &direction);
                        board_set_at_xy(pawx, pawy, -1 - pawnum);
                        draw_ball(-1 - pawnum, pawx, pawy, 0, 0);
                }
  	}
  	if(pref_get_show_path() && pref_get_show_footprints()) {
        	board_set_at_node(source_ball, 0);
      		for(k = path[0] - 1; k; k--) {
                        board_set_at_node(path[k], 0);
                }
        }
        board_set_at_node(target_ball, color);
        draw_ball(color, tbx, tby, 0, 0);
}

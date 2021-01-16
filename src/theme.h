#ifndef __THEME_H__
#define __THEME_H__

typedef struct _GtkbTheme GtkbTheme;
typedef struct _GtkbPixmap GtkbPixmap;
typedef struct _GtkbBall GtkbBall;

struct _GtkbPixmap {
   GdkPixbuf  *pixbuf;
   gint      xsize, ysize;
};

struct _GtkbBall {
   GtkbPixmap ball;           /* still picture of ball */
   GtkbPixmap small;          /* small picture of ball */
   gint       jumpphases;
   GtkbPixmap *jump;
   gint       *jumpdelays;
   gint       destroyphases;
   GtkbPixmap *destroy;
   gint       *destroydelays;
};

struct _GtkbTheme {
   GtkbPixmap emptycell;      /* pixmap for empty cell */
   GtkbPixmap hemptycell;     /* highlighted pixmap for empty cell */
   GtkbPixmap paws[8];        /* 8 pixmaps for footprints */
   gint       numballs;       /* number of balls */
   GtkbBall   *balls;         /* array of balls */
   gint       maxdestphases;  /* maximal number of destroy phases */
};


extern GtkbTheme *gtkbTheme;

gint gtkb_theme_free_handler(GtkWidget *widget, gpointer data);
int load_theme(gchar *themename);

gchar **get_available_themes(void);

gint gtkb_theme_get_balls_num(void);

gint gtkb_theme_get_coord_at_x(gint x);
gint gtkb_theme_get_coord_at_y(gint y);

void gtkb_make_hl_pixmap(GtkbTheme *theme);

gint theme_get_width(void);
gint theme_get_height(void);

#endif

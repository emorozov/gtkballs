#ifndef __PREFS_H
#define __PREFS_H

void load_preferences(void);
gchar *save_preferences(void);

gchar *pref_get_theme_name(void);
gchar *pref_get_default_theme_name(void);
void pref_set_theme_name(gchar *name);

gint pref_get_show_next_colors(void);
void pref_set_show_next_colors(gint val);
gint pref_get_show_path(void);
void pref_set_show_path(gint val);
gint pref_get_show_footprints(void);
void pref_set_show_footprints(gint val);
gint pref_get_show_destroy(void);
void pref_set_show_destroy(gint val);
gint pref_get_show_highlight(void);
void pref_set_show_highlight(gint val);

gint prefs_get_hl_dr(void);
void prefs_set_hl_dr(gint dr);
gint prefs_get_hl_dg(void);
void prefs_set_hl_dg(gint dg);
gint prefs_get_hl_db(void);
void prefs_set_hl_db(gint db);


#endif /* __PREFS_H */

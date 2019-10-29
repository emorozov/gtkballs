#ifndef __MAINMENU_H__
#define __MAINMENU_H__

void menu_set_sensitive_undo(gboolean sensitive);
void menu_set_sensitive_all(gboolean sensitive);
void menu_get_main(GtkWidget *window, GtkWidget **menubar);

#endif

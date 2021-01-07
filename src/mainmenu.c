/* mainmenu.c - functions that deal with "main menu"
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 */
#include <gtk/gtk.h>

#include "gtkballs.h"        /* new_game(), undo_move(), show_rules() */
#include "about.h"        /* about() */
#include "rules.h"        /* show_rules() */
#include "savedialog.h"        /* save/load_game_cb() */
#include "preferences.h"    /* preferences_dialog() */
#include "scoreboard.h"         /* for halloffame =/ */
#include "halloffame.h"         /* show_hall_of_fame() */
#include "rulesdialog.h"        /* rules_dialog() */

static GtkActionEntry _menu_entries[] = {
        {"GameMenu",     NULL,                  N_("_Game")},
        {"EditMenu",     NULL,                  N_("_Edit")},
        {"SettingsMenu", NULL,                  N_("_Settings")},
        {"HelpMenu",     NULL,                  N_("_Help")},
        {"New",          GTK_STOCK_NEW,         N_("_New"),          "<control>N", N_("Start new game"),                 G_CALLBACK(
                new_game)},
        {"Rules",        GTK_STOCK_PROPERTIES,  N_("_Rules"),        "<control>R", N_("Change game rules"),              G_CALLBACK(
                rules_dialog)},
        {"Save",         GTK_STOCK_SAVE,        N_("_Save"),         "<control>S", N_("Save game"),                      G_CALLBACK(
                save_game_cb)},
        {"Load",         GTK_STOCK_OPEN,        N_("_Load"),         "<control>L", N_("Load game"),                      G_CALLBACK(
                load_game_cb)},
        {"HallOfFame",   NULL,                  N_("_Hall of fame"), "<control>H", N_("Show Hall of Fame"),              G_CALLBACK(
                show_hall_of_fame_cb)},
        {"Quit",         GTK_STOCK_QUIT,        N_("_Quit"),         "<control>Q", N_("Quit program"),                   G_CALLBACK(
                gtk_main_quit)},
        {"Undo",         GTK_STOCK_UNDO,        N_("_Undo"),         "<control>U", N_("Undo last move"),                 G_CALLBACK(
                undo_move)},
        {"Preferences",  GTK_STOCK_PREFERENCES, N_("_Preferences"),  "<control>P", N_("Change game settings"),           G_CALLBACK(
                preferences_dialog)},
        {"RulesHelp",    GTK_STOCK_HELP,        N_("_Rules"),        "F1",         N_("Display help about game rules"),  G_CALLBACK(
                show_rules)},
        {"About",        NULL,                  N_("_About"),        "<control>A", N_("Show information about program"), G_CALLBACK(
                about)}
};

static const char *_ui_description =
        "<ui>"
        "  <menubar name='MainMenu'>"
        "    <menu action='GameMenu'>"
        "      <menuitem action='New'/>"
        "      <menuitem action='Rules'/>"
        "      <separator/>"
        "      <menuitem action='Save'/>"
        "      <menuitem action='Load'/>"
        "      <separator/>"
        "      <menuitem action='HallOfFame'/>"
        "      <separator/>"
        "      <menuitem action='Quit'/>"
        "    </menu>"
        "    <menu action='EditMenu'>"
        "      <menuitem action='Undo'/>"
        "    </menu>"
        "    <menu action='SettingsMenu'>"
        "      <menuitem action='Preferences'/>"
        "    </menu>"
        "    <menu action='HelpMenu'>"
        "      <menuitem action='RulesHelp'/>"
        "      <separator/>"
        "      <menuitem action='About'/>"
        "    </menu>"
        "  </menubar>"
        "</ui>";

GtkActionGroup *_action_group;
GtkUIManager *_ui_manager;

static gchar *_menu_translate(const gchar *path, gpointer data) {
    return gettext(path);
}

void menu_get_main(GtkWidget *window, GtkWidget **menubar) {
    GtkAccelGroup *accel_group;

    _action_group = gtk_action_group_new("MenuActions");
    gtk_action_group_set_translate_func(_action_group, _menu_translate, NULL, NULL);
    gtk_action_group_add_actions(_action_group, _menu_entries, G_N_ELEMENTS(_menu_entries), window);
    _ui_manager = gtk_ui_manager_new();
    gtk_ui_manager_set_add_tearoffs(_ui_manager, 1);
    gtk_ui_manager_insert_action_group(_ui_manager, _action_group, 0);
    accel_group = gtk_ui_manager_get_accel_group(_ui_manager);
    gtk_window_add_accel_group(GTK_WINDOW(window), accel_group);
    gtk_ui_manager_add_ui_from_string(_ui_manager, _ui_description, -1, NULL);
    *menubar = gtk_ui_manager_get_widget(_ui_manager, "/MainMenu");
}

void _menu_set_sensitive(const gchar *path, gboolean sensitive) {
    GtkWidget *widget;

    widget = gtk_ui_manager_get_widget(_ui_manager, path);
    gtk_widget_set_sensitive(widget, sensitive);
}

void menu_set_sensitive_undo(gboolean sensitive) {
    _menu_set_sensitive("/MainMenu/EditMenu/Undo", sensitive);
}

void menu_set_sensitive_all(gboolean sensitive) {
    gtk_action_group_set_sensitive(_action_group, sensitive);
}

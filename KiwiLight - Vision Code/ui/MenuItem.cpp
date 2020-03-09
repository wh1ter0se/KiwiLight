#include "UI.h"

/**
 * Source file for the MenuItem class.
 * Written By: Brach Knutson
 */


using namespace KiwiLight;

/**
 * Create a new MenuItem with the given name and action when clicked.
 */
MenuItem::MenuItem(std::string name) {
    this->menuitem = gtk_menu_item_new_with_label(name.c_str());
    this->menu = gtk_menu_new();
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(this->menuitem), this->menu);
}

void MenuItem::AddSubmenuItem(SubMenuItem item) {
    gtk_menu_shell_append(GTK_MENU_SHELL(this->menu), item.GetWidget());
}


void MenuItem::SetName(std::string name) {
    gtk_widget_set_name(this->menu, name.c_str());
}
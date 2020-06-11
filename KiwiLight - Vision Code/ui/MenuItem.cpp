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
    this->widget = gtk_menu_item_new_with_label(name.c_str());
    this->menu = gtk_menu_new();
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(this->widget), this->menu);
}

/**
 * Add the given Submenu item to the MenuItem.
 */
void MenuItem::AddSubmenuItem(SubMenuItem item) {
    gtk_menu_shell_append(GTK_MENU_SHELL(this->menu), item.GetWidget());
}

#include "UI.h"

/**
 * Source file for the SubMenuItem class.
 * Written By: Brach Knutson
 */


using namespace KiwiLight;

/**
 * Create a submenu item with the given name, when clicked, the item will call callback.
 */
SubMenuItem::SubMenuItem(std::string name, void(* callback)()) {
    this->submenuitem = gtk_menu_item_new_with_label(name.c_str());
    g_signal_connect(this->submenuitem, "activate", G_CALLBACK(callback), NULL); // connect the callback to the menu item.
}

void SubMenuItem::SetName(std::string name) {
    gtk_widget_set_name(this->submenuitem, name.c_str());
}
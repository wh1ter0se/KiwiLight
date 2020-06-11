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
    this->widget = gtk_menu_item_new_with_label(name.c_str());
    g_signal_connect(this->widget, "activate", G_CALLBACK(callback), NULL); // connect the callback to the menu item.
}


void SubMenuItem::SetText(std::string newLabel) {
    gtk_menu_item_set_label(GTK_MENU_ITEM(this->widget), newLabel.c_str());
}
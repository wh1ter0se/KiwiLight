#include "UI.h"

/**
 * Source file for the SubMenuItem class.
 * Written By: Brach Knutson
 */

using namespace std;
using namespace KiwiLight;

/**
 * Create a submenu item with the given name, when clicked, the item will call callback.
 */
SubMenuItem::SubMenuItem(string name, void(* callback)()) {
    this->submenuitem = gtk_menu_item_new_with_label(name.c_str());
    g_signal_connect(this->submenuitem, "activate", G_CALLBACK(callback), NULL); // connect the callback to the menu item.
}
#include "UI.h"

/**
 * Source file for the MenuBar class.
 * Written By: Brach Knutson
 */


using namespace KiwiLight;

/**
 * Creates a new MenuBar.
 */
MenuBar::MenuBar() {
    this->menubar = gtk_menu_bar_new();
}

/**
 * Add the given menu item to the bar.
 */
void MenuBar::AddItem(MenuItem item) {
    gtk_menu_shell_append(GTK_MENU_SHELL(this->menubar), item.GetWidget());
}


void MenuBar::SetName(std::string name) {
    gtk_widget_set_name(this->menubar, name.c_str());
}
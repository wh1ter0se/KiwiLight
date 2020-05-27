#include "UI.h"

/**
 * Source file for the Widget class.
 * Written By: Brach Knutson
 */

using namespace KiwiLight;

/**
 * Returns the raw GtkWidget associated with this object.
 */
GtkWidget *Widget::GetWidget() {
    return this->widget;
}

/**
 * Sets the name of the Widget, and determines styling rules.
 */
void Widget::SetName(std::string name) {
    gtk_widget_set_name(this->widget, name.c_str());
}
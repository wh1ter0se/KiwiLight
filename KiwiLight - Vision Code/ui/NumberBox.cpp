#include "UI.h"

/**
 * Source file for the NumberBox class.
 * Written By: Brach Knutson
 */

using namespace KiwiLight;

NumberBox::NumberBox(double min, double max, double value) {
    this->min = min;
    this->max = max;

    this->numberBox = gtk_spin_button_new_with_range(min, max, 1.0);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(this->numberBox), value);
}


NumberBox::NumberBox(double min, double max, double step, double value) {
    this->min = min;
    this->max = max;
    this->numberBox = gtk_spin_button_new_with_range(min, max, step);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(this->numberBox), value);
}


void NumberBox::SetValue(double value) {
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(this->numberBox), value);
}


double NumberBox::GetValue() {
    return gtk_spin_button_get_value(GTK_SPIN_BUTTON(this->numberBox));
}


void NumberBox::SetName(std::string name) {
    gtk_widget_set_name(this->numberBox, name.c_str());
}
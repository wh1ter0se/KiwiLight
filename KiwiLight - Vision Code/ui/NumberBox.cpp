#include "UI.h"

/**
 * Source file for the NumberBox class.
 * Written By: Brach Knutson
 */

using namespace KiwiLight;

/**
 * Creates a new NumberBox.
 * @param min The minimum value of the NumberBox
 * @param max The maximum value of the NumberBox
 * @param value The inital value of the NumberBox.
 */
NumberBox::NumberBox(double min, double max, double value) {
    this->min = min;
    this->max = max;

    this->widget = gtk_spin_button_new_with_range(min, max, 1.0);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(this->widget), value);
}

/**
 * Creates a new NumberBox
 * @param min The minimum value of the NumberBox
 * @param max The maximum value of the NumberBox
 * @param step The increment used when the +/- buttons are pressed.
 * @param value The inital value.
 */
NumberBox::NumberBox(double min, double max, double step, double value) {
    this->min = min;
    this->max = max;
    this->widget = gtk_spin_button_new_with_range(min, max, step);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(this->widget), value);
}

/**
 * Sets the value of the NumberBox to value.
 */
void NumberBox::SetValue(double value) {
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(this->widget), value);
}

/**
 * Returns the value of the NumberBox.
 */
double NumberBox::GetValue() {
    return gtk_spin_button_get_value(GTK_SPIN_BUTTON(this->widget));
}
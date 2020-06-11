#include "UI.h"

/**
 * Source file for the Slider class.
 * Written By: Brach Knutson
 */


using namespace KiwiLight;

/**
 * Creates a new Slider.
 * @param min The minimum value of the Slider.
 * @param max The maximum value of the Slider.
 * @param step The increment rate of the Slider.
 * @param value The inital value of the Slider
 */
Slider::Slider(double min, double max, double step, double value) {
    this->widget = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, min, max, step);
    gtk_range_set_value(GTK_RANGE(this->widget), gdouble(value));
}

/**
 * Creates a new Slider.
 * @param horizontal Whether or not the Slider is horizontal (will be vertical if false)
 * @param min The minimum value of the Slider.
 * @param max The maximum value of the Slider.
 * @param step The increment rate of the Slider.
 * @param value The inital value of the Slider.
 */
Slider::Slider(bool horizontal, double min, double max, double step, double value) {
    // this->slider = gtk_scale_new_with_range((horizontal ? GTK_ORIENTATION_HORIZONTAL : GTK_ORIENTATION_VERTICAL), min, max, step);
    this->widget = gtk_scale_new_with_range(GTK_ORIENTATION_VERTICAL, min, max, step);
    gtk_range_set_value(GTK_RANGE(this->widget), gdouble(value));
}

/**
 * Sets the value of the Slider.
 */
void Slider::SetValue(double value) {
    gtk_range_set_value(GTK_RANGE(this->widget), value);
}

/**
 * Returns the value of the Slider.
 */
double Slider::GetValue() {
    return gtk_range_get_value(GTK_RANGE(this->widget));
}

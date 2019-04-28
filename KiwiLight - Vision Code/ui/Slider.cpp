#include "UI.h"

/**
 * Source file for the Slider class.
 * Written By: Brach Knutson
 */


using namespace KiwiLight;

Slider::Slider(std::string label, int min, int max, double step, double value) {
    // this->slider = gtk_hscale_new_with_range(min, max, step);
    this->slider = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, min, max, step);
    gtk_range_set_value(GTK_RANGE(this->slider), gdouble(value));
}

void Slider::SetValue(double value) {
    gtk_range_set_value(GTK_RANGE(this->slider), value);
}


double Slider::GetValue() {
    return gtk_range_get_value(GTK_RANGE(this->slider));
}
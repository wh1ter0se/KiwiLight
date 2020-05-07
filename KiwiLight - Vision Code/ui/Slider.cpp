#include "UI.h"

/**
 * Source file for the Slider class.
 * Written By: Brach Knutson
 */


using namespace KiwiLight;

Slider::Slider(double min, double max, double step, double value) {
    this->slider = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, min, max, step);
    gtk_range_set_value(GTK_RANGE(this->slider), gdouble(value));
}


Slider::Slider(bool horizontal, double min, double max, double step, double value) {
    // this->slider = gtk_scale_new_with_range((horizontal ? GTK_ORIENTATION_HORIZONTAL : GTK_ORIENTATION_VERTICAL), min, max, step);
    this->slider = gtk_scale_new_with_range(GTK_ORIENTATION_VERTICAL, min, max, step);
    gtk_range_set_value(GTK_RANGE(this->slider), gdouble(value));
}


void Slider::SetValue(double value) {
    gtk_range_set_value(GTK_RANGE(this->slider), value);
}


double Slider::GetValue() {
    return gtk_range_get_value(GTK_RANGE(this->slider));
}


void Slider::SetName(std::string name) {
    gtk_widget_set_name(this->slider, name.c_str());
}
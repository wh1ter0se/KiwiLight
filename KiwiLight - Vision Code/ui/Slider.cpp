#include "UI.h"

/**
 * Source file for the Slider class.
 * Written By: Brach Knutson
 */

using namespace std;
using namespace KiwiLight;

Slider::Slider(string label, int min, int max, double step, double value) {
    this->slider = gtk_hscale_new_with_range(min, max, step);
    gtk_range_set_value(GTK_RANGE(this->slider), gdouble(value));
}


void Slider::SetValue(double value) {
    gtk_range_set_value(GTK_RANGE(this->slider), value);
}


double Slider::GetValue() {
    return gtk_range_get_value(GTK_RANGE(this->slider));
}
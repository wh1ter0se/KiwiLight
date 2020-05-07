#include "UI.h"

/**
 * Source file for the LabeledSlider class.
 * Written By: Brach Knutson
 */

using namespace KiwiLight;


LabeledSlider::LabeledSlider(std::string label, double min, double max, double step, double value) {
    this->panel = Panel(false, 0);
        this->label = Label(label.c_str());
            this->panel.Pack_start(this->label.GetWidget(), false, false, 0);
        this->slider = Slider(min, max, step, value);
            this->panel.Pack_start(this->slider.GetWidget(), false, false, 0);

    this->labeledslider = this->panel.GetWidget();
}


LabeledSlider::LabeledSlider(bool horizontal, std::string label, double min, double max, double step, double value) {
    this->panel = Panel(false, 0);
        this->label = Label(label.c_str());
            this->panel.Pack_start(this->label.GetWidget(), false, false, 0);
        this->slider = Slider(horizontal, min, max, step, value);
            this->panel.Pack_start(this->slider.GetWidget(), false, false, 0);

    this->labeledslider = this->panel.GetWidget();
}


double LabeledSlider::GetValue() {
    return (double) this->slider.GetValue();
}


void LabeledSlider::SetValue(double value) {
    this->slider.SetValue(value);
}


void LabeledSlider::SetLabel(std::string text) {
    this->label.SetText(text);
}


void LabeledSlider::SetName(std::string name) {
    gtk_widget_set_name(this->labeledslider, name.c_str());
}
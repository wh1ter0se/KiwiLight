#include "UI.h"

/**
 * Source file for the LabeledSlider class.
 * Written By: Brach Knutson
 */

using namespace KiwiLight;

/**
 * Creates a new LabeledSlider.
 * @param label The Label text of the slider.
 * @param min The minimum value of the slider.
 * @param max The maximum value of the slider.
 * @param step The increment rate of the slider.
 * @param value The inital value of the slider.
 */
LabeledSlider::LabeledSlider(std::string label, double min, double max, double step, double value) {
    this->panel = Panel(false, 0);
        this->label = Label(label.c_str());
            this->panel.Pack_start(this->label.GetWidget(), false, false, 0);
        this->slider = Slider(min, max, step, value);
            this->panel.Pack_start(this->slider.GetWidget(), false, false, 0);

    this->widget = this->panel.GetWidget();
}

/**
 * Creates a new LabeledSlider
 * @param horizontal True if the slider should be horizontal, false otherwise.
 * @param label The label text of the slider.
 * @param min The minimum value of the slider.
 * @param max The maximum value of the slider.
 * @param step The increment rate of the slider.
 * @param value The inital value of the slider.
 */
LabeledSlider::LabeledSlider(bool horizontal, std::string label, double min, double max, double step, double value) {
    this->panel = Panel(false, 0);
        this->label = Label(label.c_str());
            this->panel.Pack_start(this->label.GetWidget(), false, false, 0);
        this->slider = Slider(horizontal, min, max, step, value);
            this->panel.Pack_start(this->slider.GetWidget(), false, false, 0);

    this->widget = this->panel.GetWidget();
}

/**
 * Returns the current value of the slider.
 */
double LabeledSlider::GetValue() {
    return (double) this->slider.GetValue();
}

/**
 * Sets the current value of the slider.
 */
void LabeledSlider::SetValue(double value) {
    this->slider.SetValue(value);
}

/**
 * Sets the label text of the slider.
 */
void LabeledSlider::SetLabel(std::string text) {
    this->label.SetText(text);
}
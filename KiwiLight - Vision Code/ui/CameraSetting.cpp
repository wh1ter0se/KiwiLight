#include "UI.h"

/**
 * Source file for the CameraSetting class.
 * Written By: Brach Knutson
 */


using namespace KiwiLight;

/**
 * Creates a new CameraSetting widget, using the given std::string from V4L
 */
CameraSetting::CameraSetting(std::string name, int valueName, int min, int max, int value) {
    this->name = name;
    this->valueName = valueName;
    this->min = min;
    this->max = max;
    this->value = value;


    //create the widget
    std::string boundString = 
        "Min: " + std::to_string(min) + 
        ",  Max: " + std::to_string(max) +
        ", ID: " + std::to_string(valueName);

    std::string nameString = this->name + " (" + boundString + ")";

    Panel main = Panel(true, 0);
        this->nameLabel = Label(nameString);
            main.Pack_start(nameLabel.GetWidget(), true, true, 0);
        this->input = NumberBox(min, max, value);
            main.Pack_start(this->input.GetWidget(), true, true, 0);

    this->camerasetting = main.GetWidget();
}

/**
 * Gets and returns the value of the widget.
 */
int CameraSetting::GetValue() {
    return (int) this->input.GetValue();
}

/**
 * Returns an integer describing the name of the setting that this widget embodies
 */
int CameraSetting::GetValueName() {
    return this->valueName;
}

/**
 * Sets the value of the setting to newValue.
 */
void CameraSetting::SetValue(int newValue) {
    this->value = newValue;
    this->input.SetValue((double) value);
}


void CameraSetting::SetName(std::string name) {
    gtk_widget_set_name(this->camerasetting, name.c_str());
}

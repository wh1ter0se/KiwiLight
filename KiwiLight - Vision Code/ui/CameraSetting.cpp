#include "UI.h"

/**
 * Source file for the CameraSetting class.
 * Written By: Brach Knutson
 */


using namespace KiwiLight;

/**
 * Creates a new CameraSetting widget.
 */
CameraSetting::CameraSetting(std::string, int valueName, double value) {
    this->name = name;
    this->valueName = valueName;
    this->value = value;


    //create the widget
    std::string boundString = 
        "Min: " + std::to_string(min) + 
        ", Max: " + std::to_string(max) +
        ", ID: " + std::to_string(valueName);

    std::string nameString = this->name + " (" + boundString + ")";

    Panel main = Panel(true, 0);
        this->nameLabel = Label(nameString);
            main.Pack_start(nameLabel.GetWidget(), true, true, 0);
        this->input = NumberBox(min, max, 0.01, value);
            main.Pack_start(this->input.GetWidget(), true, true, 0);

    this->widget = main.GetWidget();
}

/**
 * Gets and returns the value of the widget.
 */
double CameraSetting::GetValue() {
    return this->input.GetValue();
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
void CameraSetting::SetValue(double newValue) {
    this->value = newValue;
    this->input.SetValue(value);
}

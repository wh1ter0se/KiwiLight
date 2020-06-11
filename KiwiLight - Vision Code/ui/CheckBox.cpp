#include "UI.h"

/**
 * Source file for the CheckBox class.
 * Written By: Brach Knutson
 */


using namespace KiwiLight;

/**
 * Creates a new CheckBox instance. the state will default to value of checked.
 */
CheckBox::CheckBox(std::string name, bool checked) {
    this->widget = gtk_check_button_new_with_label(name.c_str());
    SetState(checked);
}

/**
 * Sets the state of the checkbox to the value of checked.
 */
void CheckBox::SetState(bool checked) {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(this->widget), checked);
}

/**
 * Returns the state of the checkbox. True for checked, false for not.
 */
bool CheckBox::GetState() {
    return gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(this->widget));
}
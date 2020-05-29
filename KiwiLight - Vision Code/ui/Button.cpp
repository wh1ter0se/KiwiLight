#include "UI.h"

/**
 * Source file for the UI Button class.
 * Written By: Brach Knutson
 */


using namespace KiwiLight;

/**
 * Creates a new button object given the label text text and the callback "callback"
 */
Button::Button(std::string text, void(*callback)()) {
    this->text = text;

    this->widget = gtk_button_new_with_label(text.c_str());
    g_signal_connect(this->widget, "clicked", G_CALLBACK(callback), NULL);
}

/**
 * Sets the text of the Button.
 */
void Button::SetText(std::string text) {
    gtk_button_set_label(GTK_BUTTON(this->widget), text.c_str());
}

/**
 * Sets the callback of the Button.
 * DEPRECATED: This method has not been implemented and is therfore not used in KiwiLight.
 */
[[deprecated("This method has not been implemented and is therfore not used in KiwiLight.")]]
void Button::SetCallback( void(*callback)() ) {
    
}

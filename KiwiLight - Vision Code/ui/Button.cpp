#include "UI.h"

/**
 * Source file for the UI Button class.
 * Written By: Brach Knutson
 */


using namespace KiwiLight;

/**
 * Creates a new button object given the label text text and the callback callback
 */
Button::Button(std::string text, void(*callback)()) {
    this->text = text;

    this->button = gtk_button_new_with_label(text.c_str());
    g_signal_connect(this->button, "clicked", G_CALLBACK(callback), NULL);
}

void Button::SetText(std::string text) {
    gtk_button_set_label(GTK_BUTTON(this->button), text.c_str());
}


void Button::SetCallback( void(*callback)() ) {
    
}


void Button::SetName(std::string name) {
    gtk_widget_set_name(this->button, name.c_str());
}
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


Button::Button(std::string text, void(*callback)(gpointer, GtkWidget*), void* data) {
    this->text = text;
    this->widget = gtk_button_new_with_label(text.c_str());
    g_signal_connect_swapped(this->widget, "clicked", G_CALLBACK(callback), data);
}

/**
 * Sets the text of the Button.
 */
void Button::SetText(std::string text) {
    gtk_button_set_label(GTK_BUTTON(this->widget), text.c_str());
}

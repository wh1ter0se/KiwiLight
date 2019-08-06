#include "UI.h"

/**
 * Source file for the TextBox class.
 * Written By: Brach Knutson
 */


using namespace KiwiLight;

/**
 * Creates a new textbox with the given text.
 */
TextBox::TextBox(std::string text) {
    this->textbox = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(this->textbox), text.c_str());
}

/**
 * Sets the textbox text to the given std::string.
 */
void TextBox::SetText(std::string text) {
    gtk_entry_set_text(GTK_ENTRY(this->textbox), text.c_str());
}

/**
 * Sets the alignment of the entry. 0=left, 1=right
 */
void TextBox::SetAlignment(double align) {
    gtk_entry_set_alignment(GTK_ENTRY(this->textbox), gfloat(align));
}

/**
 * Returns the text inside the textbox.
 */
std::string TextBox::GetText() {
    return gtk_entry_get_text(GTK_ENTRY(this->textbox));
}


void TextBox::SetName(std::string name) {
    gtk_widget_set_name(this->textbox, name.c_str());
}
#include "UI.h"

/**
 * Source file for the TextBox class.
 * Written By: Brach Knutson
 */

using namespace std;
using namespace KiwiLight;

/**
 * Creates a new textbox with the given text.
 */
TextBox::TextBox(string text) {
    this->textbox = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(this->textbox), text.c_str());
}

/**
 * Sets the textbox text to the given string.
 */
void TextBox::SetText(string text) {
    gtk_entry_set_text(GTK_ENTRY(this->textbox), text.c_str());
}

/**
 * Sets the editable property of the textbox to the passed boolean.
 */
void TextBox::SetEditable(bool edit) {
    gtk_entry_set_editable(GTK_ENTRY(this->textbox), gboolean(edit));
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
string TextBox::GetText() {
    return gtk_entry_get_text(GTK_ENTRY(this->textbox));
}
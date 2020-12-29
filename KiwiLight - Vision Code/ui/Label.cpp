#include "UI.h"

/**
 * Source file for the Label class.
 * Written By: Brach Knutson
 */


using namespace KiwiLight;

/**
 * Creates a new label.
 */
Label::Label(std::string text) {
    this->widget = gtk_label_new(text.c_str()); 
}

/**
 * Sets the text of the label to the given std::string.
 */
void Label::SetText(std::string text) {
    gtk_label_set_text(GTK_LABEL(this->widget), text.c_str());
    this->text = text;
}

/**
 * Enables/Disables line wrapping.
 */
void Label::SetLineWrap(bool enabled) {
    gtk_label_set_line_wrap(GTK_LABEL(this->widget), enabled);
}
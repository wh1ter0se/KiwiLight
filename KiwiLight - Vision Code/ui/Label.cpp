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

/**
 * Sets the justify of the Label.
 * DEPRECIATED: This method is no longer used by KiwiLight and will be removed in the next update.
 */
void Label::SetJustify(int justify) {
    GtkJustification justification;
    switch(justify) {
        case 0:
        justification = GTK_JUSTIFY_LEFT;
        break;

        case 1:
        justification = GTK_JUSTIFY_RIGHT;
        break;

        default:
        justification = GTK_JUSTIFY_CENTER;
    }

    gtk_label_set_justify(GTK_LABEL(this->widget), justification);
}
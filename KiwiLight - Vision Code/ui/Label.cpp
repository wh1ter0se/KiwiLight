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
    this->label = gtk_label_new(text.c_str());
}

/**
 * Sets the text of the label to the given std::string.
 */
void Label::SetText(std::string text) {
    gtk_label_set_text(GTK_LABEL(this->label), text.c_str());
    this->text = text;
}

void Label::SetFont(std::string font) {
    this->font = font;

    std::string pango_string = font + " " + std::to_string(this->font_size);
    gtk_widget_modify_font(this->label, 
        pango_font_description_from_string(pango_string.c_str())
    );
}

/**
 * Sets the font size of the label to the given size.
 */
void Label::SetFontSize(double size) {
    this->font_size = size;
    
    std::string pango_string = this->font + " " + std::to_string(size);
    gtk_widget_modify_font(this->label, 
        pango_font_description_from_string(pango_string.c_str())
    );
}
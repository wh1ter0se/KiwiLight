#include "UI.h"

/**
 * Source file for the UI Separator class.
 * Written By: Brach Knutson
 */


using namespace KiwiLight;

/**
 * Creates a new Separator.
 */
Separator::Separator(bool horizontal) {
    if(horizontal)
        this->separator = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
    else
        this->separator = gtk_separator_new(GTK_ORIENTATION_VERTICAL);
}

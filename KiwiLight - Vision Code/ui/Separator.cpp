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
        this->separator = gtk_hseparator_new();
    else
        this->separator = gtk_vseparator_new();
}
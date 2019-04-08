#include "UI.h"

/**
 * Source file for the Panel class.
 * Written By: Brach Knutson
 */

using namespace std;
using namespace KiwiLight;

/**
 * Creates a new panel.
 */
Panel::Panel(bool horizontal, int spacing) {
    if(horizontal)
        this->panel = gtk_hbox_new(FALSE, spacing);
    else
        this->panel = gtk_vbox_new(FALSE, spacing);    
}

/**
 * Packs the given widget into the start of the container.
 */
void Panel::Pack_start(Widget &widget, bool expand, bool fill, int pad) {
    gtk_box_pack_start(GTK_BOX(this->panel), widget.GetWidget(), gboolean(expand), gboolean(fill), pad);
}

/**
 * Packs the given widget into the end of the container.
 */
void Panel::Pack_end(Widget &widget, bool expand, bool fill, int pad) {
    gtk_box_pack_end(GTK_BOX(this->panel), widget.GetWidget(), gboolean(expand), gboolean(fill), pad);
}
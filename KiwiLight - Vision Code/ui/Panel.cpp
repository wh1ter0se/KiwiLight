#include "UI.h"

/**
 * Source file for the Panel class.
 * Written By: Brach Knutson
 */


using namespace KiwiLight;

/**
 * Creates a new panel.
 */
Panel::Panel(bool horizontal, int spacing) {
    //using depreciated gtk_hbox_new() until gtk_box_new() is implemented in current version of gtk.
    if(horizontal)
        this->panel = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, spacing);
    else
        this->panel = gtk_box_new(GTK_ORIENTATION_VERTICAL, spacing);
}

/**
 * Packs the given widget into the start of the container.
 */
void Panel::Pack_start(GtkWidget *widget, bool expand, bool fill, int pad) {
    gtk_box_pack_start(GTK_BOX(this->panel), widget, gboolean(expand), gboolean(fill), pad);
}

/**
 * Packs the given widget into the end of the container.
 */
void Panel::Pack_end(GtkWidget *widget, bool expand, bool fill, int pad) {
    gtk_box_pack_end(GTK_BOX(this->panel), widget, gboolean(expand), gboolean(fill), pad);
}
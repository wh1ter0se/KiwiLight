#include "UI.h"

/**
 * Source file for the Scrollable class.
 * Written By: Brach Knutson
 */

using namespace KiwiLight;


Scrollable::Scrollable(bool horizontal, bool vertical) {
    this->scrollable = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(this->scrollable), 
                                  (horizontal ? GTK_POLICY_AUTOMATIC : GTK_POLICY_NEVER),
                                  (vertical? GTK_POLICY_AUTOMATIC : GTK_POLICY_NEVER));

    gtk_container_set_border_width(GTK_CONTAINER(this->scrollable), 5);
}


void Scrollable::PackWidget(GtkWidget *wid) {
    gtk_container_add(GTK_CONTAINER(this->scrollable), wid);
}


void Scrollable::SetName(std::string name) {
    gtk_widget_set_name(this->scrollable, name.c_str());
}
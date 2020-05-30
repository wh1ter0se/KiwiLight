#include "UI.h"

/**
 * Source file for the Scrollable class.
 * Written By: Brach Knutson
 */

using namespace KiwiLight;

/**
 * Creates a new Scrollable.
 * @param horizontal Whether or not there should be a horizontal scrollbar.
 * @param vertical Whether or not there should be a vertical scrollbar.
 * DEPRECATED: The Scrollable class is not used and will be removed in the next update.
 */
Scrollable::Scrollable(bool horizontal, bool vertical) {
    this->widget = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(this->widget), 
                                  (horizontal ? GTK_POLICY_AUTOMATIC : GTK_POLICY_NEVER),
                                  (vertical? GTK_POLICY_AUTOMATIC : GTK_POLICY_NEVER));

    gtk_container_set_border_width(GTK_CONTAINER(this->widget), 5);
}

/**
 * Sets the widget being scrolled.
 * DEPRECATED: The Scrollable class will be removed in the next update.
 */
void Scrollable::PackWidget(GtkWidget *wid) {
    gtk_container_add(GTK_CONTAINER(this->widget), wid);
}

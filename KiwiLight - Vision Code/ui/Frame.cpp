#include "UI.h"

/**
 * Source file for the Frame class.
 * Written By: Brach Knutson
 */


using namespace KiwiLight;

/**
 * Creates a new frame with the given text in the label.
 * DEPRECATED: The Frame class is not used and will be removed in the next update.
 */
[[deprecated("The Frame class is not used and will be removed in the next update.")]]
Frame::Frame(std::string label) {
    this->widget = gtk_frame_new(label.c_str());
    g_object_ref_sink(this->widget);
    gtk_frame_set_shadow_type(GTK_FRAME(this->widget), GTK_SHADOW_ETCHED_IN);
}

/**
 * Pack the given widget into the frame (is it vertical or horizontal? idk!!!)
 * DEPRECATED: The Frame class will be removed in the next update.
 */
[[deprecated("The Frame class is not used and will be removed in the next update.")]]
void Frame::Pack(GtkWidget *widget) {
    gtk_container_add(GTK_CONTAINER(this->widget), widget);
}

/**
 * Removes the passed Widget.
 * DEPRECATED: The Frame class will be removed in the next update.
 */
[[deprecated("The Frame class is not used and will be removed in the next update.")]]
void Frame::Unpack(GtkWidget *widget) {
    gtk_container_remove(GTK_CONTAINER(this->widget), widget);
}
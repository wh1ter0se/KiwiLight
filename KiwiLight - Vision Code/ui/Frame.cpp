#include "UI.h"

/**
 * Source file for the Frame class.
 * Written By: Brach Knutson
 */


using namespace KiwiLight;

/**
 * Creates a new frame with the given text in the label.
 */
Frame::Frame(std::string label) {
    this->frame = gtk_frame_new(label.c_str());
    g_object_ref_sink(this->frame);
    gtk_frame_set_shadow_type(GTK_FRAME(this->frame), GTK_SHADOW_ETCHED_IN);
}

/**
 * Pack the given widget into the frame (is it vertical or horizontal? idk!!!)
 */
void Frame::Pack(GtkWidget *widget) {
    gtk_container_add(GTK_CONTAINER(this->frame), widget);
}


void Frame::Unpack(GtkWidget *widget) {
    gtk_container_remove(GTK_CONTAINER(this->frame), widget);
}


void Frame::SetName(std::string name) {
    gtk_widget_set_name(this->frame, name.c_str());
}
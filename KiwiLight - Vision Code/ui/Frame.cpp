#include "UI.h"

/**
 * Source file for the Frame class.
 * Written By: Brach Knutson
 */

using namespace std;
using namespace KiwiLight;

/**
 * Creates a new frame with the given text in the label.
 */
Frame::Frame(string label) {
    this->frame = gtk_frame_new(label.c_str());
    gtk_frame_set_shadow_type(GTK_FRAME(this->frame), GTK_SHADOW_ETCHED_IN);
}

/**
 * Pack the given widget into the frame (is it vertical or horizontal? idk!!!)
 */
void Frame::Pack(Widget &widget) {
    gtk_container_add(GTK_CONTAINER(this->frame), widget.GetWidget());
}
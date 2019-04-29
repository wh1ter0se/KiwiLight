#include "UI.h"

/**
 * Source file for the ImageFrame class.
 * Written By: Brach Knutson
 */

using namespace KiwiLight;


ImageFrame::ImageFrame() {
    // this->imgFrame = gtk_image_new();
}

/**
 * Creates a new ImageFrame displaying the given image.
 */
ImageFrame::ImageFrame(Image img) {
    this->imgFrame = gtk_image_new();
    gtk_image_set_from_pixbuf(GTK_IMAGE(this->imgFrame), img.ReturnImage());
}

/**
 * Draws the Image in the ImageFrame.
 */
void ImageFrame::Update(Image img) {
    gtk_image_set_from_pixbuf(GTK_IMAGE(this->imgFrame), img.ReturnImage());
}
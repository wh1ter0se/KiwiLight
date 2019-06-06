#include "UI.h"

/**
 * Source file for the ImageFrame class.
 * Written By: Brach Knutson
 */

using namespace KiwiLight;


ImageFrame::ImageFrame() {
    // this->imgFrame = gtk_image_new();
    this->constantWidth = 0;
    this->constantHeight = 0;
    this->hasConstantResolution = false;
    this->imageUpdated = false;
}

/**
 * Creates a new ImageFrame displaying the given image.
 */
ImageFrame::ImageFrame(Image img) {
    this->constantWidth = 0;
    this->constantHeight = 0;
    this->hasConstantResolution = false;
    this->imgFrame = gtk_image_new();
    gtk_image_set_from_pixbuf(GTK_IMAGE(this->imgFrame), img.ReturnImage());
    this->imageUpdated = true;
}


void ImageFrame::SetConstantResolution(int width, int height) {
    this->constantWidth = width;
    this->constantHeight = height;
    this->hasConstantResolution = true;

    //update for changes to take effect
    this->Update(Image());
}


void ImageFrame::SetHasConstantResolution(bool hasConstantResolution) {
    this->hasConstantResolution = hasConstantResolution;
}

/**
 * Draws the Image in the ImageFrame.
 */
void ImageFrame::Update(Image img) {
    if(this->imageUpdated) {
        gtk_image_clear(GTK_IMAGE(this->imgFrame));
    } else {
        this->imageUpdated = true;
    }

    if(this->hasConstantResolution) {
        img.Resize(this->constantWidth, this->constantHeight);
    }

    gtk_image_set_from_pixbuf(GTK_IMAGE(this->imgFrame), img.ReturnImage());
    gtk_widget_queue_draw(this->imgFrame);
}


void ImageFrame::SetName(std::string name) {
    gtk_widget_set_name(this->imgFrame, name.c_str());
}
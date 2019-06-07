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
    this->imgFrame = gtk_drawing_area_new();
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
void ImageFrame::Update(Image imag) {
    // if(this->imageUpdated) {
    //     gtk_image_clear(GTK_IMAGE(this->imgFrame));
    // } else {
    //     this->imageUpdated = true;
    // }

    // if(this->hasConstantResolution) {
    //     img.Resize(this->constantWidth, this->constantHeight);
    // }

    // gtk_image_set_from_pixbuf(GTK_IMAGE(this->imgFrame), img.ReturnImage());

    GdkWindow *win = gtk_widget_get_window(this->imgFrame);

    cairo_t *cairo = gdk_cairo_create(win);

    cairo_save(cairo);
    cairo_translate(cairo, 0, 0);

    cairo_surface_t *surface = gdk_cairo_surface_create_from_pixbuf(imag.ReturnImage(), 1, win);
    cairo_set_source_surface(cairo, surface, 0, 0);
    cairo_rectangle(cairo, 0, 0, imag.ReturnImageCVMat().cols, imag.ReturnImageCVMat().rows);
    cairo_paint(cairo);
    cairo_fill(cairo);
    cairo_restore(cairo);
}


void ImageFrame::SetName(std::string name) {
    gtk_widget_set_name(this->imgFrame, name.c_str());
}
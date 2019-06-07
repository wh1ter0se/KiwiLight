#include "UI.h"

/**
 * Source file for the Image class.
 * Written By: Brach Knutson
 */

using namespace cv;
using namespace KiwiLight;


Image::Image(std::string fileName) {
    this->image = gtk_image_new_from_file(fileName.c_str());
    this->colorspace = ImageColorspace::RGB;
    this->declaredAsStaticImage = true;
}


Image::Image(ImageColorspace colorspace) {
    this->image = gtk_drawing_area_new();
    this->colorspace = colorspace;
    this->declaredAsStaticImage = false;
}

/**
 * Updates and redraws the widget with newImage.
 */
void Image::Update(cv::Mat newImage) {
    if(!this->declaredAsStaticImage) {
        //convert the newImage to RGB colorspace
        cvtColor(newImage, newImage, COLOR_BGR2RGB);

        //create the cairo renderer and initalize it
        cairo_region_t *region = cairo_region_create();
        GdkWindow *win = gtk_widget_get_window(this->image);

        GdkDrawingContext *drawingContext = gdk_window_begin_draw_frame(win, region);
        cairo_t *cairo = gdk_drawing_context_get_cairo_context(drawingContext);

        cairo_save(cairo);
        cairo_translate(cairo, 0, 0);

        //create the pixbuf from the mat data
        guchar *imageData = (guchar*) newImage.data;
        GdkPixbuf *imgData = gdk_pixbuf_new_from_data(imageData,
                                                    GDK_COLORSPACE_RGB,
                                                    FALSE,
                                                    8,
                                                    newImage.cols,
                                                    newImage.rows,
                                                    newImage.step,
                                                    NULL,
                                                    NULL);

        //create a surface from the pixbuf and render it
        cairo_surface_t *surface = gdk_cairo_surface_create_from_pixbuf(imgData, 1, win);
        cairo_set_source_surface(cairo, surface, 0, 0);
        cairo_rectangle(cairo, 0, 0, newImage.cols, newImage.rows);
        cairo_paint(cairo);
        cairo_fill(cairo);
        cairo_restore(cairo);
        gdk_window_end_draw_frame(win, drawingContext);

        //request a new widget size so the entire image is visible
        gtk_widget_set_size_request(this->image, newImage.cols, newImage.rows);

        //clean up memory so leaks don't happen
        cairo_surface_destroy(surface);
        cairo_region_destroy(region);
        g_object_unref(G_OBJECT(imgData));
    } else {
        std::cout << "CANNOT UPDATE A STATIC IMAGE!" << std::endl;
    }
}

void Image::SetName(std::string name) {
    gtk_widget_set_name(this->image, name.c_str());
}
#include "UI.h"

/**
 * Source file for the Image class.
 * Written By: Brach Knutson
 */

using namespace KiwiLight;


Image::Image(std::string fileName) {
    this->img = gdk_pixbuf_new_from_file(fileName.c_str(), NULL);
}


Image::Image(cv::Mat img) {
    std::cout << "empty: " << img.empty() << std::endl;
    uchar *imgData = img.data;
    gboolean hasAlpha = FALSE;
    int imgWidth = img.cols;
    int imgHeight = img.rows;
    if(img.channels() == 4) {
        hasAlpha = TRUE;
    }

    this->img = gdk_pixbuf_new_from_data(imgData, 
                                         GDK_COLORSPACE_RGB, 
                                         FALSE,
                                         8,
                                         imgWidth,
                                         imgHeight,
                                         img.step,
                                         NULL,
                                         NULL);
}
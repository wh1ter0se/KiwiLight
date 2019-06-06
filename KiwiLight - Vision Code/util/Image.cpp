#include "Util.h"
#include "gtk-3.0/gtk/gtk.h"

/**
 * Source file for the Image class.
 * Written By: Brach Knutson
 */

using namespace cv;
using namespace KiwiLight;


Image::Image() {
    this->img = cv::Mat(Size(10, 10), CV_8U, Scalar(0,0,0));
}


Image::Image(std::string fileName) {
    this->img = cv::imread(fileName);

    //since we are imread()-ing we must convert the image to RGB
    this->img.convertTo(this->img, CV_16U);
    cv::cvtColor(this->img, this->img, cv::COLOR_BGR2RGB);
    this->img.convertTo(this->img, CV_8U);
}


Image::Image(cv::Mat image) {
    // image.convertTo(image, CV_16U);
    // cv::cvtColor(image, image, COLOR_BGR2RGB);
    // image.convertTo(this->img, CV_8U);

    image.copyTo(this->img);
    // this->img = image;
}


void Image::Resize(int width, int height) {
    try {
        cv::Mat resized(Size(width, height), CV_8UC3, Scalar(0,0,0));
        cv::resize(this->img, resized, cv::Size(width, height), 0, 0, INTER_BITS2);
        resized.convertTo(resized, CV_8U);
        this->img = resized;
    } catch(cv::Exception ex) {
    }
}


GdkPixbuf *Image::ReturnImage() {
    const guchar *imgData = (const guchar*) this->img.data;
    gboolean hasAlpha = FALSE;
    int imgWidth = this->img.cols;
    int imgHeight = this->img.rows;
    if(this->img.channels() == 4) {
        hasAlpha = TRUE;
    }

    this->pixbuf = gdk_pixbuf_new_from_data(imgData, 
                                            GDK_COLORSPACE_RGB, 
                                            FALSE,
                                            8,
                                            imgWidth,
                                            imgHeight,
                                            this->img.step,
                                            NULL,
                                            NULL);

    return this->pixbuf;
}
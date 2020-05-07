#include "Runner.h"

/**
 * Source file for the Contour class.
 * Written By: Brach Knutson
 */

using namespace cv;
using namespace KiwiLight;

/**
 * Creates a new contour object given the vector of points. 
 */
Contour::Contour(std::vector<cv::Point> points) {
    this->points = points;

    //find out the basic information of the contour
    cv::Rect boundingRect = cv::boundingRect(points);
    this->x = boundingRect.x;
    this->y = boundingRect.y;
    this->width = boundingRect.width;
    this->height = boundingRect.height;

    int centerX = (this->width / 2) + this->x;
    int centerY = (this->height / 2) + this->y;
    this->center = Point(centerX, centerY);

    cv::RotatedRect angleRect = cv::minAreaRect(points);
    this->angle = (int) angleRect.angle;
    this->angle = this->angle % 90;

    int trueArea = cv::contourArea(points);
    this->solidity = trueArea / (double) this->Area();
}
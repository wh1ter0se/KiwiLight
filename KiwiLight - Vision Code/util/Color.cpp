#include "Util.h"

/**
 * Source file for the Color class.
 * Written By: Brach Knutson
 */

using namespace cv;
using namespace KiwiLight;

/**
 * Creates a new color with the given values and errors.
 * NOTE: all color values are in the HSV colorspace (hue - saturation - value).
 */
Color::Color(int h, int s, int v, int hError, int sError, int vError) {
    this->h = h;
    this->s = s;
    this->v = v;
    this->hError = hError;
    this->sError = sError;
    this->vError = vError;
}

/**
 * Returns a representation of the color in a cv::Scalar for use in the inRange or whatever
 */
cv::Scalar Color::ReturnScalar() {
    return cv::Scalar(this->h, this->s, this->v);
}

/**
 * Returns a cv::Scalar representation of the upper bound of the color range.
 * (that is, all values have had their corrisponding error values added to them)
 */
cv::Scalar Color::GetUpperBound() {
    int newH = this->h + this->hError;
    int newS = this->s + this->sError;
    int newV = this->v + this->vError;
    return cv::Scalar(newH, newS, newV);
}

/**
 * Returns a cv::Scalar representation of the lower bound of the color range.
 * (all values had errors subtracted from them)
 */
cv::Scalar Color::GetLowerBound() {
    int newH = this->h - this->hError;
    int newS = this->s - this->sError;
    int newV = this->v - this->vError;
    return cv::Scalar(newH, newS, newV);
}
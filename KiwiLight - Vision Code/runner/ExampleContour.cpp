#include "Runner.h"

/**
 * Source file for the ExampleContour class.
 * Written By: Brach Knutson
 */

using namespace cv;
using namespace KiwiLight;

/**
 * Creates an ExampleContour to model a real contour using the given information.
 */
ExampleContour::ExampleContour(int id, SettingPair distX, SettingPair distY, SettingPair angle,SettingPair aspectRatio,SettingPair solidity,int minimumArea) {
    this->id = id;
    this->distX = distX;
    this->distY = distY;
    this->angle = angle;
    this->aspectRatio = aspectRatio;
    this->solidity = solidity;
    this->minimumArea = minimumArea;
}


bool ExampleContour::IsContour(Contour contour) {
    bool distanceTest = true;
    bool angleTest = false;
    bool arTest = false;
    bool solidTest = false;
    bool areaTest = false;

    //conduct all five tests and return result of all five
    // bool x = (contour.X() > this->distX.LowerBound()) && (contour.X() < this->distX.UpperBound());
    // bool y = (contour.Y() > this->distY.LowerBound()) && (contour.Y() < this->distY.UpperBound());
    // distanceTest = x && y;

    bool angleLower = (contour.Angle() > this->angle.LowerBound());
    bool angleUpper = (contour.Angle() < this->angle.UpperBound());
    angleTest = angleLower && angleUpper;

    bool arLower = (contour.AspectRatio() > this->aspectRatio.LowerBound());
    bool arUpper = (contour.AspectRatio() < this->aspectRatio.UpperBound());
    arTest = arLower && arUpper;

    bool solidLower = (contour.Solidity() > this->solidity.LowerBound());
    bool solidUpper = (contour.Solidity() < this->solidity.UpperBound());
    solidTest = solidLower && solidUpper;

    areaTest = (contour.Area() > this->minimumArea);

    return (distanceTest && angleTest && arTest && solidTest && areaTest);
}
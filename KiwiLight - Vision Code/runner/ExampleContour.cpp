#include "Runner.h"

/**
 * Source file for the ExampleContour class.
 * Written By: Brach Knutson
 */

using namespace cv;
using namespace KiwiLight;

/**
 * Creates an instance of the default ExampleContour, a square.
 */
ExampleContour::ExampleContour(int id) {
    this->id = id;
    this->distX = SettingPair(0, 0.25);
    this->distY = SettingPair(0, 0.25);
    this->angle = SettingPair(0, 90);
    this->aspectRatio = SettingPair(1, 0.25);
    this->solidity = SettingPair(1, 0.125);
    this->minimumArea = 500;
}

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
    bool angleTest = false;
    bool arTest = false;
    bool solidTest = false;
    bool areaTest = false;

    //conduct all five tests and return result of all five

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

    return (angleTest && arTest && solidTest && areaTest);
}

// void SetDistX(SettingPair distX);
// void SetDistY(SettingPair distY);
// void SetAngle(SettingPair angle);
// void SetAspectRatio(SettingPair aspectRatio);
// void SetSolidity(SettingPair solidity);
// void SetMinimumArea(int minimumArea);


void ExampleContour::SetDistX(SettingPair distX) {
    this->distX = distX;
}


void ExampleContour::SetDistY(SettingPair distY) {
    this->distY = distY;
}


void ExampleContour::SetAngle(SettingPair angle) {
    this->angle = angle;
}


void ExampleContour::SetAspectRatio(SettingPair aspectRatio) {
    this->aspectRatio = aspectRatio;
}


void ExampleContour::SetSolidity(SettingPair solidity) {
    this->solidity = solidity;
}


void ExampleContour::SetMinimumArea(int minimumArea) {
    this->minimumArea = minimumArea;
}
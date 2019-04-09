#include "KiwiLight.h"

/**
 * Source file for Camera class.
 * Written By: Brach Knutson
 */


using namespace cv;
using namespace KiwiLight;

/**
 * Creates a new Camera using the device at the given index.
 */
Camera::Camera(int index) {
    this->stream = cv::VideoCapture(index);
    this->index = index;
    this->opened = true;
    iteration = 0;

}

/**
 * Updates the video stream shown on the screen. 
 */
void Camera::Update() {
    if(opened) {
        if(this->stream.isOpened()) {
            cv::Mat img;
            bool success = stream.read(img);
            if(success) {
                //show image in window on screen
                std::string winName = "Camera Device " + std::to_string(index);
                cv::imshow(winName.c_str(), img);
                cv::waitKey(5);
            }
        } else {
            if(iteration >= 10) {
                this->stream.release();
                this->stream = cv::VideoCapture(index);
                iteration = 0;
            } else {
                iteration++;
            }
        }
    }

    //check to see if we need to close the camera for settings to be applied
    if(Flags::GetFlag("GetReadyToApplySettings")) {
        this->stream.release();
        cv::destroyAllWindows();
        this->opened = false;
        Flags::LowerFlag("GetReadyToApplySettings");
        Flags::RaiseFlag("ApplySettings");
    }

    if(Flags::GetFlag("StartCamera")) {
        Flags::LowerFlag("StartCamera");
        this->stream = cv::VideoCapture(this->index);
        this->opened = true;
    }
}
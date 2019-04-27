#include "Util.h"

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
    std::cout << "c1" << std::endl;
    std::cout.flush();
    this->stream = cv::VideoCapture(index);
    this->index = index;
    this->opened = true;
    this->streamingSuccessful = false;
    this->frameWidth = 600;
    this->frameHeight = 400;
    this->iteration = 0;
    std::cout << "c2" << std::endl;
    std::cout.flush();

}


cv::Mat Camera::GetImage() {
    cv::Mat img;
    bool success = this->stream.read(img);
    return img;
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
                // //show image in window on screen
                // std::string winName = "Camera Device " + std::to_string(index);
                // cv::imshow(winName.c_str(), img);
                // cv::waitKey(5);

                streamingSuccessful = true;
            }
        } else {
            streamingSuccessful = false;
            if(iteration >= 10) {
                Close();
                Open();
                iteration = 0;
            } else {
                iteration++;
            }
        } 
    }

    if(Flags::GetFlag("CloseCamera")) {
        Flags::LowerFlag("CloseCamera");
        Close();
        Flags::RaiseFlag("CamClosed");
    }

    if(Flags::GetFlag("StartCamera")) {
        Flags::LowerFlag("StartCamera");
        Open();
        Flags::RaiseFlag("CamOpened");
    }
}

/**
 * Closes the current camera, and opens the one on the given index.
 */
void Camera::SetIndex(int index) {
    this->opened = false;
    this->stream.release();
    cv::destroyAllWindows();

    this->index = index;
    this->stream = cv::VideoCapture(index);
    this->opened = true;

}

/**
 * Uses OPENCV to set the width of the camera stream.
 */
void Camera::SetWidth(int width) {
    this->frameWidth = width;
}

/**
 * Uses OPENCV to set the height of the camera stream.
 */
void Camera::SetHeight(int height) {
    this->frameHeight = height;
}

/**
 * Closes and frees the camera stream. The camera object will still be active.
 */
void Camera::Close() {
    if(this->opened) {
        this->stream.release();
        // cv::destroyAllWindows();
        this->opened = false;
    }
}

/**
 * Reopens the camera stream after closure
 */
void Camera::Open() {
    if(!this->opened) {
        this->stream = cv::VideoCapture(this->index);

        std::cout << "setting (w, h): " << this->frameWidth << ", " << this->frameHeight << std::endl;

        this->stream.set(cv::CAP_PROP_FRAME_WIDTH, this->frameWidth);
        this->stream.set(cv::CAP_PROP_FRAME_HEIGHT, this->frameHeight);

        std::cout << "stream w: " << this->stream.get(cv::CAP_PROP_FRAME_WIDTH) << std::endl;
        std::cout << "stream h: " << this->stream.get(cv::CAP_PROP_FRAME_HEIGHT) << std::endl;
        std::cout.flush();
        this->opened = true;
    }
}
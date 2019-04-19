#include "Runner.h"

/**
 * Source file for the PreProcessor class.
 * Written By: Brach Knutson
 */

using namespace cv;
using namespace KiwiLight;

/**
 * Creates a new PreProcessor with the given settings. 
 * PreProcessor will pick out the settings it will use, so just give it all of them.
 */
PreProcessor::PreProcessor(ConfigurationSettingsList settings, bool fullPreprocessor) {
    this->isFullPreprocessor = fullPreprocessor;

    try {
        this->threshold = std::stod(settings.GetSetting("thresholdValue"));
        this->threshValue = std::stod(settings.GetSetting("threshMaxValue"));
        this->threshtype = std::stoi(settings.GetSetting("threshType"));

        this->dilate = std::stoi(settings.GetSetting("dilation"));

        int colorH = std::stoi(settings.GetSetting("colorH"));
        int colorS = std::stoi(settings.GetSetting("colorS"));
        int colorV = std::stoi(settings.GetSetting("colorV"));
        int colorHError = std::stoi(settings.GetSetting("colorH_error"));
        int colorSError = std::stoi(settings.GetSetting("colorS_error"));
        int colorVError = std::stoi(settings.GetSetting("colorV_error"));

        this->targetColor = Color(colorH, colorS, colorV, colorHError, colorSError, colorVError);
    } catch(std::invalid_argument ex) {
        std::cout << "WARNING: FORMATTING ERROR FOR PREPROCESSOR CONFIGURATION; REVERTING TO PARTIAL PREPROCESSOR" << std::endl;
        this->isFullPreprocessor = false;
    }
}

/**
 * Takes the given generic image and makes it usable for the PostProcessor.
 */
cv::Mat PreProcessor::ProcessImage(cv::Mat img) {
    cv::Mat out;

    if(this->isFullPreprocessor) {
        cv::threshold(img, img, this->threshold, this->threshValue, this->threshtype);
        cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, 
                                                cv::Size(this->dilate,this->dilate));
        cv::dilate(img, img, kernel);
        cv::cvtColor(img, img, cv::COLOR_BGR2HSV);
        cv::inRange(img, this->targetColor.GetLowerBound(), this->targetColor.GetUpperBound(), out);
    } else {
        cv::cvtColor(img, out, cv::COLOR_BGR2GRAY);
    }

    return out;
}
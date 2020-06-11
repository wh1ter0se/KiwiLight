#include "Runner.h"

/**
 * Source file for the PreProcessor class.
 * Written By: Brach Knutson
 */

using namespace cv;
using namespace KiwiLight;

/**
 * Creates a new PreProcessor.
 * @param fullPreprocessor True if the preprocessor should preprocess raw images, false if the image is already preprocessed.
 * @param targetColor The color of the objects.
 * @param threshold The amount of light required to highlight objects.
 * @param erosion The amount of erosion to apply to an image (to reduce noise)
 * @param dilation The amount of dilation to apply to an image (to make objects more solid)
 * @param debug True if the preprocessor is in debug mode, false otherwise.
 */
PreProcessor::PreProcessor(bool fullPreprocessor, Color targetColor, int threshold, int erosion, int dilation, bool debug) {
    this->isFullPreprocessor = fullPreprocessor;
    this->debugging = debug;
    this->targetColor = targetColor;
    this->threshold = threshold;
    this->threshValue = 255;
    this->threshtype = 0;
    this->erode = erosion;
    this->dilate = dilation;
}

/**
 * Takes the given generic image and makes it usable for the PostProcessor.
 */
cv::Mat PreProcessor::ProcessImage(cv::Mat img) {
    cv::Mat out;

    if(this->isFullPreprocessor) {
        cv::threshold(img, img, this->threshold, this->threshValue, this->threshtype);
        cv::Mat dilateKernel = cv::getStructuringElement(cv::MORPH_RECT, 
                                                cv::Size(this->dilate, this->dilate));
                                                
        cv::Mat erodeKernel = cv::getStructuringElement(cv::MORPH_RECT,
                                                        cv::Size(this->erode, this->erode));
        
        cv::erode(img, img, erodeKernel);
        cv::dilate(img, img, dilateKernel);
        cv::cvtColor(img, img, cv::COLOR_BGR2HSV);
        cv::inRange(img, this->targetColor.GetLowerBound(), this->targetColor.GetUpperBound(), out);
    } else {
        cv::cvtColor(img, out, cv::COLOR_BGR2GRAY);
    }

    return out;
}

/**
 * Sets a property of the preprocessor.
 * @param prop The property to set.
 * @param value The value to set the property to.
 */
void PreProcessor::SetProperty(PreProcessorProperty prop, double value) {
    switch(prop) {
        case PreProcessorProperty::IS_FULL:
            this->isFullPreprocessor = (value == 1 ? true : false);
            break;
        case PreProcessorProperty::THRESHOLD:
            this->threshold = value;
            break;
        case PreProcessorProperty::EROSION:
            this->erode = value;
            break;
        case PreProcessorProperty::DILATION:
            this->dilate = value;
            break;
        case PreProcessorProperty::COLOR_HUE:
            {
            Color newColor = Color((int) value, this->targetColor.GetS(), this->targetColor.GetV(), this->targetColor.GetHError(), this->targetColor.GetSError(), this->targetColor.GetVError());
            this->targetColor = newColor;
            break;
            }
        case PreProcessorProperty::COLOR_SATURATION:
            {
            Color newColor = Color(this->targetColor.GetH(), (int) value, this->targetColor.GetV(), this->targetColor.GetHError(), this->targetColor. GetSError(), this->targetColor.GetVError());
            this->targetColor = newColor;
            break;
            }
        case PreProcessorProperty::COLOR_VALUE:
            {
            Color newColor = Color(this->targetColor.GetH(), this->targetColor.GetS(), (int) value, this->targetColor.GetHError(), this->targetColor.GetSError(), this->targetColor.GetVError());
            this->targetColor = newColor;
            break;
            }

        case PreProcessorProperty::COLOR_ERROR:
            {
            Color newColor = Color(this->targetColor.GetH(), this->targetColor.GetS(), this->targetColor.GetV(), value, value, value);
            this->targetColor = newColor;
            break;
            }
    }
}

/**
 * Reads a property from the PreProcessor.
 * @param prop The property to read.
 * @return The value of the property.
 */
double PreProcessor::GetProperty(PreProcessorProperty prop) {
    double finalValue = -1.0;

    switch(prop) {
        case PreProcessorProperty::IS_FULL:
            finalValue = (this->isFullPreprocessor ? 1 : 0);
            break;
        case PreProcessorProperty::THRESHOLD:
            finalValue = this->threshold;
            break;
        case PreProcessorProperty::EROSION:
            finalValue = this->erode;
            break;
        case PreProcessorProperty::DILATION:
            finalValue = this->dilate;
            break;
        case PreProcessorProperty::COLOR_HUE:
            finalValue = this->targetColor.GetH();
            break;
        case PreProcessorProperty::COLOR_SATURATION:
            finalValue = this->targetColor.GetS();
            break;
        case PreProcessorProperty::COLOR_VALUE:
            finalValue = this->targetColor.GetV();
            break;
        case PreProcessorProperty::COLOR_ERROR:
            finalValue = this->targetColor.GetHError();
            break;
        default:
            std::cout << "PROPERTY NOT FOUND" << std::endl;
            break;
    }

    return finalValue;
}

/**
 * Returns a string summary of this PreProcessor.
 */
std::string PreProcessor::toString() {
    std::string endline = "\n";
    std::string thisString = "";
    thisString += std::string("Debugging: ") + std::string((this->debugging ? "YES" : "NO")) + endline;

    if(this->isFullPreprocessor) {
                      std::string("Type: FULL\n") +
                      std::string("Threshold Value: ") + std::to_string(this->threshold) + endline +
                      std::string("Dilation: ") + std::to_string(this->dilate) + endline + 
                      std::string("Color H: ") + std::to_string(this->targetColor.GetH()) + endline + 
                      std::string("Color S: ") + std::to_string(this->targetColor.GetS()) + endline + 
                      std::string("Color V: ") + std::to_string(this->targetColor.GetV()) + endline +
                      std::string("Color Error: ") + std::to_string(this->targetColor.GetHError()) + endline;
    } else {
        thisString += std::string("Type: PARTIAL") + endline;
    }

    return thisString;
}

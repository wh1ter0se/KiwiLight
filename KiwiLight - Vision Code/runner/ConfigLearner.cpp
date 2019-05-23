#include "Runner.h"

/**
 * Source file for the ConfigLearner class.
 * Written By: Brach Knutson
 */

using namespace cv;
using namespace KiwiLight;

/**
 * Creates new instance of a ConfigLearner with the given preprocessor.
 */
ConfigLearner::ConfigLearner(XMLTag preprocessor, cv::VideoCapture stream) {
    this->stream = stream;
    this->configsettings = ConfigurationSettingsList();
    this->configsettings.AddSetting("PreprocessorType", preprocessor.GetAttributesByName("type")[0].Value());
    XMLTag threshold = preprocessor.GetTagsByName("targetThreshold")[0];
        this->configsettings.AddSetting("thresholdValue", threshold.GetTagsByName("threshold")[0].Content());
        this->configsettings.AddSetting("threshMaxValue", threshold.GetTagsByName("maxValue")[0].Content());
        this->configsettings.AddSetting("threshType", threshold.GetTagsByName("type")[0].Content());

    this->configsettings.AddSetting("dilation", preprocessor.GetTagsByName("dilation")[0].Content());
    XMLTag color = preprocessor.GetTagsByName("targetColor")[0];
        this->configsettings.AddSetting("colorH", color.GetTagsByName("h")[0].Content());
        this->configsettings.AddSetting("colorS", color.GetTagsByName("s")[0].Content());
        this->configsettings.AddSetting("colorV", color.GetTagsByName("v")[0].Content());
        this->configsettings.AddSetting("colorH_error", color.GetTagsByName("h")[0].GetAttributesByName("error")[0].Value());
        this->configsettings.AddSetting("colorS_error", color.GetTagsByName("s")[0].GetAttributesByName("error")[0].Value());
        this->configsettings.AddSetting("colorV_error", color.GetTagsByName("v")[0].GetAttributesByName("error")[0].Value());

    bool isFullPreprocessor = (preprocessor.GetAttributesByName("type")[0].Value() == "full" ? true : false);
    this->preprocessor = PreProcessor(this->configsettings, isFullPreprocessor);
}

/**
 * Learns the target and returns the XMLTag representing it.
 * Note that some distance information will need to be added
 */
XMLTag ConfigLearner::LearnTarget() {
    this->stream.read(this->out);
}

/**
 * Updates the output image
 */
void ConfigLearner::Update(int minArea) {
    Mat img;
    bool success = this->stream.read(img);
    if(success) {
        Mat preprocessed = this->preprocessor.ProcessImage(img);
        preprocessed.copyTo(this->out);

        cv::cvtColor(this->out, this->out, cv::COLOR_GRAY2BGR);
        
        //find contours and highlight all contours that are bigger than a certain area
        std::vector <std::vector <Point> > contours; //all contours in image
        std::vector <std::vector <Point> > bigContours; // contours that are bigger than minArea
        findContours(preprocessed, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

        //find all contours that are bigger than minimumArea, then highlight them.
        for(int i=0; i<contours.size(); i++) {
            std::vector<Point> contour = contours[i];

            Rect bounds = boundingRect(contour);
            int area = bounds.width * bounds.height;

            if(area > minArea) {
                bigContours.push_back(contour);
            }
        }

        //draw rectangle around the big rects
        if(bigContours.size() > 0) {
            int smallestX = 5000,
                smallestY = 5000,
                biggestX = -5000,
                biggestY = -5000,
                width = 0,
                height = 0;

            for(int i=0; i<bigContours.size(); i++) {
                Rect bounds = boundingRect(bigContours[i]);

                if(bounds.x < smallestX) {
                    smallestX = bounds.x;
                } 
                if(bounds.x > biggestX) {
                    biggestX = bounds.x;
                    width = bounds.width;
                }

                if(bounds.y < smallestY) {
                    smallestY = bounds.y;
                } 
                if(bounds.y > biggestY) {
                    biggestY = bounds.y;
                    height = bounds.height;
                }
            }

            int trueWidth = (biggestX - smallestX) + width;
            int trueHeight = (biggestY - smallestY) + height;
            
            int centerX = (width / 2) + smallestX;
            int centerY = (height / 2) + smallestY;
            
            Point centerPoint = Point(centerX, centerY);
            Rect bigRect = Rect(smallestX, smallestY, trueWidth, trueHeight);

            rectangle(this->out, bigRect, Scalar(255,0,0), 3);
            circle(this->out, centerPoint, 3, Scalar(0,255,0), 4);
        }
    } else {
        std::cout << "COULD NOT GRAB FROM CAMERA!! ";
    }
}

/**
 * Stops the config learner and releases the video stream
 */
void ConfigLearner::Stop() {

}
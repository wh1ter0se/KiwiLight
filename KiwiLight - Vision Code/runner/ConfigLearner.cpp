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
    std::cout << "cl1a" << std::endl;
    this->stream = stream;
    std::cout << "cl1b" << std::endl;
    this->configsettings.AddSetting("PreprocessorType", preprocessor.GetAttributesByName("type")[0].Value());
    XMLTag threshold = preprocessor.GetTagsByName("targetThreshold")[0];
        this->configsettings.AddSetting("thresholdValue", threshold.GetTagsByName("threshold")[0].Content());
        this->configsettings.AddSetting("threshMaxValue", threshold.GetTagsByName("maxValue")[0].Content());
        this->configsettings.AddSetting("threshType", threshold.GetTagsByName("type")[0].Content());

    std::cout << "cl1c" << std::endl;
    this->configsettings.AddSetting("dilation", preprocessor.GetTagsByName("dilation")[0].Content());
    XMLTag color = preprocessor.GetTagsByName("targetColor")[0];
        this->configsettings.AddSetting("colorH", color.GetTagsByName("h")[0].Content());
        this->configsettings.AddSetting("colorS", color.GetTagsByName("s")[0].Content());
        this->configsettings.AddSetting("colorV", color.GetTagsByName("v")[0].Content());
        this->configsettings.AddSetting("colorH_error", color.GetTagsByName("h")[0].GetAttributesByName("error")[0].Value());
        this->configsettings.AddSetting("colorS_error", color.GetTagsByName("s")[0].GetAttributesByName("error")[0].Value());
        this->configsettings.AddSetting("colorV_error", color.GetTagsByName("v")[0].GetAttributesByName("error")[0].Value());

    std::cout << "cl1d" << std::endl;
    bool isFullPreprocessor = (preprocessor.GetAttributesByName("type")[0].Value() == "full" ? true : false);
    std::cout << "cl1e" << std::endl;
    this->preprocessor = PreProcessor(this->configsettings, true);
    std::cout << "cl2" << std::endl;
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
void ConfigLearner::Update() {
    this->stream.read(this->out);
}

/**
 * Stops the config learner and releases the video stream
 */
void ConfigLearner::Stop() {

}
#include "Runner.h"

/**
 * Source file for the Runner class.
 * Written By: Brach Knutson
 */

using namespace cv;
using namespace KiwiLight;

/**
 * Creates a new PostProcessor with the given settings. 
 * Just give it all the runner settings, PostProcessor will pick out the ones it will use.
 */
PostProcessor::PostProcessor(ConfigurationSettingsList settings) {

}

/**
 * Processes the given image (from the preprocessor) and returns a vector containing any 
 * targets it finds.
 */
std::vector<Target> ProcessImage(cv::Mat img) {

}
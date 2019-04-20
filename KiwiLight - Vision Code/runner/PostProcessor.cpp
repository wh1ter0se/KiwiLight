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
PostProcessor::PostProcessor(std::vector<ExampleTarget> targets) {
    this->targets = targets;
    ExampleTarget targ = this->targets[0];
}

/**
 * Processes the given image (from the preprocessor) and returns a vector containing any 
 * targets it finds.
 */
std::vector<Target> PostProcessor::ProcessImage(cv::Mat img) {
    std::vector<Target> foundTargets = std::vector<Target>();

    //find contours with input image
    std::vector< std::vector< Point > > contours;
    cv::findContours(img, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    //create contour wrappers and prepare to compare with targets
    std::vector<Contour> objects = std::vector<Contour>();
    for(int i=0; i<contours.size(); i++) {
        Contour newContour = Contour(contours[i]);
        objects.push_back(newContour);
    }

    for(int k=0; k<this->targets.size(); k++) {
        std::vector<Target> targs = this->targets[k].GetTargets(objects);
        //add results to our found targets
        for(int a=0; a<targs.size(); a++) {
            foundTargets.push_back(targs[a]);
        }
    }

    return foundTargets;
}
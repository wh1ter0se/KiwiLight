#include "Runner.h"

/**
 * Source file for the TargetDistanceLearner class.
 * Written By: Brach Knutson
 */

using namespace cv;
using namespace KiwiLight;

/**
 * Creates a new TargetDistanceLearner using the given PreProcessor and PostProcessor.
 */
TargetDistanceLearner::TargetDistanceLearner(PreProcessor preprocessor, PostProcessor postprocessor) {
    this->preprocessor = preprocessor;
    this->postprocessor = postprocessor;
    this->frames = 0;
}

/**
 * "Feeds" a raw image to the TargetDistanceLearner
 * @param image The unprocessed image that will be fed to the learner.
 */
void TargetDistanceLearner::FeedImage(Mat image) {
    if(image.empty()) {
        failedFrames++;
        return;
    }
    
    Mat img(image);
    img = this->preprocessor.ProcessImage(img);
    std::vector<Target> frameTargs = this->postprocessor.ProcessImage(img);
    if(frameTargs.size() == 1) {
        Target targ = frameTargs[0];
        this->targetWidths.push_back((double) targ.Bounds().width);
        this->frames++;
    }
}

/**
 * Feeds a target to the TargetDistanceLearner.
 * @param targ The Target to feed to the learner.
 */
void TargetDistanceLearner::FeedTarget(Target targ) {
    this->targetWidths.push_back((double) targ.Bounds().width);
    this->frames++;
}

/**
 * Feeds the distance learner nothing.
 */
void TargetDistanceLearner::FeedBlank() {
    this->frames++;
}

/**
 * Calculates the focal width required to achieve the distance value of the fed targets.
 * @param trueDistance The real distance from the camera to the target
 * @param trueWidth The width of the target.
 * NOTE: trueDistance and trueWidth MUST be in the same unit (no mixing inches and centimeters)!
 * The unit that these numbers are reported in will be the unit that distance is reported in!
 */
double TargetDistanceLearner::GetFocalWidth(double trueDistance, double trueWidth) {
    int sizeBeforeRemove = targetWidths.size();
    double focalWidth = -1;
    if(sizeBeforeRemove > 15) {
        this->targetWidths = Util::SortLeastGreatestDouble(this->targetWidths);
        this->targetWidths = Util::RemoveOutliers(this->targetWidths, 30);
        
        double avgWidth = Util::Average(targetWidths);
        
        focalWidth = avgWidth * trueDistance / trueWidth;
        this->targetWidths = std::vector<double>(); //reset the vector so it can learn again
    } else {
        std::cout << "Not enough frames with targets! Aborting." << std::endl;
    }

    this->frames = 0; //reset the frames counter so it can learn again
    return focalWidth;
}

/**
 * Returns the number of frames/targets that were "fed" to the TargetDistanceLearner.
 */
int TargetDistanceLearner::GetFramesLearned() {
    return this->frames;
}

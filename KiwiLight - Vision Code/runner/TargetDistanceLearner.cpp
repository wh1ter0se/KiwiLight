#include "Runner.h"

/**
 * Source file for the TargetDistanceLearner class.
 * Written By: Brach Knutson
 */

using namespace cv;
using namespace KiwiLight;

static const int NUMBER_OF_FRAMES_TO_LEARN = 20;


TargetDistanceLearner::TargetDistanceLearner(PreProcessor preprocessor, PostProcessor postprocessor, VideoCapture cap, Size constantSz) {
    this->preprocessor = preprocessor;
    this->postprocessor = postprocessor;
    this->cap = cap;
    this->constantResize = constantSz;
    this->outString = "Collecting and processing images (0%)";
}


double TargetDistanceLearner::LearnFocalWidth(double trueWidth, double trueDistance) {
    //capture the number of frames we will use to learn
    std::vector<Target> targetsToUse;
    std::vector<double> targetWidths;

    std::cout << "Learning focal width..." << std::endl;

    for(int i=0; i<NUMBER_OF_FRAMES_TO_LEARN; i++) {
        Mat img;
        bool success = this->cap.read(img);

        if(success) {
            resize(img, img, this->constantResize);
            img = this->preprocessor.ProcessImage(img);
            std::vector<Target> frameTargs = this->postprocessor.ProcessImage(img);

            if(frameTargs.size() == 1) {
                //we only want one so we know which target to focus on
                Target newTarg = frameTargs[0];
                targetWidths.push_back(newTarg.Bounds().width);
                targetsToUse.push_back(newTarg);
            }

            double percentComplete = ((double) i / (double) NUMBER_OF_FRAMES_TO_LEARN) * 90;
            this->outString = "Collecting and processing images (" + std::to_string((int) percentComplete) + "%)";
        }
    }

    this->outString = "Sorting data and calculating constants (90%)";

    std::cout << "Successfully captured " << targetsToUse.size() << "/" << NUMBER_OF_FRAMES_TO_LEARN << " Targets." << std::endl;

    int sizeBeforeRemove = targetWidths.size();
    targetWidths = DataUtils::SortLeastGreatestDouble(targetWidths);
    targetWidths = DataUtils::RemoveOutliers(targetWidths, 30);

    std::cout << "Removed " << (sizeBeforeRemove - targetWidths.size()) << " outliers." << std::endl;

    double avgWidth = DataUtils::Average(targetWidths);
    
    std::cout << "avg width: " << avgWidth << std::endl;

    double focalWidth = avgWidth * trueDistance / trueWidth;
    std::cout << "Focal width: " << focalWidth << std::endl;
    return focalWidth;
}
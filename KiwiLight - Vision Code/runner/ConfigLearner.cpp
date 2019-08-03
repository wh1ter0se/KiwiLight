#include "Runner.h"

/**
 * Source file for the ConfigLearner class.
 * Written By: Brach Knutson
 */

using namespace cv;
using namespace KiwiLight;


//The number of frames to capture when learning a target
static int NUMBER_OF_FRAMES_TO_LEARN = 50;

/**
 * Creates new instance of a ConfigLearner with the given preprocessor.
 */
ConfigLearner::ConfigLearner(PreProcessor preprocessor, cv::VideoCapture stream) {
    this->constantResize = Size(stream.get(cv::CAP_PROP_FRAME_WIDTH), stream.get(cv::CAP_PROP_FRAME_HEIGHT));
    this->stream = stream;
    this->preprocessor = preprocessor;
}

/**
 * Learns the target and returns the XMLTag representing it.
 * Note that some distance information will need to be added
 * @return An ExampleTarget that describes the target that was learned.
 */
ExampleTarget ConfigLearner::LearnTarget(int minArea) {
    //define vector where the camera frames will be stored
    this->outString = "Collecting and processing images (0%)";
    std::vector<CameraFrame> frames = std::vector<CameraFrame>();

    //capture the images that will be used to learn the target
    for(int i=0; i<NUMBER_OF_FRAMES_TO_LEARN; i++) {
        Mat img;
        bool success = false;
        if(RunnerSettings::USE_CAMERA) {
            success = this->stream.read(img);
        } else {
            success = true;
            img = imread(RunnerSettings::IMAGE_TO_USE);
        }

        if(success) {
            resize(img, img, this->constantResize);
            img = this->preprocessor.ProcessImage(img);
            CameraFrame newFrame = CameraFrame(img, minArea);
            frames.push_back(newFrame);
        }

        //update the progress string (collecting images will be 90% of the work)
        framesCollected = i;
        double percentComplete = ((double) i / NUMBER_OF_FRAMES_TO_LEARN) * 90.0;
        this->outString = "Collecting and processing images (" + std::to_string((int) percentComplete) + "%)";
    }

    this->outString = "Preparing to analyze contour data (95%)";

    int framesCaptured = frames.size();

    //create a sorted list of the number of contours in each image (it is double because DataUtils sorts doubles)
    std::vector<double> numberContoursList;
    std::vector <std::vector <Contour> > groupedContours; //vector of contours grouped by their distance from target center
    for(int i=0; i<frames.size(); i++) {
        groupedContours.push_back(frames[i].GetContoursGrouped());
        numberContoursList.push_back((double) frames[i].NumberOfContours());
    }

    double regularNumberOfContours = DataUtils::MostCommonValue(numberContoursList);

    //discard any frame that does not have the same contours as regularNumberOfContours
    for(int i=0; i<frames.size(); i++) {
        if(frames[i].NumberOfContours() != regularNumberOfContours) {
            frames.erase(frames.begin() + i);
            i--;
        }
    }

    //go through groupedContours to get and format data.
    std::vector<ExampleContour> finishedContours;
    for(int i=0; i<regularNumberOfContours; i++) {
        std::vector<double> horizontalDistances = std::vector<double>();
        std::vector<double> verticalDistances   = std::vector<double>();
        std::vector<double> angles              = std::vector<double>();
        std::vector<double> solidities          = std::vector<double>();
        std::vector<double> aspectRatios        = std::vector<double>();

        for(int k=0; k<groupedContours.size(); k++) {
            //gather and format contour data

            Contour contourToAnalyze = groupedContours[k][i];
            Distance contourDistToCenter = frames[k].GetContourDistance(contourToAnalyze);

            horizontalDistances.push_back(contourDistToCenter.x);
            verticalDistances.push_back(contourDistToCenter.y);
            angles.push_back(contourToAnalyze.Angle());
            solidities.push_back(contourToAnalyze.Solidity());
            aspectRatios.push_back(contourToAnalyze.AspectRatio());
        }

        horizontalDistances = DataUtils::SortLeastGreatestDouble(horizontalDistances);
        verticalDistances   = DataUtils::SortLeastGreatestDouble(verticalDistances);
        angles              = DataUtils::SortLeastGreatestDouble(angles);
        solidities          = DataUtils::SortLeastGreatestDouble(solidities);
        aspectRatios        = DataUtils::SortLeastGreatestDouble(aspectRatios);

        //record the sizes of each array so we can see how many outliers are removed
        std::vector<double> dataSizes;
        dataSizes.push_back((double) horizontalDistances.size());
        dataSizes.push_back((double) verticalDistances.size());
        dataSizes.push_back((double) angles.size());
        dataSizes.push_back((double) solidities.size());
        dataSizes.push_back((double) aspectRatios.size());

        //remove outliers
        horizontalDistances = DataUtils::RemoveOutliers(horizontalDistances, 0.5);
        verticalDistances   = DataUtils::RemoveOutliers(verticalDistances, 0.5);
        angles              = DataUtils::RemoveOutliers(angles, 10.0);
        solidities          = DataUtils::RemoveOutliers(solidities, 0.25);
        aspectRatios        = DataUtils::RemoveOutliers(aspectRatios, 0.25);

        std::vector<double> newDataSizes;
        newDataSizes.push_back((double) horizontalDistances.size());
        newDataSizes.push_back((double) verticalDistances.size());
        newDataSizes.push_back((double) angles.size());
        newDataSizes.push_back((double) solidities.size());
        newDataSizes.push_back((double) aspectRatios.size());

        double originalTotal = DataUtils::Total(dataSizes);
        double removedTotal = DataUtils::Total(newDataSizes);

        double totalRemoved = originalTotal - removedTotal;
        double avgRemoved = totalRemoved / newDataSizes.size();

        double averageHorizontalDistance = DataUtils::Average(horizontalDistances);
        double averageVerticaldistance   = DataUtils::Average(verticalDistances);
        double averageAngle              = DataUtils::Average(angles);
        double averageSolidity           = DataUtils::Average(solidities);
        double averageAspectRatio        = DataUtils::Average(aspectRatios);

        SettingPair horizontalDistancePair = SettingPair(averageHorizontalDistance, 0.25);
        SettingPair verticalDistancePair   = SettingPair(averageVerticaldistance, 0.25);
        SettingPair anglePair              = SettingPair(averageAngle, 10.0);
        SettingPair solidityPair           = SettingPair(averageSolidity, 0.25);
        SettingPair aspectRatioPair        = SettingPair(averageAspectRatio, 0.25);

        ExampleContour newExampleContour = ExampleContour(i, horizontalDistancePair, verticalDistancePair, anglePair, aspectRatioPair, solidityPair, minArea);
        finishedContours.push_back(newExampleContour);

        double percentComplete = (((double) i / (double) regularNumberOfContours) * 10.0) + 90.0;
        this->outString = "Analyzing data from contours (" + std::to_string((int) percentComplete) + "%)";

        std::cout << std::endl;
    }

    ExampleTarget newTarget = ExampleTarget(0, finishedContours, 0.0, 0.0, 0.0, 0.0);
    return newTarget;
}

/**
 * Updates the output image
 */
void ConfigLearner::Update(int minArea) {
    Mat img;
    bool success = false;
    if(RunnerSettings::USE_CAMERA) {
        success = this->stream.read(img);
    } else {
        success = true;
        img = imread(RunnerSettings::IMAGE_TO_USE);
        // img = Mat(this->constantResize, CV_8UC3, Scalar(0, 0, 0));
    }

    if(success) {
        resize(img, img, this->constantResize);
        img.copyTo(this->original);
        Mat preprocessed = this->preprocessor.ProcessImage(img);
        preprocessed.copyTo(this->out);

        cvtColor(this->out, this->out, cv::COLOR_GRAY2BGR);
        
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
    this->stream.~VideoCapture();
}


void ConfigLearner::SetConstantResize(Size sz) {
    this->constantResize = sz;
}
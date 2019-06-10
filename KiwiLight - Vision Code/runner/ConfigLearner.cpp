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
    std::cout << "Learning Target" << std::endl;
    std::vector<CameraFrame> frames = std::vector<CameraFrame>();

    //capture the images that will be used to learn the target
    std::cout << "min area: " << minArea << std::endl;
    for(int i=0; i<NUMBER_OF_FRAMES_TO_LEARN; i++) {
        Mat img;
        bool success = this->stream.read(img);

        if(success) {
            img = this->preprocessor.ProcessImage(img);
            CameraFrame newFrame = CameraFrame(img, minArea);
            frames.push_back(newFrame);
        }
    }

    int framesCaptured = frames.size();
    std::cout << "Captured " << framesCaptured << "/" << NUMBER_OF_FRAMES_TO_LEARN << " frames successfully." << std::endl;

    //create a sorted list of the number of contours in each image (FYI it is double because DataUtils sorts doubles)
    std::vector<double> numberContoursList;
    for(int i=0; i<frames.size(); i++) {
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

    std::cout << "Normal number of contours is " << (int) regularNumberOfContours << ". Discarded " << (NUMBER_OF_FRAMES_TO_LEARN - frames.size()) << " Invalid frames." << std::endl;

    
}

/**
 * Updates the output image
 */
void ConfigLearner::Update(int minArea) {
    Mat img;
    bool success = this->stream.read(img);
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
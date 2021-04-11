#ifndef KiwiLight_RUNNER_H
#define KiwiLight_RUNNER_H

#include "Settings.h"
#include "../util/Util.h"
#include "opencv2/opencv.hpp"

/**
 * The main header file for the KiwiLight Runner
 * Written By: Brach Knutson
 */

using namespace cv;

namespace KiwiLight {

    /**
     * Descriptor enumeration for the type of preprocessor being used
     */
    enum PreProcessorType {
        FULL,
        PARTIAL
    };

    /**
     * KiwiLight Runner Properties.
     */
    enum RunnerProperty {
        OFFSET_X,
        OFFSET_Y,
        IMAGE_WIDTH,
        IMAGE_HEIGHT,
        TRUE_WIDTH,
        PERCEIVED_WIDTH,
        CALIBRATED_DISTANCE,
        ERROR_CORRECTION,
        CALC_DIST_BY_HEIGHT,
        MAX_CONTOURS
    };

    /**
     * KiwiLight PreProcessor Properties.
     */
    enum PreProcessorProperty {
        IS_FULL,
        THRESHOLD,
        DILATION,
        EROSION,
        COLOR_HUE,
        COLOR_SATURATION,
        COLOR_VALUE,
        COLOR_ERROR
    };

    /**
     * KiwiLight Target Properties.
     */
    enum TargetProperty {
        DIST_X,
        DIST_Y,
        ANGLE,
        ASPECT_RATIO,
        SOLIDITY,
        MINIMUM_AREA
    };

    /**
     * KiwiLight Distance Calculation Modes.
     */
    enum DistanceCalcMode {
        BY_WIDTH,
        BY_HEIGHT
    };

    /**
     * KiwiLight Troubleshooter output data.
     */
    struct TroubleshootingData {
        int missedHorizontalDistances,
            missedVerticalDistances,
            missedAngles,
            missedSolidities,
            missedAspectRatios;

        double averageHorizontalDistance,
               averageVerticalDistance,
               averageAngle,
               averageSolidity,
               averageAspectRatio;

        double targetHorizontalDistance,
               targetVerticalDistance,
               targetAngle,
               targetSolidity,
               targetAspectRatio;
    };
    
    /**
     * An object found in a postprocessed image.
     * This class can be described as a "wrapper class" for vector<Point>'s (output of cv::findContours())
     */
    class Contour {
        public:
        Contour() {};
        Contour(std::vector<cv::Point> points);
        int X()        { return this->x;      };
        int Y()        { return this->y;      };
        Point Center() { return this->center; };
        int Width()    { return this->width;  };
        int Height()   { return this->height; };
        int Angle()    { return this->angle;  };
        int Area()     { return (this->width * this->height);   };
        double AspectRatio() { return this->width / (double) this->height; };
        double Solidity()    { return this->solidity;    };

        private:
        std::vector<cv::Point> points;
        int x,
            y,
            width,
            height,
            angle;
        
        double solidity;

        Point center;
    };

    /**
     * A imaginary template for a real contour. This class compares Contours with itself and can tell if a contour can "represent" itself.
     */
    class ExampleContour {
        public:
        ExampleContour() {};
        ExampleContour(int id);
        ExampleContour(int id,
                       SettingPair distX,
                       SettingPair distY,
                       SettingPair angle,
                       SettingPair aspectRatio,
                       SettingPair solidity,
                       int minimumArea);

        bool IsContour(Contour contour);
        int ID() { return this->id; };
        SettingPair DistX()       { return this->distX; };
        SettingPair DistY()       { return this->distY; };
        SettingPair Angle()       { return this->angle; };
        SettingPair AspectRatio() { return this->aspectRatio; };
        SettingPair Solidity()    { return this->solidity; };
        int MinimumArea()         { return this->minimumArea; };

        void SetDistX(SettingPair distX);
        void SetDistY(SettingPair distY);
        void SetAngle(SettingPair angle);
        void SetAspectRatio(SettingPair aspectRatio);
        void SetSolidity(SettingPair solidity);
        void SetMinimumArea(int minimumArea);

        private:
        int id;
        SettingPair distX,
                    distY,
                    angle,
                    aspectRatio,
                    solidity;

        int minimumArea;
    };

    /**
     * Represents a Target which is found in an image. Provides information useful to the robot for alignment and other auto functions.
     */
    class Target {
        public:
        Target();
        Target(int id, std::vector<Contour> contours, double knownHeight, double focalHeight, double distErrorCorrect, double calibratedDistance, DistanceCalcMode distMode);
        int ID() { return this->id; };
        std::vector<Contour> Contours() { return this->contours; };
        double Distance();
        double Distance(DistanceCalcMode mode);
        double HorizontalAngle(int imageCenterX);
        double HorizontalAngle(double distanceToTarget, int imageCenterX);
        double VerticalAngle(int imageCenterY);
        double VerticalAngle(double distanceToTarget, int imageCenterY);
        double ObliqueAngle(int imageCenterX, int imageCenterY);
        double KnownWidth() { return this->knownHeight; };
        double FocalWidth() { return this->focalHeight; };
        double DistanceErrorCorrection() { return this->distErrorCorrect; };
        double CalibratedDistance() { return this->calibratedDistance; };
        cv::Point Center() { return cv::Point(this->x, this->y); };
        cv::Rect Bounds();

        private:
        std::vector<Contour> contours;

        int id,
            x,
            y,
            width,
            height;

        double knownHeight,
               focalHeight,
               distErrorCorrect,
               calibratedDistance;

        DistanceCalcMode distMode;
    };

    /**
     * An imaginary template for a real Target.
     * This class does most of the PostProcessor's heavy lifting, specifically picking Targets out of groups of Contours.
     */
    class ExampleTarget {
        public:
        ExampleTarget() {};
        ExampleTarget(
            int id, 
            std::vector<ExampleContour> contours, 
            double knownHeight,
            double focalHeight, 
            double distErrorCorrect, 
            double calibratedDistance, 
            DistanceCalcMode mode,
            int maxContours
        );
        std::vector<Target> GetTargets(std::vector<Contour> contours);
        bool isTarget(std::vector<Contour> contours);
        std::vector<Contour> GetValidContours(std::vector<Contour> contours);
        int ID() { return this->id; };
        std::vector<ExampleContour> Contours() { return this->contours; };
        ExampleContour GetExampleContourByID(int id);
        void SetContourProperty(int contour, TargetProperty prop, SettingPair values);
        SettingPair GetContourProperty(int contour, TargetProperty prop);
        void SetTargetProperty(RunnerProperty prop, double value);
        double GetTargetProperty(RunnerProperty prop);

        private:
        bool ArrayMaxed(int arr[], int size, int max);
        bool ContainsDuplicates(int arr[], int size);
        bool CombonationAlreadyTested(int combonation[], std::vector< std::vector<int> > testedCombos, int comboSize);
        int id;
        std::vector<ExampleContour> contours;

        double knownHeight,
               focalHeight,
               distErrorCorrect,
               calibratedDistance;

        DistanceCalcMode distMode;

        int maxContours;
    };

    /**
     * A module which takes raw images and gets them ready for work by the PostProcessor.
     */
    class PreProcessor {
        public:
        PreProcessor() {};
        PreProcessor(bool FullPreprocessor, Color targetColor, int threshold, int erosion, int dilation, bool debugging);
        void SetProperty(PreProcessorProperty prop, double value);
        double GetProperty(PreProcessorProperty prop);
        cv::Mat ProcessImage(cv::Mat img);
        std::string toString();

        private:
        bool isFullPreprocessor,
             debugging;

        //threshold values for contrasting image
        double threshold,
            threshValue;
        
        int threshtype;

        //dilation values for expanding, blurring, or dilating
        int dilate,
            erode;
        
        //what the camera looks for
        Color targetColor;
    };

    /**
     * Takes preprocessed images and finds targets within them.
     */
    class PostProcessor {
        public:
        PostProcessor() {};
        PostProcessor(ExampleTarget target, bool debugging);
        void SetTarget(ExampleTarget target);
        int NumberOfContours();
        std::vector<Target> ProcessImage(cv::Mat img);
        std::vector<Contour> GetValidContoursForTarget(std::vector<Contour> contours);
        void SetTargetContourProperty(int contour, TargetProperty prop, SettingPair values);
        SettingPair GetTargetContourProperty(int contour, TargetProperty prop);
        void SetRunnerProperty(RunnerProperty prop, double value);
        double GetRunnerProperty(RunnerProperty prop);
        ExampleTarget GetTarget();
        std::vector<Contour> GetContoursFromLastFrame() { return this->contoursFromLastFrame; };

        private:
        bool debugging;
        ExampleTarget target;
        std::vector<Contour> contoursFromLastFrame;
    };

    /**
     * Calculates and stores data used by the ConfigLearner and the DistanceLearner.
     */
    class CameraFrame {
        public:
        CameraFrame() {};
        CameraFrame(Mat img, int minimumArea);
        Target SeenTarget() { return this->seenTarget; };
        int NumberOfContours() { return this->seenTarget.Contours().size(); };
        std::vector<Contour> GetContoursGrouped();
        std::vector<Contour> GetContours() { return this->seenTarget.Contours(); };
        int PositionValue(Contour contour);
        Distance GetContourDistance(Contour contour);

        private:
        Target seenTarget;
        Mat image;
    };

    /**
     * Utility which logs Runner activity into a log file which can be read by a LogViewer.
     */
    class Logger {
        public:
        static const int UPDATE_VECTOR_MAX_SIZE;
        static const int FILE_WRITE_INTERVAL;

        Logger() {};
        Logger(std::string filePath);
        void SetConfName(std::string confName, std::string confFilePath);
        void Start();
        void Log(std::string runnerOutput);

        private:
        void WriteNewFile();

        std::string 
            filePath,
            confFilePath,
            confName,
            beginTime,
            lastFrameMessage;
        Clock clock;
        long lastFrameTime;
        double 
            runningFPSAvg,
            runningDistanceAvg;
        int 
            totalFrames,            
            framesWithTargetSeen,
            lastGeneralUpdateTime,
            generalUpdateInterval,
            lastFileWriteTime;

        std::vector<LogEvent> updates;
        LogEvent 
            fastestFPSEvent,
            slowestFPSEvent,
            closestDistanceEvent,
            farthestDistanceEvent;
    };

    /**
     * Handles everything vision from taking images to send coordinates to a RoboRIO(or other UDP destination)
     */
    class Runner {
        public:
        static const std::string NULL_MESSAGE;

        Runner() {};
        Runner(std::string filename, bool debugging);
        Runner(std::string filename, bool debugging, bool applyCameraSettings);
        PreProcessor GetPreProcessor() { return this->preprocessor; };
        PostProcessor GetPostProcessor() { return this->postprocessor; };
        int GetCameraIndex() { return this->cameraIndex; };
        void SetImageResize(Size sz);
        std::string Iterate();
        bool GetLastFrameSuccessful() { return this->lastIterationSuccessful; };
        std::vector<Target> GetLastFrameTargets() { return this->lastFrameTargets; };
        Target GetClosestTargetToCenter() { return this->closestTarget; };
        Point GetLastFrameCenterPoint() { return this->lastFrameCenterPoint; };
        std::string GetFileName() { return this->src; };
        std::string GetConfName() { return this->configName; };
        cv::Mat GetOriginalImage() { return this->originalImage; };
        cv::Mat GetOutputImage() { return this->outputImage; };
        Size GetConstantSize() { return this->constantResize; };
        int NumberOfContours();
        ExampleTarget GetExampleTarget();
        void SetExampleTarget(ExampleTarget target);
        void SetPreprocessorProperty(PreProcessorProperty prop, double value);
        double GetPreprocessorProperty(PreProcessorProperty prop);
        void SetPostProcessorContourProperty(int contour, TargetProperty prop, SettingPair values);
        SettingPair GetPostProcessorContourProperty(int contour, TargetProperty prop);
        void SetRunnerProperty(RunnerProperty prop, double value);
        double GetRunnerProperty(RunnerProperty prop);

        private:
        void parseDocument(XMLDocument doc, bool applyUDP);
        void applySettings(XMLDocument document);

        PreProcessor preprocessor;
        PostProcessor postprocessor;
        Size constantResize;

        Target closestTarget;

        std::string src,
                    configName;

        int cameraIndex;

        cv::Mat outputImage,
                originalImage;

        ExampleTarget postProcessorTarget;
        std::vector<Target> lastFrameTargets;
        Point lastFrameCenterPoint;
        bool lastIterationSuccessful;
        bool stop,
             debug;

        double centerOffsetX,
               centerOffsetY;

        int maxSendRate;
    };

    /**
     * utility that learns a seen target
     */
    class ConfigLearner {
        public:
        ConfigLearner() {};
        ConfigLearner(PreProcessor preprocessor, Size imageSize);
        void StartLearning();
        void SetMinimumArea(int minimumArea);
        void Feed();
        ExampleTarget StopLearning();
        bool GetLearning() { return this->currentlyLearning; };
        bool GetHasFailed() { return this->failedFrames > 10; };
        int GetFramesLearned();
        Mat GetOutputImageFromLastFeed() { return this->out; };
        Mat GetOriginalImageFromLastFeed() { return this->original; };

        private:
        PreProcessor preprocessor;
        Size imageSize;
        int minimumArea;

        bool currentlyLearning;
        std::vector<CameraFrame> currentFrames;

        int failedFrames;

        Mat out,
            original;
    };


    /**
     * A tool that calculates the constants needed for accurate distance calculation
     */
    class TargetDistanceLearner {
        public:
        TargetDistanceLearner() {};
        TargetDistanceLearner(PreProcessor preprocessor, PostProcessor postprocessor);
        void FeedImage(Mat img);
        void FeedTarget(Target targ);
        void FeedBlank();
        int GetFramesLearned();
        bool GetHasFailed() { return this->failedFrames > 10; };
        double GetFocalWidth(double trueDistance, double trueWidth);

        private:
        int frames;
        int failedFrames;
        PreProcessor preprocessor;
        PostProcessor postprocessor;
        std::vector<double> targetWidths;
    };

    /**
     * A tool which troubleshoots Targets against ExampleTargets and reveals the Target's problems.
     * 
     * NOTE: This tool was an early (pre-v1.0) concept which was not actually implemented into current versions of KiwiLight.
     * As a result, this class does not comply with some of the post-1.0 archetecture, such as the universal KiwiLightApp VideoCapture.
     * This class has been tested, though not completely. It can be used, but there is no garantee that it will work.
     */
    class TargetTroubleshooter {
        public:
        TargetTroubleshooter() {};
        TargetTroubleshooter(VideoCapture cap, PreProcessor preprocessor, ExampleTarget target);
        bool Troubleshoot(TroubleshootingData dataOut[]);
        std::string GetOutputString() { return this->outString; };

        private:
        VideoCapture cap;
        PreProcessor preprocessor;
        ExampleTarget target;

        int framesCollected;
        std::string outString;
    };
}

#endif

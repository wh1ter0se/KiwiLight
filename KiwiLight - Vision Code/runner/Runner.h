#ifndef KiwiLight_RUNNER_H
#define KiwiLight_RUNNER_H

#include "Settings.h"
#include "../util/Util.h"
#include "opencv2/opencv.hpp"

/**
 * The main header file for the KiwiLight Runner
 * Written By: Brach Knutson
 */

namespace KiwiLight {

    /**
     * Descriptor enumeration for the type of preprocessor being used
     */
    enum PreProcessorType {
        FULL,
        PARTIAL
    };

    enum RunnerProperty {
        IMAGE_WIDTH,
        IMAGE_HEIGHT,
        TRUE_WIDTH,
        PERCEIVED_WIDTH,
        CALIBRATED_DISTANCE,
        ERROR_CORRECTION
    };

    
    enum PreProcessorProperty {
        IS_FULL,
        THRESHOLD,
        THRESH_VALUE,
        DILATION,
        COLOR_HUE,
        COLOR_SATURATION,
        COLOR_VALUE,
        COLOR_ERROR
    };

    /**
     * Describes the names of certain properties of a target.
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
     * Represents a single setting that is used by either the PostProcessor or Preprocessor.
     */
    class ConfigurationSetting {
        public:
        ConfigurationSetting(std::string name, std::string value);
        std::string Name() { return this->name; };
        void SetValue(std::string value);
        std::string GetValue() { return this->value; };

        private:
        std::string name;
        std::string value;
    };

    /**
     * Represents a list of ConfigurationSetting's
     */
    class ConfigurationSettingsList {
        public:
        ConfigurationSettingsList();
        void AddSetting(std::string settingName, std::string value);
        void SetSetting(std::string settingName, std::string value);
        std::string GetSetting(std::string settingName);

        private:
        std::vector<ConfigurationSetting> settings;
    };


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

    
    class ExampleContour {
        public:
        ExampleContour() {};
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
     * Represents a target found in an image
     */
    class Target {
        public:
        Target();
        Target(int id, std::vector<Contour> contours, double knownHeight, double focalHeight, double distErrorCorrect, double calibratedDistance);
        int ID() { return this->id; };
        std::vector<Contour> Contours() { return this->contours; };
        double Distance();
        int Angle(int imageCenterX);
        int Angle(double distanceToTarget, int imageCenterX);
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
    };


    class ExampleTarget {
        public:
        ExampleTarget() {};
        ExampleTarget(int id, std::vector<ExampleContour> contours, double knownHeight, double focalHeight, double distErrorCorrect, double calibratedDistance);
        std::vector<Target> GetTargets(std::vector<Contour> contours);
        bool isTarget(std::vector<Contour> contours);
        int ID() { return this->id; };
        std::vector<ExampleContour> Contours() { return this->contours; };
        ExampleContour GetExampleContourByID(int id);
        void SetContourProperty(int contour, TargetProperty prop, SettingPair values);
        SettingPair GetContourProperty(int contour, TargetProperty prop);
        void SetTargetProperty(RunnerProperty prop, double value);
        double GetTargetProperty(RunnerProperty prop);
        void AddGenericContour();

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
    };

    /**
     * A module which takes images and gets them ready for work by the PostProcessor.
     */
    class PreProcessor {
        public:
        PreProcessor() {};
        PreProcessor(ConfigurationSettingsList settings, bool FullPreprocessor, bool debug);
        PreProcessor(bool FullPreprocessor, Color targetColor, int threshold, int dilation, bool debugging);
        void SetProperty(PreProcessorProperty prop, double value);
        double GetProperty(PreProcessorProperty prop);
        cv::Mat ProcessImage(cv::Mat img);

        private:
        bool isFullPreprocessor,
             debugging;

        //threshold values for contrasting image
        double threshold,
            threshValue;
        
        int threshtype;

        //dilation values for expanding, blurring, or dilating
        int dilate;
        
        //what the camera looks for
        Color targetColor;
    };

    /**
     * Takes preprocessed images and finds targets within them.
     */
    class PostProcessor {
        public:
        PostProcessor() {};
        PostProcessor(std::vector<ExampleTarget> targets, bool debugging);
        //x, y, angle, solidity, aspectratio, minarea
        int NumTargets() { return this->targets.size(); };
        void SetTargetContourProperty(int contour, TargetProperty prop, SettingPair values);
        SettingPair GetTargetContourProperty(int contour, TargetProperty prop);
        void SetRunnerProperty(RunnerProperty prop, double value);
        double GetRunnerProperty(RunnerProperty prop);
        ExampleTarget GetExampleTargetByID(int id);
        std::vector<Target> ProcessImage(cv::Mat img);

        private:
        bool debugging;
        std::vector<ExampleTarget> targets;
    };


    class CameraFrame {
        public:
        CameraFrame() {};
        CameraFrame(Mat img, int minimumArea);
        Target SeenTarget() { return this->seenTarget; };
        int NumberOfContours() { return this->seenTarget.Contours().size(); };
        std::vector<Contour> GetContoursGrouped();
        std::vector<Contour> GetContours() { return this->seenTarget.Contours(); };
        Distance GetContourDistance(Contour contour);

        private:
        Target seenTarget;
    };

    /**
     * Handles everything vision from taking images to send coordinates to a RoboRIO(or other UDP destination)
     */
    class Runner {
        public:
        Runner() {};
        Runner(std::string filename, bool debugging);
        Runner(std::string filename, bool debugging, bool openNewVideoStream);
        Runner(std::string filename, bool debugging, VideoCapture cap);
        PreProcessor GetPreProcessor() { return this->preprocessor; };
        PostProcessor GetPostProcessor() { return this->postprocessor; };
        int GetCameraIndex() { return this->cameraIndex; };
        void SetImageResize(Size sz);
        void Loop();
        void Stop();
        void StopLoopOnly();
        void UnlockLoop();
        void Start();
        std::string Iterate();
        int GetNumberOfTargets() { return this->postprocessor.NumTargets(); };
        std::vector<Target> GetLastFrameTargets() { return this->lastFrameTargets; };
        Target GetClosestTargetToCenter() { return this->closestTarget; };
        std::string GetFileName() { return this->src; };
        cv::Mat GetOriginalImage() { return this->originalImage; };
        cv::Mat GetOutputImage() { return this->outputImage; imshow("out", this->outputImage); };
        cv::VideoCapture GetVideoStream() { return this->cap; };
        Size GetConstantSize() { return this->constantResize; };
        ExampleTarget GetExampleTargetByID(int id);
        void SetUDPEnabled(bool enabled);
        bool GetUDPEnabled();
        void SetExampleTarget(int targetID, ExampleTarget target);
        void SetPreprocessorProperty(PreProcessorProperty prop, double value);
        double GetPreprocessorProperty(PreProcessorProperty prop);
        void SetPostProcessorContourProperty(int contour, TargetProperty prop, SettingPair values);
        SettingPair GetPostProcessorContourProperty(int contour, TargetProperty prop);
        void SetRunnerProperty(RunnerProperty prop, double value);
        double GetRunnerProperty(RunnerProperty prop);
        std::string GetSetting(std::string settingName);

        private:
        void parseDocument(XMLDocument doc);
        void applySettings();

        VideoCapture cap;
        PreProcessor preprocessor;
        PostProcessor postprocessor;
        Size constantResize;
        Target closestTarget;

        std::string src;
        int cameraIndex;

        cv::Mat outputImage,
                originalImage;
        ConfigurationSettingsList settings;
        std::vector<ExampleTarget> postProcessorTargets;
        std::vector<Target> lastFrameTargets;
        bool stop,
             debug;

        int centerOffset;

        UDP udp;
    };

    /**
     * utility that learns a seen target
     */
    class ConfigLearner {
        public:
        ConfigLearner() {};
        ConfigLearner(PreProcessor preprocessor, VideoCapture stream);
        ExampleTarget LearnTarget(int minArea);
        void Update(int minArea);
        void Stop();
        void SetConstantResize(Size sz);
        std::string GetOutputString() { return this->outString; }; //this is here because I run this in a separate thread and want to show progress
        cv::Mat GetOriginalImage() { return this->original; };
        cv::Mat GetOutputImage() { return this->out; };

        private:
        VideoCapture stream;
        Size constantResize;
        cv::Mat original,
                out;
        PreProcessor preprocessor;

        int framesCollected;
        std::string outString;
    };

    /**
     * A tool that calculates the constants needed for accurate distance calculation
     */
    class TargetDistanceLearner {
        public:
        TargetDistanceLearner() {};
        TargetDistanceLearner(PreProcessor preprocessor, PostProcessor postprocessor, VideoCapture cap, Size constantSz);
        double LearnFocalWidth(double trueWidth, double trueDistance);
        double LearnErrorCorrect(double trueWidth, double trueDistance, double calibratedDistance, double focalWidth);
        std::string GetOutputString() { return this->outString; };

        private:
        Size constantResize;
        PreProcessor preprocessor;
        PostProcessor postprocessor;
        VideoCapture cap;

        int framesCollected;
        std::string outString;
    };
}

#endif
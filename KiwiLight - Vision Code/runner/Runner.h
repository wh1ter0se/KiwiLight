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
        Contour(std::vector<cv::Point> points);
        int X()      { return this->x;      };
        int Y()      { return this->y;      };
        int Width()  { return this->width;  };
        int Height() { return this->height; };
        int Angle()  { return this->angle;  };
        int Area()   { return this->width * this->height;   };
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
    };

    
    class ExampleContour {
        public:
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
        Target() {};
        Target(int id, std::vector<Contour> contours);
        int ID() { return this->id; };
        cv::Point Center() { return cv::Point(this->x, this->y); };
        cv::Rect Bounds();

        private:
        std::vector<Contour> contours;

        int id,
            x,
            y,
            width,
            height;
    };


    class ExampleTarget {
        public:
        ExampleTarget(int id, std::vector<ExampleContour> contours);
        std::vector<Target> GetTargets(std::vector<Contour> contours);
        bool isTarget(std::vector<Contour> contours);
        int ID() { return this->id; };
        std::vector<ExampleContour> Contours() { return this->contours; };

        private:
        bool ArrayMaxed(int arr[], int size, int max);
        bool ContainsDuplicates(int arr[], int size);
        int id;
        std::vector<ExampleContour> contours;
    };

    /**
     * A module which takes images and gets them ready for work by the PostProcessor.
     */
    class PreProcessor {
        public:
        PreProcessor(ConfigurationSettingsList settings, bool FullPreprocessor);
        cv::Mat ProcessImage(cv::Mat img);

        private:
        bool isFullPreprocessor;

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
        PostProcessor(std::vector<ExampleTarget> targets);
        std::vector<Target> ProcessImage(cv::Mat img);

        private:
        std::vector<ExampleTarget> targets;
    };

    /**
     * Handles everything vision from taking images to send coordinates to a RoboRIO(or other UDP destination)
     */
    class Runner {
        public:
        Runner(std::string filename, bool debugging);
        void Loop();
        void SetSetting(std::string settingName, std::string value);
        std::string GetSetting(std::string settingName);

        private:
        UDP udp;
        ConfigurationSettingsList settings;
        std::vector<ExampleTarget> postProcessorTargets;
        void parseDocument(XMLDocument doc);
        bool stop,
             debug;

        int centerOffset;
    };
}

#endif
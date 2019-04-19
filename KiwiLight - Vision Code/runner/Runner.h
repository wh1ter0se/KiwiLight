#ifndef KiwiLight_RUNNER_H
#define KiwiLight_RUNNER_H

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

    
    class ExampleContour {
        public:
        ExampleContour(int x, int y, int width, int height, int angle, int solidity);
        int X()           { return this->x;                    };
        int Y()           { return this->y;                    };
        int Width()       { return this->width;                };
        int Height()      { return this->height;               };
        int Angle()       { return this->angle;                };
        int Area()        { return this->width * this->height; };
        int AspectRatio() { return this->width / this->height; };
        int Solidity()    { return this->solidity;             };

        private:
        int x,
            y,
            width,
            height,
            angle,
            solidity;
    };


    class Contour {
        public:
        Contour(std::vector<cv::Point> points);
        int X()      { return this->x;      };
        int Y()      { return this->y;      };
        int Width()  { return this->width;  };
        int Height() { return this->height; };
        int Angle()  { return this->angle;  };
        int Area()   { return this->area;   };
        double AspectRatio() { return this->aspectRatio; };
        double Solidity()    { return this->solidity;    };

        private:
        std::vector<cv::Point> points;
        int x,
            y,
            width,
            height,
            angle,
            area;
        
        double aspectRatio,
               solidity;
    };


    /**
     * Represents a target found in an image
     */
    class Target {
        public:
        Target(int id, std::vector<Contour> contours);
        int X() { return this->x; };
        int Y() { return this->y; };

        private:
        std::vector<Contour> contours;

        int id,
            x,
            y;
    };


    class ExampleTarget {
        public:
        ExampleTarget(int id, std::vector<ExampleContour> contours);
        std::vector<Target> GetTargets(std::vector<Contour> contours);
        bool isTarget(std::vector<Contour> contours);
        int ID() { return this->id; };
        std::vector<ExampleContour> Contours() { return this->contours; };

        private:
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
        PostProcessor(ConfigurationSettingsList settings);
        std::vector<Target> ProcessImage(cv::Mat img);
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
        ConfigurationSettingsList settings;
        void parseDocument(XMLDocument doc);
        bool stop,
             debug;
    };
}

#endif
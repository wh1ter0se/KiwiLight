#ifndef KiwiLight_H
#define KiwiLight_H

#include <iostream>
#include <string>
#include <fstream>
#include "ui/UI.h"
#include "util/Util.h"
#include "runner/Runner.h"

/**
 * the main header for the KiwiLight program and namespace
 * Written by: Brach Knutson
 */


using namespace cv;

namespace KiwiLight {

    /**
     * Represents a USB camera or built in webcam.
     */
    class Camera {
        public:
        Camera() {};
        Camera(int index);
        void Update();
        void SetIndex(int index);
        void SetWidth(int width);
        void SetHeight(int height);
        int GetIndex() { return this->index; };
        bool isOpen() { return this->stream.isOpened() && this->streamingSuccessful; };
        bool QueryPropertySupported(int propid);

        private:
        void Close();
        void Open();
        int iteration, 
            index,
            frameWidth,
            frameHeight;
        bool opened,
             streamingSuccessful;
        cv::VideoCapture stream;
    };

    /**
     * A window where camera settings can be modified.
     */
    class Settings : public Widget {
        public:
        Settings() {};
        Settings(int index);
        // ~Settsings();
        void Update();
        void UpdateValue();
        void Show() { gtk_widget_show_all(this->settingsWidget); };
        int GetWidth() { return camWidth; };
        int GetHeight() { return camHeight; };
        GtkWidget *GetWidget() { return settingsWidget; };

        private:
        static void ScheduleApplySettings();
        static void FRCSettings();
        int searchAndReturnValue(std::string searchString, std::string term);
        CameraSetting frameWidth,
                      frameHeight;
        std::vector<CameraSetting> settings;
        int camWidth,
            camHeight;
        GtkWidget *settingsWidget;
    };

    /**
     * A UI event manager, which keeps static callbacks in one place
     */
    class Kiwi {
        public:
        static void ShowAbout();
        static void HELPME();
        static void AddConfig();
        static void SaveConfig();
        static void RunSelected();
        static void OpenConfig();
        static void ConfStartConfigOnBoot();
        static void CompileConfig();
        static void EditSelected();
    };
}

#endif
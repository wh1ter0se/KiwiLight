#ifndef KiwiLight_H
#define KiwiLight_H

#include <iostream>
#include <string>
#include <fstream>
#include "ui/UI.h"
#include "util/Util.h"
#include "runner/Runner.h"
#include "confs/json_interpreter/Json.h"

/**
 * the main header for the KiwiLight program and namespace
 * Written by: Brach Knutson
 */

using namespace std;
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
        int GetIndex() { return this->index; };
        bool isOpen() { return this->stream.isOpened(); };
        bool QueryPropertySupported(int propid);

        private:
        int iteration;
        int index;
        bool opened;
        cv::VideoCapture stream;
    };

    /**
     * A window where camera settings can be modified.
     */
    class Settings : public Widget {
        public:
        Settings();
        static void ScheduleApplySettings();
        void Update();
        GtkWidget *GetWidget() { return settingsWidget; };

        private:
        int searchAndReturnValue(string searchString, string term);
        vector<CameraSetting> settings;
        GtkWidget *settingsWidget;
    };

    /**
     * Controls the UI of the program.
     */
    class Kiwi {
        public:
        Kiwi() {};
        Kiwi( void (*UpdateMethod)() );
        void UpdateSettings();
        static void UpdateCamera();
        static void ShowAbout();
        static void HELPME();

        private:
        void CreateMenuBar();
        static void AddConfig();
        static void SaveConfig();
        static void RunSelected();
        static void OpenConfig();
        static void ConfStartConfigOnBoot();
        static void CompileConfig();
        static void EditSelected();

        Window win;
        MenuBar menubar;
        Settings camSettings;
        Panel content, data, confPanel, camera;
        Frame config, cameraFrame;
        static Camera cam; 
    };
}

#endif
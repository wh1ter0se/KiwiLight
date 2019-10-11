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

    enum UIMode {
        UI_STREAM,
        UI_RUNNER,
        UI_EDITOR,
        UI_QUTTING
    };

    /**
     * Its KiwiLight! Handles everything from building, starting, calling and things in the UI.
     */
    class KiwiLightApp {
        public:
        //UI building and starting
        static void Create(int argc, char *argv[]);
        static void Start();

        //UI accessors
        static VideoCapture GetCamera();
        static Runner GetRunner();
        static ConfigEditor GetEditor();

        //misc. UI callbacks
        static void CloseEditor(bool saveFirst);
        static void StartEditorLearningTarget();
        static void StartEditorLearningDistance();
        static void EditorReconnectUDP();
        static void EditorSetImageResolutionFromOverview();
        static void EditorConnectUDPFromOverview();
        static void EditorApplyCameraSettings();

        private:
        //menu bar utility
        static MenuBar CreateMenuBar();

        //streaming thread launcher
        static void LaunchStreamingThread();

        //UI constant callbacks
        static void UpdateApp();
        static void UpdateStreamsConstantly();
        static void UpdateStreams();

        //UI button callbacks
        static void OpenNewCamera();
        static void ToggleUDP();
        static void NewConfiguration();
        static void EditConfiguration();
        static void OpenConfiguration();
        static void CloseConfiguration();
        static void Quit();
        static void ShowAboutWindow();
        static void ShowHelpWindow();

        //essential objects
        static VideoCapture camera;
        static Runner runner;
        static ConfigEditor configeditor;

        //utilities
        static UIMode mode;
        static bool lastFrameGrabSuccessful;
        static Mat lastFrameGrabImage;

        //ui widgets
        static Window win;
        static ConfigPanel confInfo;
        static NumberBox cameraIndexBox;
        static Label cameraStatusLabel;
        static Image outputImage;
        static Button toggleUDPButton;
    };

}

#endif
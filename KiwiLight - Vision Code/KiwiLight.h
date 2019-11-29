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
        UI_PAUSING
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
        static Runner GetRunner();
        static ConfigEditor GetEditor();

        //camera accessors
        static Mat TakeImage();
        static double GetCameraProperty(int propId);
        static bool CameraOpen();

        //camera mutators
        static void SetCameraProperty(int propId, double value);

        //general accessors 
        static bool LastImageCaptureSuccessful();
        static UIMode CurrentMode();

        //misc. UI callbacks
        static void CloseEditor(bool saveFirst);
        static void StartEditorLearningTarget();
        static void StartEditorLearningDistance();
        static void EditorReconnectUDP();
        static void EditorSetImageResolutionFromOverview();
        static void EditorConnectUDPFromOverview();
        static void EditorApplyCameraSettings();
        static void EditorOpenNewCameraFromOverview();
        static void OpenNewCameraOnIndex(int index);

        private:
        //menu bar utility
        static MenuBar CreateMenuBar();

        //thread utilities
        static void LaunchStreamingThread(UIMode newMode);
        static void StopStreamingThread();

        //UI constant callbacks
        static void UpdateApp();
        static void UpdateStreamsConstantly();
        static void UpdateStreams();

        //UI button callbacks
        static void OpenNewCameraFromMainIndex();
        static void ToggleUDP();
        static void NewConfiguration();
        static void EditConfiguration();
        static void OpenConfiguration();
        static void OpenConfigurationFromFile(std::string fileName);
        static void CloseConfiguration();
        static void Quit();
        static void ShowAboutWindow();
        static void ShowHelpWindow();

        //essential objects
        static Runner runner;
        static ConfigEditor configeditor;
        static VideoCapture camera;
        static GThread *streamingThread;

        //utilities
        static UIMode mode;
        static bool lastImageGrabSuccessful;
        static bool udpEnabled;
        static bool streamThreadEnabled;
        static Mat lastFrameGrabImage;
        static int currentCameraIndex;

        //ui widgets
        static Window win;
        static ConfigPanel confInfo;
        static NumberBox cameraIndexBox;
        static Label cameraStatusLabel;
        static Image outputImage;
        static Button toggleUDPButton;
        
        //counters
        static int cameraFailures;
    };

}

#endif

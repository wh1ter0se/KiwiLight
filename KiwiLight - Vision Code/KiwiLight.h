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

    enum AppMode {
        UI_STREAM,
        UI_RUNNER,
        UI_EDITOR,
        UI_PAUSING,
        UI_HEADLESS,
    };

    /**
     * Its KiwiLight! Handles everything from building, starting, calling and things in the UI.
     */
    class KiwiLightApp {
        public:
        //UI building and starting
        static void Create(int argc, char *argv[]);
        static void Start();
        static void WaitForSocket();

        //logging
        static void ConfigureHeadless(std::string runnerNames, std::string runnerFiles);
        static void ReportHeadless(std::string runnerOut);

        //UI accessors
        static Runner GetRunner();
        static ConfigEditor GetEditor();
        static bool UIInitalized();

        //camera accessors
        static Mat TakeImage();
        static double GetCameraProperty(int propId);
        static bool CameraOpen();

        //camera mutators
        static void SetCameraProperty(int propId, double value);

        //general accessors 
        static bool LastImageCaptureSuccessful();
        static AppMode CurrentMode();
        static UDP GetUDP();
        static bool GetUDPEnabled();
        static std::string GetCurrentFile();

        //misc. UI callbacks
        static bool CloseEditor(bool saveFirst);
        static void StartEditorLearningTarget();
        static void StartEditorLearningDistance();
        static void EditorReconnectUDP();
        static void ToggleUDP();
        static void EditorSetImageResolutionFromOverview();
        static void EditorConnectUDPFromOverview();
        static void EditorApplyCameraSettings();
        static void EditorOpenNewCameraFromOverview();
        static void EditorApplyFRCSettings();
        static void EditorSetTargetColorHSV(int h, int s, int v);
        static void ToggleLogPlot();
        static void GenerateLogPlot();
        static void OpenNewCameraOnIndex(int index);
        static void ReconnectUDP(std::string newAddress, int newPort);
        static void ReconnectUDP(std::string newAddress, int newPort, bool block);
        static void SetUDPMaxSendRate(int maxSendRate);
        static void SendOverUDP(std::string message);
        static void CronDeleteButtonPressed(void* data, GtkWidget *widget);
        static void CronAddButtonPressed();

        //thread utilities
        static void LaunchStreamingThread(AppMode newMode);
        static void StopStreamingThread();

        private:
        //menu bar utility
        static MenuBar CreateMenuBar();

        //UI constant callbacks
        static void UpdateApp();
        static void UpdateStreamsConstantly();
        static void UpdateStreams();

        //UI button callbacks
        static void OpenNewCameraFromMainIndex();
        static void NewConfiguration();
        static void EditConfiguration();
        static void OpenConfiguration();
        static bool OpenConfigurationFromFile(std::string fileName);
        static void CloseConfiguration();
        static bool PromptEditorSaveAndClose();
        static bool PromptHeadlessStop();
        static void Quit();
        static void ShowCronMenu();
        static void RunHeadlessly();
        static void StopRunningHeadlessly();
        static void RunHeadlesslyCallback();
        static void ShowLog(XMLDocument log);
        static void ShowLog();
        static void ShowAboutWindow();
        static void ShowHelpWindow();

        //essential objects
        static Runner runner;
        static ConfigEditor configeditor;
        static CronWindow cronWindow;
        static Logger logger;
        static LogViewer logViewer;
        static VideoCapture camera;
        static UDP udpSender;
        static GThread 
            *streamingThread;

        //utilities
        static AppMode mode;
        static bool 
            lastImageGrabSuccessful,
            udpEnabled,
            streamThreadEnabled,
            lfgImgInUse,
            uiInitalized;
        static Mat 
            lastFrameGrabImage,
            defaultOutImage;
        static int currentCameraIndex;

        //ui widgets
        static Window win;
        static UDPPanel udpPanel;
        static ConfigPanel confInfo;
        static NumberBox cameraIndexBox;
        static Label cameraStatusLabel;
        static Image outputImage;
        static SubMenuItem runHeadlessly;
        static Button toggleRunningButton;
        
        //counters
        static int cameraFailures;
    };

}

#endif

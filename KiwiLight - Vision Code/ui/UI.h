#ifndef KiwiLight_UI_H
#define KiwiLight_UI_H

// #include "gnuplot_i.hpp"
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include "../util/Util.h"
#include "../runner/Runner.h"
#include "gtk-3.0/gtk/gtk.h"
#include "opencv2/opencv.hpp"

/**
 * the main header for the UI module for KiwiLight
 * Written By: Brach Knutson
 */

using namespace cv;

namespace KiwiLight {

    /**
     * Colorspaces that are used by the Image class.
     */
    enum ImageColorspace {
        RGB
    };

    /**
     * Represents a generic UI widget.
     */
    class Widget {
        public:
        GtkWidget *GetWidget();
        void SetName(std::string name);

        protected:
        GtkWidget *widget;
    };

    /**
     * Represents a container. Used to organize UI objects inside a window.
     */
    class Panel : public Widget {
        public:
        Panel() {};
        Panel(bool horizontal, int spacing);
        void Pack_start(GtkWidget *widget, bool expand, bool fill, int pad);
        void Pack_end(GtkWidget *widget, bool expand, bool fill, int pad);
        void Show() { gtk_widget_show_all(this->widget); };
    };

    /**
     * A panel with scrollbars.
     * DEPRECATED: This class is not used in KiwiLight and will be removed in the next update.
     */
    class [[deprecated("This class is not used in KiwiLight and will be removed in the next update.")]] Scrollable : public Widget {
        public:
        [[deprecated("The Scrollable class is not used and will be removed in the next update.")]] Scrollable() {};
        [[deprecated("The Scrollable class is not used and will be removed in the next update.")]] Scrollable(bool horizontal, bool vertical);
        [[deprecated("The Scrollable class is not used and will be removed in the next update.")]] void PackWidget(GtkWidget *wid);
    };

    /**
     * A frame with a label.
     * DEPRECATED: The Frame class is not used in KiwiLight and will be removed in the next update.
     */
    class [[deprecated("The Frame class is not used in KiwiLight and will be removed in the next update.")]] Frame : public Widget {
        public:
        Frame() {};
        [[deprecated("The Frame class is not used and will be removed in the next update.")]] Frame(std::string label);
        [[deprecated("The Frame class is not used and will be removed in the next update.")]] void Pack(GtkWidget *widget);
        [[deprecated("The Frame class is not used and will be removed in the next update.")]] void Unpack(GtkWidget *widget);
    };

    /**
     * Represents a UI window.
     */
    class Window : public Widget {
        public:
        Window() {};
        Window(GtkWindowType type);
        Window(GtkWindowType type, bool terminateOnClose);
        static void SetOnAppClosed(void(*onAppClosed)());
        void SetOnWindowClosed(void(*onWindowClosed)());
        void SetPane(Panel pane);
        void Show();
        void SetSize(int w, int h);
        int SetInterval(int interval, void(*method)());
        void RemoveInterval(int id);
        void Main() { gtk_main(); };
        void SetCSS(std::string fileName);
        
        private:
        static void(*onAppClosed)();
        static void(*timeoutMethod)();
        static gboolean delete_event(GtkWidget *widget, GdkEvent *event, gpointer pointer);
        static void Destroy();
        static gboolean timeoutCallMethod();
    };

    /**
     * Represents a label with text. 
     */
    class Label : public Widget {
        public:
        Label() {};
        Label(std::string text);
        void SetText(std::string text);
        std::string GetText() { return text; };
        void SetLineWrap(bool enabled);
        void SetJustify(int justify);

        static const int JUSTIFY_LEFT = 0,
                          JUSTIFY_RIGHT = 1,
                          JUSTIFY_CENTER = 2;

        private:
        std::string text;
        std::string font;
        double font_size;
    };

    /**
     * Represents a user-editable textbox.
     */
    class TextBox : public Widget {
        public:
        TextBox() {};
        TextBox(std::string text);
        void SetText(std::string text);
        void SetAlignment(double align);
        std::string GetText();
    };

    /**
     * A Box that contains a number, and buttons to increment/decrement it.
     */
    class NumberBox : public Widget {
        public:
        NumberBox() {};
        NumberBox(double min, double max, double value);
        NumberBox(double min, double max, double step, double value);
        void SetValue(double value);
        double GetValue();

        private:
        double min,
            max;
    };

    /**
     * Represents a checkbox with boolean value
     */
    class CheckBox : public Widget {
        public:
        CheckBox() {};
        CheckBox(std::string name, bool checked);
        void SetState(bool checked);
        bool GetState();
    };

    /**
     * Represents a user-editable scale widget.
     */
    class Slider : public Widget {
        public:
        Slider() {};
        Slider(double min, double max, double step, double value);
        Slider(bool horizontal, double min, double max, double step, double value);
        void SetValue(double value);
        double GetValue();
    };

    /**
     * A faint line that can be placed between to objects in a container
     */
    class Separator : public Widget {
        public:
        Separator(bool horizontal);
    };

    /**
     * Represents a button with a static method callback
     */
    class Button : public Widget {
        public:
        Button() {};
        Button(std::string text, void(*callback)() );
        void SetText(std::string text);
        std::string GetText() { return text; };

        //DEPRECATED
        [[deprecated("This method has not been implemented and is therfore not used in KiwiLight.")]]
        void SetCallback( void(*callback)() );
        
        private:
        std::string text;
    };

    /**
     * A blocking dialog with "ok" and "cancel" buttons.
     */
    class ConfirmationDialog : public Widget {
        public:
        ConfirmationDialog() {};
        ConfirmationDialog(std::string message);
        void SetBody(Panel pnl);
        bool ShowButDontClose();
        void ShowWithoutRunning();
        bool ShowAndGetResponse();
        void Destroy();

        private:
        Panel content;
    };

    /**
    * An option that would show under a menu bar item.
    */
    class SubMenuItem : public Widget {
        public:
        SubMenuItem() {};
        SubMenuItem(std::string name, void(*callback)());
        void SetText(std::string newLabel);
    };

    /**
     * A menu item able to be seen on a menu bar(ex "file", "help", etc)
     */
    class MenuItem : public Widget {
        public:
        MenuItem(std::string name);
        void AddSubmenuItem(SubMenuItem item);

        private:
        GtkWidget *menu;
    };

    /**
     * A menu bar that would show up at the top of the screen.
     */
    class MenuBar : public Widget {
        public:
        MenuBar();
        void AddItem(MenuItem item);
    };

    /**
     * A widget that displays an image, either loaded from file, or displayed from an OpenCV Mat.
     */
    class Image : public Widget {
        public:
        Image() { this->declared = false; };
        Image(std::string fileName);
        Image(ImageColorspace colorspace);
        bool Declared() { return this->declared; };
        void Update(cv::Mat newImage);

        private:
        bool declared;
        bool declaredAsStaticImage;
        ImageColorspace colorspace;

        //for one time init without updates
        Mat originalImage;
    };

    /**
     * A file chooser dialog.
     */
    class FileChooser : public Widget {
        public:
        FileChooser() {};
        FileChooser(bool writing, std::string defaultFileName);
        std::string Show();
    };

    /**
     * A panel with tabs which display different widgets.
     */
    class TabView : public Widget {
        public:
        TabView() {};
        TabView(std::string tab1Name, GtkWidget *tab1Content);
        void AddTab(std::string tabName, GtkWidget* tabContent);
    };

    /**
     * A dialog with a text box.
     * DEPRECATED: The PopupTextBox class is not used in KiwiLight and will be removed in the next update.
     */
    class [[deprecated("The PopupTextBox class is no longer used and will be removed in the next update.")]] PopupTextBox : public Widget {
        public:
        PopupTextBox() {};
        [[deprecated("The PopupTextBox class is no longer used and will be removed in the next update.")]] PopupTextBox(std::string name, std::string prompt, std::string initValue);
        [[deprecated("The PopupTextBox class is no longer used and will be removed in the next update.")]] std::string Show();

        private:
        TextBox textbox;
    };

    /**
     * A Slider with a label.
     */
    class LabeledSlider : public Widget {
        public:
        LabeledSlider() {};
        LabeledSlider(std::string label, double min, double max, double step, double value);
        LabeledSlider(bool horizontal, std::string label, double min, double max, double step, double value);
        double GetValue();
        void SetValue(double value);
        void SetLabel(std::string text);

        private:
        Panel panel;
        Label label;
        Slider slider;
    };

    /**
     * A window that shows information about KiwiLight.
     */
    class AboutWindow : public Widget {
        public:
        AboutWindow() {};
        AboutWindow(GtkWindowType type);
        void Show();
        
        private:
        Window window;
    };

    /**
     * A window that provides help with using KiwiLight.
     */
    class HelpWindow : public Widget {
        public:
        HelpWindow() {};
        HelpWindow(GtkWindowType type);
        void Show();

        private:
        Window window;
    };

    /**
     * A window that houses a crontab manager.
     */
    class CronWindow : public Widget {
        public:
        CronWindow() {};
        CronWindow(GtkWindowType type);
        bool isOpen();
        void Show();
        void Close();
        void SaveRule(bool shouldRun);

        private:
        bool isCurrentFileAutomatic();
        std::vector<std::string> ReadAllRules();
        std::vector<std::string> ReadKiwiLightRules();

        Window window;
        Label 
            cronStatus,
            currentRuleStatus;

        bool isOpened;
    };

    /**
     * A window that displays a KiwiLight log file.
     */
    class LogViewer : public Widget {
        public:
        static const std::string TEMPFILE_DIR;
        LogViewer() {};
        LogViewer(XMLDocument log);
        void Show();
        void TogglePlotShowing();
        void GenerateAndShowPlot();
        void Release();

        private:
        void createHorizontalReadout(std::string header, Label readout, bool isBig);
        void generatePlot(long elapsedTime, int maxFPS, int maxDist, LogEvent events[], const int numEvents);
        LogEvent eventFromTag(XMLTag tag);
        std::string timeFromMS(long ms);

        bool 
            initalized = false,
            showingPlot;
        Window window;
        Panel 
            contents,
            readouts;
        Label
            logFileName,
            logRecordedTime,
            confNames,
            confFiles,
            totalRunningTime,
            totalFrames,
            framesWithTargetSeen,
            targetLostEventCount,
            averageFrameTime,
            fastestFrameTime,
            slowestFrameTime,
            averageDistance,
            closestDistance,
            farthestDistance;
        Button plotButton;
        int 
            totalRunningTimeNum,
            totalFramesNum,
            framesWithTargetSeenNum,
            targetLostEventCountNum,
            closestDistanceNum,
            farthestDistanceNum,
            numEvents;
        double 
            averageFPSNum,
            averageDistanceNum,
            fastestFPSNum,
            slowestFPSNum;

        LogEvent *events;
    };

    /**
     * A singlular widget adjusting a camera setting
     */
    class CameraSetting : public Widget {
        public:
        CameraSetting() {};
        CameraSetting(std::string name, int valueName, double min, double max, double value);
        double GetValue();
        int GetValueName();
        void SetValue(double newValue);
        void Destroy();
        std::string GetName() { return name; };

        private:
        Label  nameLabel;
        NumberBox input;

        std::string 
            name,
            type;

        int 
            valueName,
            min, 
            max,
            value;
    };

    /**
     * Panel widget that displays the properties of the KiwiLight UDP socket sender.
     */
    class UDPPanel : public Widget {
        public:
        UDPPanel() {};
        UDPPanel(bool enabled);
        void SetAddress(std::string address);
        void SetPort(int port);
        void SetEnabled(bool enabled);
        void SetConnected(bool connected);
        void ReadAndSetInfo();

        private:
        void createHorizontalReadout(std::string header, Label readout);
        Panel readouts;
        Label
            address,
            port,
            connected;

        Button toggleButton;
    };

    /**
     * Panel where configuration can be viewed
     */
    class ConfigPanel : public Widget {
        public:
        ConfigPanel() {};
        ConfigPanel(std::string fileName) : ConfigPanel(XMLDocument(fileName)) {};
        ConfigPanel(XMLDocument doc);
        void LoadConfig(std::string fileName);
        void LoadConfig(XMLDocument file);
        void Clear();
        std::string GetConfigurationName() { return this->configNameString; };
        std::string GetConfigFile();

        private:
        Panel panel;
        Label
            header,
            fileLabel,
            PreProcessorLabel,
            TargetLabel,
            UDPAddressLabel,
            UDPPortLabel;

        std::string 
            configFile,
            configNameString;
    };

    /**
     * A panel that edits basic config properties.
     */
    class OverviewPanel : public Widget {
        public:
        OverviewPanel() {};
        OverviewPanel(XMLDocument doc);
        void SetConfigName(std::string name);
        std::string GetConfigName();
        void SetTargetInformationLabels(
            bool targetSpotted,
            int targetImgX,
            int targetImgY,
            int targetImgW,
            int targetImgH,
            double targetDist,
            double targetHAngle,
            double targetVAngle
        );
        void SetCameraIndex(int index);
        int GetCameraIndex();
        void SetUDPAddr(std::string addr);
        std::string GetUDPAddr();
        void SetUDPPort(int port);
        int GetUDPPort();
        void SetUDPEnabledLabels(bool UDPEnabled);
        void SetTargetInformationLabelsFromString(std::string iterOutput);

        //DEPRECATED
        [[deprecated("This method is not needed in KiwiLight and will be removed in the next update.")]] 
        void Update();


        private:
        TextBox configName;
        TextBox udpAddr;
        NumberBox udpPort;
        NumberBox cameraIndex;
        Button enableUDP;

        Label
            targetSpotted,
            targetImageLocation,
            targetSize,
            targetDist,
            targetHAngle,
            targetVAngle;
    };

    /**
     * A window where KiwiLight camera settings can be modified.
     */
    class Settings : public Widget {
        public:
        Settings() {};
        Settings(XMLDocument doc);
        void UpdateValue();
        void Show() { gtk_widget_show_all(this->widget); };
        XMLTag GetFinishedTag();
        void SetSettingValueFromID(int id, double value);
        double GetSettingValueFromID(int id);
        void SetCameraIndex(int index);
        int GetCameraIndex();
        std::vector<int> GetSettingIDs();

        //DEPRECATED
        [[deprecated("This method is unused and will be removed in the next update.")]] 
        void Update();


        private:
        NumberBox cameraIndex;
        std::vector<CameraSetting> settings;
    };

    /**
     * A panel that edits KiwiLight PreProcessors.
     */
    class PreprocessorEditor : public Widget {
        public:
        PreprocessorEditor() {};
        PreprocessorEditor(PreProcessor preprocessor);
        void Update();
        double GetProperty(PreProcessorProperty prop);
        void SetProperty(PreProcessorProperty prop, double value);
        GtkWidget *GetWidget() { return this->widget; };
        void SetName(std::string name);

        private:
        bool lastIsFull;
        CheckBox 
            isFull,
            isPartial;
        
        NumberBox 
            colorH,
            colorS,
            colorV;

        Image colorPreview;

        LabeledSlider 
            colorError,
            threshold,
            erosion,
            dilation;
        
        GtkWidget *widget;
    };

    /**
     * A panel that edits KiwiLight PostProcessors.
     */
    class PostprocessorEditor : public Widget {
        public:
        PostprocessorEditor() {};
        PostprocessorEditor(PostProcessor postprocessor);
        void Update();
        int GetNumContours();
        void SetNumContours(int contours);
        SettingPair GetProperty(int contour, TargetProperty prop);
        void SetProperty(int contour, TargetProperty prop, SettingPair value);

        private:
        int lastDesiredContour;
        
        NumberBox 
            contourchooser,
            distX,
            distY;

        LabeledSlider 
            distXErr,
            distYErr,
            angle,
            angleErr,
            ar,
            arErr,
            solidity,
            solidityErr,
            minimumArea;

        Label totalContours;

        Runner storageRunner;
    };

    /**
     * A panel that edits KiwiLight Runners.
     */
    class RunnerEditor : public Widget {
        public:
        RunnerEditor() {};
        RunnerEditor(Runner runner);
        void Update(int targetDistance);
        double GetProperty(RunnerProperty prop);
        void SetProperty(RunnerProperty prop, double value);
        std::string GetUDPAddr();
        int GetUDPPort();
        void SetUDPAddr(std::string newAddr);
        void SetUDPPort(int newPort);
        void SetUDPEnabledLabels(bool UDPEnabled);

        private:
        Label distanceLabel;

        TextBox udpAddress;
        NumberBox udpPort;

        Button enableUDP;

        CheckBox useHeight;

        LabeledSlider 
            offsetX,
            offsetY,
            imageWidth,
            imageHeight,
            targetTrueWidth,
            targetPercievedWidth,
            targetCalibratedDistance,
            targetErrorCorrection;
    };

    /**
     * A window editor that edits a KiwiLight configuration.
     */
    class ConfigEditor : public Widget {
        public:
        ConfigEditor() {};
        ConfigEditor(std::string fileName);
        void Update();
        bool UpdateImageOnly();
        std::string GetLastFrameResult();
        bool Save();
        void Close();
        void StartLearningTarget();
        void StartLearningDistance();
        void ReconnectUDPFromEditor();
        void SetUDPEnabledLabels(bool UDPEnabled);
        void ApplyCameraSettings();
        void SetCameraIndexBoxes(int index);
        void ReconnectUDPFromOverview();
        void OpenNewCameraFromOverview();
        std::string GetFileName() { return this->fileName; };
        cv::Mat GetOutputImage() { return this->out; };

        private:
        void UpdateImage();

        //universal config learning utility
        ConfigLearner learner;
        bool learnerActivated;

        //universal focal width learning utility
        TargetDistanceLearner distanceLearner;
        bool distanceLearnerRunning;

        TargetTroubleshooter troubleshooter;
        
        Label serviceMonitor;
        Label serviceLabel;

        //runtime things
        Runner runner;
        XMLDocument currentDoc;
        std::string 
                fileName,
                confName;
                
        std::string lastIterationResult;

        TabView tabs;
        OverviewPanel configOverview;
        Settings cameraSettings;
        PreprocessorEditor preprocessorSettings;
        PostprocessorEditor postprocessorSettings;
        RunnerEditor runnerSettings;

        cv::Mat out,
                original;
        Image outputImage;

        Window window;
        Panel content;
    };
}

#endif

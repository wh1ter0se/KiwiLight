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

    enum ImageColorspace {
        RGB
    };

    /**
     * Represents a generic UI widget.
     */
    class Widget {
        public:
        virtual GtkWidget *GetWidget() = 0;
        virtual void SetName(std::string name) = 0;
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
        void Show() { gtk_widget_show_all(this->panel); };
        GtkWidget *GetWidget() { return panel; };
        void SetName(std::string name);

        private:
        GtkWidget *panel;
    };


    class Scrollable : public Widget {
        public:
        Scrollable() {};
        Scrollable(bool horizontal, bool vertical);
        void PackWidget(GtkWidget *wid);
        GtkWidget *GetWidget() { return this->scrollable; };
        void SetName(std::string name);

        private:
        GtkWidget *scrollable;
    };


    class Frame : public Widget {
        public:
        Frame() {};
        Frame(std::string label);
        void Pack(GtkWidget *widget);
        void Unpack(GtkWidget *widget);
        GtkWidget *GetWidget() { return this->frame; };
        void SetName(std::string name);

        private:
        GtkWidget *frame;
    };

    /**
     * Represents a UI window that contains a form, or what have you
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
        GtkWidget *GetWidget() { return this->window; };
        void SetName(std::string name);
        
        private:
        GtkWidget *window;
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
        void SetName(std::string name);
        void SetLineWrap(bool enabled);
        void SetJustify(int justify);
        GtkWidget *GetWidget() { return this->label; };

        static const int JUSTIFY_LEFT = 0,
                          JUSTIFY_RIGHT = 1,
                          JUSTIFY_CENTER = 2;

        private:
        std::string text;
        std::string font;
        double font_size;
        GtkWidget *label;
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
        GtkWidget *GetWidget() { return this->textbox; };
        void SetName(std::string name);

        private:
        GtkWidget *textbox;
    };


    class NumberBox : public Widget {
        public:
        NumberBox() {};
        NumberBox(double min, double max, double value);
        NumberBox(double min, double max, double step, double value);
        void SetValue(double value);
        double GetValue();
        GtkWidget *GetWidget() { return this->numberBox; };
        void SetName(std::string name);

        private:
        double min,
            max;
        
        GtkWidget *numberBox;
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
        GtkWidget *GetWidget() { return this->checkbox; };
        void SetName(std::string name);

        private:
        GtkWidget *checkbox;
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
        GtkWidget *GetWidget() { return this->slider; };
        void SetName(std::string name);

        private:
        GtkWidget *slider;
    };

    /**
     * A faint line that can be placed between to objects in a container
     */
    class Separator : public Widget {
        public:
        Separator(bool horizontal);
        GtkWidget *GetWidget() { return this->separator; };
        void SetName(std::string name);

        private:
        GtkWidget *separator;
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
        void SetCallback( void(*callback)() );
        GtkWidget *GetWidget() { return this->button; };
        void SetName(std::string name);

        private:
        std::string text;
        GtkWidget *button;
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
        GtkWidget *GetWidget() { return this->dialog; };
        void SetName(std::string name);

        private:
        Panel content;
        GtkWidget *dialog;
    };

    /**
    * An option that would show under a menu bar item.
    */
    class SubMenuItem : public Widget {
        public:
        SubMenuItem(std::string name, void(*callback)());
        GtkWidget *GetWidget() { return this->submenuitem; };
        void SetName(std::string name);

        private:
        GtkWidget *submenuitem;
    };

    /**
     * A menu item able to be seen on a menu bar(ex "file", "help", etc)
     */
    class MenuItem : public Widget {
        public:
        MenuItem(std::string name);
        void AddSubmenuItem(SubMenuItem item);
        GtkWidget *GetWidget() { return this->menuitem; };
        void SetName(std::string name);

        private:
        GtkWidget *menuitem;
        GtkWidget *menu;
    };

    /**
     * A menu bar that would show up at the top of the screen.
     */
    class MenuBar : public Widget {
        public:
        MenuBar();
        void AddItem(MenuItem item);
        GtkWidget *GetWidget() { return this->menubar; };
        void SetName(std::string name);

        private:
        GtkWidget *menubar;
    };

    class Image : public Widget {
        public:
        Image() { this->declared = false; };
        Image(std::string fileName);
        Image(ImageColorspace colorspace);
        bool Declared() { return this->declared; };
        void Update(cv::Mat newImage);
        GtkWidget *GetWidget() { return this->image; };
        void SetName(std::string name);

        private:
        bool declared;
        bool declaredAsStaticImage;
        GtkWidget *image;
        ImageColorspace colorspace;

        //for one time init without updates
        Mat originalImage;
    };

    class FileChooser : public Widget {
        public:
        FileChooser() {};
        FileChooser(bool writing, std::string defaultFileName);
        std::string Show();
        GtkWidget *GetWidget() { return this->filechooser; };
        void SetName(std::string name);

        private:
        GtkWidget *filechooser;
    };


    class TabView : public Widget {
        public:
        TabView() {};
        TabView(std::string tab1Name, GtkWidget *tab1Content);
        void AddTab(std::string tabName, GtkWidget* tabContent);
        GtkWidget *GetWidget() { return this->tabview; };
        void SetName(std::string name);

        private:
        GtkWidget *tabview;
    };


    class PopupTextBox : public Widget {
        public:
        PopupTextBox() {};
        PopupTextBox(std::string name, std::string prompt, std::string initValue);
        std::string Show();
        GtkWidget *GetWidget() { return this->popuptextbox; };
        void SetName(std::string name);

        private:
        GtkWidget *popuptextbox;
        TextBox textbox;
    };

    class LabeledSlider : public Widget {
        public:
        LabeledSlider() {};
        LabeledSlider(std::string label, double min, double max, double step, double value);
        LabeledSlider(bool horizontal, std::string label, double min, double max, double step, double value);
        double GetValue();
        void SetValue(double value);
        void SetLabel(std::string text);
        GtkWidget *GetWidget() { return this->labeledslider; };
        void SetName(std::string name);

        private:
        Panel panel;
        Label label;
        Slider slider;
        GtkWidget *labeledslider;
    };


    class AboutWindow : public Widget {
        public:
        AboutWindow() {};
        AboutWindow(GtkWindowType type);
        void Show();
        GtkWidget *GetWidget() { return this->aboutwindow; };
        void SetName(std::string name);

        private:
        Window window;
        GtkWidget *aboutwindow;
    };


    class HelpWindow : public Widget {
        public:
        HelpWindow() {};
        HelpWindow(GtkWindowType type);
        void Show();
        GtkWidget *GetWidget() { return this->helpwindow; };
        void SetName(std::string name);

        private:
        Window window;
        GtkWidget *helpwindow;
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
        GtkWidget *GetWidget() { return this->cronwindow; };
        void SetName(std::string name);

        private:
        bool isCurrentFileAutomatic();
        std::vector<std::string> ReadAllRules();
        std::vector<std::string> ReadKiwiLightRules();

        Window window;
        Label 
            cronStatus,
            currentRuleStatus;

        bool isOpened;

        GtkWidget *cronwindow;
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
        GtkWidget *GetWidget() { return this->logviewer; };
        void SetName(std::string name);

        private:
        void createHorizontalReadout(std::string header, Label readout, bool isBig);
        Image generateGraph(long elapsedTime, int maxFPS, int maxDist, LogEvent *events, int numEvents);
        LogEvent eventFromTag(XMLTag tag);
        std::string timeFromMS(long ms);

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

        GtkWidget *logviewer;
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
        GtkWidget *GetWidget() { return this->camerasetting; };
        void SetName(std::string name);

        private:
        GtkWidget *camerasetting;

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
        std::string GetConfigFile() { return this->configFile; };
        GtkWidget *GetWidget() { return this->configPanel; };
        void SetName(std::string name);

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

        GtkWidget *configPanel;
    };

    class OverviewPanel : public Widget {
        public:
        OverviewPanel() {};
        OverviewPanel(XMLDocument doc);
        void Update();
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
        GtkWidget *GetWidget() { return overviewpanel; };
        void SetName(std::string name);

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

        GtkWidget *overviewpanel;
    };

    /**
     * A window where camera settings can be modified.
     */
    class Settings : public Widget {
        public:
        Settings() {};
        Settings(XMLDocument doc);
        void Update();
        void UpdateValue();
        void Show() { gtk_widget_show_all(this->settingsWidget); };
        XMLTag GetFinishedTag();
        void SetSettingValueFromID(int id, double value);
        double GetSettingValueFromID(int id);
        void SetCameraIndex(int index);
        int GetCameraIndex();
        std::vector<int> GetSettingIDs();
        GtkWidget *GetWidget() { return settingsWidget; };
        void SetName(std::string name);

        private:
        NumberBox cameraIndex;
        std::vector<CameraSetting> settings;
        GtkWidget *settingsWidget;
    };

    
    class PreprocessorEditor : public Widget {
        public:
        PreprocessorEditor() {};
        PreprocessorEditor(PreProcessor preprocessor);
        void Update();
        double GetProperty(PreProcessorProperty prop);
        void SetProperty(PreProcessorProperty prop, double value);
        GtkWidget *GetWidget() { return this->preprocessoreditor; };
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
        
        GtkWidget *preprocessoreditor;
    };


    class PostprocessorEditor : public Widget {
        public:
        PostprocessorEditor() {};
        PostprocessorEditor(PostProcessor postprocessor);
        void Update();
        int GetNumContours();
        void SetNumContours(int contours);
        SettingPair GetProperty(int contour, TargetProperty prop);
        void SetProperty(int contour, TargetProperty prop, SettingPair value);
        GtkWidget *GetWidget() { return this->postprocessoreditor; };
        void SetName(std::string name);

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

        GtkWidget *postprocessoreditor;
    };


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
        GtkWidget *GetWidget() { return this->runnereditor; };
        void SetName(std::string name);

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

        GtkWidget *runnereditor;
    };

    class ConfigEditor : public Widget {
        public:
        ConfigEditor() {};
        ConfigEditor(std::string fileName);
        void Update();
        bool UpdateImageOnly();
        std::string GetLastFrameResult();
        void Save();
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
        GtkWidget *GetWidget() { return this->configeditor; };
        void SetName(std::string name);

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

        GtkWidget *configeditor;
    };
}

#endif

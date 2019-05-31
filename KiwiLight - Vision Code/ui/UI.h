#ifndef KiwiLight_UI_H
#define KiwiLight_UI_H

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
        static void(*timeoutMethod)();
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
        bool ShowAndGetResponse();
        GtkWidget *GetWidget() { return this->dialog; };
        void SetName(std::string name);

        private:
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

    /**
     * An image frame which can display images.
     */
    class ImageFrame: public Widget {
        public:
        ImageFrame();
        ImageFrame(Image img);
        void SetConstantResolution(int width, int height);
        void SetHasConstantResolution(bool hasConstantResolution);
        void Update(Image img);
        GtkWidget *GetWidget() { return this->imgFrame; };
        void SetName(std::string name);

        private:
        bool hasConstantResolution;
        int constantWidth,
            constantHeight;

        GtkWidget *imgFrame;
    };

    class FileChooser : public Widget {
        public:
        FileChooser() {};
        FileChooser(bool writing);
        std::string Show();
        GtkWidget *GetWidget() { return this->filechooser; };
        void SetName(std::string name);

        private:
        GtkWidget *filechooser;
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

    /**
     * A singlular widget adjusting a camera setting
     */
    class CameraSetting : public Widget {
        public:
        CameraSetting() {};
        CameraSetting(std::string name, int min, int max, int value);
        int GetValue();
        void SetValue(int newValue);
        void Destroy();
        std::string GetName() { return name; };
        GtkWidget *GetWidget() { return this->camerasetting; };
        void SetName(std::string name);

        private:
        GtkWidget *camerasetting;

        Label  nameLabel;
        NumberBox input;

        std::string name,
                    type;

        int min, 
            max,
            value;
    };

    /**
     * A window where camera settings can be modified.
     */
    class Settings : public Widget {
        public:
        Settings() {};
        Settings(int index);
        void Update();
        void UpdateValue();
        void Show() { gtk_widget_show_all(this->settingsWidget); };
        int GetWidth() { return camWidth; };
        int GetHeight() { return camHeight; };
        XMLTag GetFinishedTag();
        GtkWidget *GetWidget() { return settingsWidget; };
        void SetName(std::string name);

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


    class ConfigRunnerEditor : public Widget {
        public:
        ConfigRunnerEditor() {};
        ConfigRunnerEditor(std::string fileName);
        void Update(cv::Mat originalImage, cv::Mat processedImage, double targetDistance);
        double GetProperty(RunnerProperty prop);
        ConfigurationSettingsList GetSettings();
        std::string GetUDPAddress();
        int GetUDPPort();
        GtkWidget *GetWidget() { return this->configrunnereditor; };
        void SetName(std::string name);

        private:
        Panel panel;

        Label distToTarget,
              imageTooBig;

        TextBox UDPAddr,
                UDPPort;

        //sliders used to adjust image resize
        LabeledSlider imageResizeX,
                      imageResizeY;

        //sliders used to adjust distance settings
        LabeledSlider TargetWidth,
                      TargetFocalWidth,
                      TargetDistErrCorrect,
                      TargetCalibratedDistance;

        Button tuneDistance,
               saveAndExit;

        ImageFrame outputImages;

        GtkWidget *configrunnereditor;
    };

    class ConfigTargetEditor : public Widget {
        public:
        ConfigTargetEditor() {};
        ConfigTargetEditor(std::string fileName, Runner runner);
        void Update();
        int NumContours();
        double GetPreProcessorProperty(PreProcessorProperty prop);
        SettingPair GetTargetPropertyValue(int contour, TargetProperty property);
        GtkWidget *GetWidget() { return this->configtargeteditor; };
        void SetName(std::string name);

        private:
        void SetValues(int contourID);

        int lastRequestedContour; //last recorded number in the contours box
        PreProcessorType currentPreProcessor;
        Runner runner;

        Panel panel;

        CheckBox fullPreProcessor,
                 partialPreProcessor;

        NumberBox colorH,
                  colorS,
                  colorV;

        LabeledSlider colorError;

        LabeledSlider thresholdValue,
                      dilationFactor;

        NumberBox ContourID;
        
        NumberBox ContourDistX,
                      ContourDistY;

        //sliders used to adjust contour distance from center of target
        LabeledSlider ContourDistXErr,
                      ContourDistYErr;

        //sliders used to adjust contour shape and other properties
        LabeledSlider ContourAngle,
                      ContourAngleErr,
                      ContourSolidity,
                      ContourSolidityErr,
                      ContourAR,
                      ContourARErr,
                      ContourMinArea;
            
        Button learnTarget;
        
        GtkWidget *configtargeteditor;
    };


    class ConfigCameraEditor : public Widget {
        public:
        ConfigCameraEditor() {};
        ConfigCameraEditor(int index);
        void Update();
        void StartApply();
        GtkWidget *GetWidget() { return this->configcameraeditor; };
        void SetName(std::string name);

        private:
        Panel panel;
        Slider camExposure,
               camWhiteBalance,
               camBrightness;

        CheckBox autoExp,
                 autoWB;

        GtkWidget *configcameraeditor;
    };


    enum EditorMode {
        USE_RUNNER,
        USE_LEARNER
    };


    class ConfigEditor : public Widget {
        public:
        ConfigEditor() {};
        ConfigEditor(std::string fileName);
        void Update();
        void Save();
        void Close();
        std::string GetFileName() { return this->fileName; };
        cv::Mat GetOutputImage() { return this->out; };
        GtkWidget *GetWidget() { return this->configeditor; };
        void SetName(std::string name);

        private:
        Runner runner;
        ConfigLearner learner;
        EditorMode editorMode;
        XMLDocument currentDoc;
        std::string fileName;
        std::string confName;

        cv::Mat out;

        Window window;
        Panel content;
        Panel buttonPanel;
        Settings cameraSettings;
        ConfigTargetEditor targetEditor;
        ConfigRunnerEditor runnerEditor;
        GtkWidget *configeditor;
    };
}

#endif
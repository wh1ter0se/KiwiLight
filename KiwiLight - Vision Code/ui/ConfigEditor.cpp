#include "UI.h"

/**
 * Source file for the ConfigEditor class.
 * Written By: Brach Knutson
 */

using namespace cv;
using namespace KiwiLight;

ConfigLearner ConfigEditor::learner = ConfigLearner();
ExampleTarget ConfigEditor::learnerResult = ExampleTarget();
int ConfigEditor::learnerMinArea = 0;

TargetDistanceLearner ConfigEditor::distLearner = TargetDistanceLearner();
double ConfigEditor::distResult = 0;
double ConfigEditor::targetTrueDistance = 0;
double ConfigEditor::targetTrueWidth = 0;

TargetTroubleshooter ConfigEditor::troubleshooter = TargetTroubleshooter();
TroubleshootingData ConfigEditor::troubleData[0];

/**
 * Creates a window to edit the bassed file.
 */
ConfigEditor::ConfigEditor(std::string fileName, VideoCapture cap) {
    this->monitorLearner = false;
    this->monitorDistanceLearner = false;
    this->monitorTroubleshooter = false;
    this->runner = Runner(fileName, true, cap);
    this->currentDoc = XMLDocument(fileName);
    this->fileName = fileName;
    this->out = Mat(Size(50, 50), CV_8UC3);
    this->confName = this->currentDoc.GetTagsByName("configuration")[0].GetAttributesByName("name")[0].Value();

    this->window = Window(GTK_WINDOW_TOPLEVEL, false);
        this->content = Panel(true, 0);

            Panel cameraSettingsPanel = Panel(false, 0);
                Label cameraSettingsHeader = Label("Camera Settings");
                    cameraSettingsHeader.SetName("header");
                    cameraSettingsPanel.Pack_start(cameraSettingsHeader.GetWidget(), true, true, 0);

                this->cameraSettings = Settings(0, cap);
                    cameraSettingsPanel.Pack_start(this->cameraSettings.GetWidget(), true, true, 0);

            Panel preprocessorSettingsPanel = Panel(false, 0);
                Label preprocessorSettingsPanelHeader = Label("Preprocessor");
                    preprocessorSettingsPanelHeader.SetName("header");
                    preprocessorSettingsPanel.Pack_start(preprocessorSettingsPanelHeader.GetWidget(), true, true, 0);
                
                this->preprocessorSettings = PreprocessorEditor(this->runner.GetPreProcessor());
                    preprocessorSettingsPanel.Pack_start(this->preprocessorSettings.GetWidget(), true, true, 0);

            Panel postprocessorSettingsPanel = Panel(false, 0);
                Label postprocessorSettingsPanelHeader = Label("Postprocessor");
                    postprocessorSettingsPanelHeader.SetName("header");
                    postprocessorSettingsPanel.Pack_start(postprocessorSettingsPanelHeader.GetWidget(), true, true, 0);
                
                this->postprocessorSettings = PostprocessorEditor(this->runner.GetPostProcessor());
                    postprocessorSettingsPanel.Pack_start(this->postprocessorSettings.GetWidget(), true, true, 0);

            Panel runnerSettingsPanel = Panel(false, 0);
                Label runnerSettingsPanelHeader = Label("Runner");
                    runnerSettingsPanelHeader.SetName("header");
                    runnerSettingsPanel.Pack_start(runnerSettingsPanelHeader.GetWidget(), true, true, 0);

                this->runnerSettings = RunnerEditor(this->runner);
                    runnerSettingsPanel.Pack_start(this->runnerSettings.GetWidget(), true, true, 0);

            this->tabs = TabView("Camera", cameraSettingsPanel.GetWidget());
            this->tabs.AddTab("Preprocessor", preprocessorSettingsPanel.GetWidget());
            this->tabs.AddTab("Postprocessor", postprocessorSettingsPanel.GetWidget());
            this->tabs.AddTab("Runner", runnerSettingsPanel.GetWidget());
            this->content.Pack_start(this->tabs.GetWidget(), true, true, 0);
        this->window.SetPane(this->content);
    
    this->window.SetCSS("ui/Style.css");
    this->window.Show();
}

/**
 * Updates the editor and checks for button presses, etc.
 */
void ConfigEditor::Update() {
    
}


void ConfigEditor::UpdateImageOnly() {
    
}

/**
 * Causes the editor to save the config to file.
 */
void ConfigEditor::Save() {
    
}


void ConfigEditor::Close() {
    gtk_widget_destroy(this->configeditor);
}


void ConfigEditor::SetUDPEnabled(bool enabled) {
    this->runner.SetUDPEnabled(enabled);
}


bool ConfigEditor::GetUDPEnabled() {
    return this->runner.GetUDPEnabled();
}


void ConfigEditor::ResetRunnerResolution() {
    int camResX = this->cameraSettings.GetWidth();
    int camResY = this->cameraSettings.GetHeight();
    Size newRes = Size(camResX, camResY);
    this->runner.SetResolution(newRes);
}


void ConfigEditor::SetName(std::string name) {
    gtk_widget_set_name(this->configeditor, name.c_str());
}

void ConfigEditor::LearnTarget() {
    ConfigEditor::learnerResult = ConfigEditor::learner.LearnTarget(ConfigEditor::learnerMinArea);
    Flags::RaiseFlag("ConfigLearnerDone");
    g_thread_exit(0);
}

void ConfigEditor::LearnDistance() {
    ConfigEditor::distResult = ConfigEditor::distLearner.LearnFocalWidth(ConfigEditor::targetTrueWidth, targetTrueDistance);
    Flags::RaiseFlag("DistanceLearnerDone");
    g_thread_exit(0);
}

void ConfigEditor::TroubleshootTarget() {
    ConfigEditor::troubleshooter.Troubleshoot(ConfigEditor::troubleData);
    Flags::RaiseFlag("TargetTroubleshooterDone");
    g_thread_exit(0);
}
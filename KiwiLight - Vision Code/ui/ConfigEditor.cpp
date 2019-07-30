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
            Panel overviewPanel = Panel(false, 0);

                this->configOverview = ConfigPanel(this->currentDoc, false);
                    overviewPanel.Pack_start(this->configOverview.GetWidget(), true ,true, 0);

            Panel cameraSettingsPanel = Panel(false, 0);
                Label cameraSettingsHeader = Label("Camera Settings");
                    cameraSettingsHeader.SetName("header");
                    cameraSettingsPanel.Pack_start(cameraSettingsHeader.GetWidget(), true, true, 0);

                this->cameraSettings = Settings(0, cap);
                    cameraSettingsPanel.Pack_start(this->cameraSettings.GetWidget(), true, false, 0);

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

            this->tabs = TabView("Overview", overviewPanel.GetWidget());
            this->tabs.AddTab("Camera", cameraSettingsPanel.GetWidget());
            this->tabs.AddTab("Preprocessor", preprocessorSettingsPanel.GetWidget());
            this->tabs.AddTab("Postprocessor", postprocessorSettingsPanel.GetWidget());
            this->tabs.AddTab("Runner", runnerSettingsPanel.GetWidget());
            this->content.Pack_start(this->tabs.GetWidget(), true, true, 0);

            this->outputImage = Image(ImageColorspace::RGB);
                this->content.Pack_start(this->outputImage.GetWidget(), false, false, 0);

        this->window.SetPane(this->content);
    
    this->window.SetCSS("ui/Style.css");
    this->window.Show();
}

/**
 * Updates the editor and checks for button presses, etc.
 */
void ConfigEditor::Update() {
    this->runner.Iterate();
    Mat original = this->runner.GetOriginalImage();
    original.convertTo(original, CV_8UC3);
    Mat displayable;

    try {
        vconcat(original, this->out, displayable);
        this->outputImage.Update(displayable);
    } catch(cv::Exception ex) {
        std::cout << "cv exception in ce" << std::endl;
    }

    this->preprocessorSettings.Update();
    this->postprocessorSettings.Update();
    this->runnerSettings.Update(this->runner.GetClosestTargetToCenter().Distance());

    //apply the preprocessor settings
    this->runner.SetPreprocessorProperty(PreProcessorProperty::IS_FULL, this->preprocessorSettings.GetProperty(PreProcessorProperty::IS_FULL));
    this->runner.SetPreprocessorProperty(PreProcessorProperty::THRESHOLD, this->preprocessorSettings.GetProperty(PreProcessorProperty::THRESHOLD));
    this->runner.SetPreprocessorProperty(PreProcessorProperty::EROSION, this->preprocessorSettings.GetProperty(PreProcessorProperty::EROSION));
    this->runner.SetPreprocessorProperty(PreProcessorProperty::DILATION, this->preprocessorSettings.GetProperty(PreProcessorProperty::DILATION));
    this->runner.SetPreprocessorProperty(PreProcessorProperty::COLOR_HUE, this->preprocessorSettings.GetProperty(PreProcessorProperty::COLOR_HUE));
    this->runner.SetPreprocessorProperty(PreProcessorProperty::COLOR_SATURATION, this->preprocessorSettings.GetProperty(PreProcessorProperty::COLOR_SATURATION));
    this->runner.SetPreprocessorProperty(PreProcessorProperty::COLOR_VALUE, this->preprocessorSettings.GetProperty(PreProcessorProperty::COLOR_VALUE));
    this->runner.SetPreprocessorProperty(PreProcessorProperty::COLOR_ERROR, this->preprocessorSettings.GetProperty(PreProcessorProperty::COLOR_ERROR));

    //apply all contour settings to the runner
    for(int i=0; i<this->postprocessorSettings.GetNumContours(); i++) {
        this->runner.SetPostProcessorContourProperty(i, TargetProperty::DIST_X, this->postprocessorSettings.GetProperty(i, TargetProperty::DIST_X));
        this->runner.SetPostProcessorContourProperty(i, TargetProperty::DIST_Y, this->postprocessorSettings.GetProperty(i, TargetProperty::DIST_Y));
        this->runner.SetPostProcessorContourProperty(i, TargetProperty::ANGLE, this->postprocessorSettings.GetProperty(i, TargetProperty::ANGLE));
        this->runner.SetPostProcessorContourProperty(i, TargetProperty::ASPECT_RATIO, this->postprocessorSettings.GetProperty(i, TargetProperty::ASPECT_RATIO));
        this->runner.SetPostProcessorContourProperty(i, TargetProperty::SOLIDITY, this->postprocessorSettings.GetProperty(i, TargetProperty::SOLIDITY));
        this->runner.SetPostProcessorContourProperty(i, TargetProperty::MINIMUM_AREA, this->postprocessorSettings.GetProperty(i, TargetProperty::MINIMUM_AREA));
    }

    //apply runner properties
    this->runner.SetRunnerProperty(RunnerProperty::OFFSET_X, this->runnerSettings.GetProperty(RunnerProperty::OFFSET_X));
    this->runner.SetRunnerProperty(RunnerProperty::OFFSET_Y, this->runnerSettings.GetProperty(RunnerProperty::OFFSET_Y));
    this->runner.SetRunnerProperty(RunnerProperty::IMAGE_WIDTH, this->runnerSettings.GetProperty(RunnerProperty::IMAGE_WIDTH));
    this->runner.SetRunnerProperty(RunnerProperty::IMAGE_HEIGHT, this->runnerSettings.GetProperty(RunnerProperty::IMAGE_HEIGHT));
    this->runner.SetRunnerProperty(RunnerProperty::TRUE_WIDTH, this->runnerSettings.GetProperty(RunnerProperty::TRUE_WIDTH));
    this->runner.SetRunnerProperty(RunnerProperty::PERCEIVED_WIDTH, this->runnerSettings.GetProperty(RunnerProperty::PERCEIVED_WIDTH));
    this->runner.SetRunnerProperty(RunnerProperty::CALIBRATED_DISTANCE, this->runnerSettings.GetProperty(RunnerProperty::CALIBRATED_DISTANCE));
    this->runner.SetRunnerProperty(RunnerProperty::ERROR_CORRECTION, this->runnerSettings.GetProperty(RunnerProperty::ERROR_CORRECTION));
}

/**
 * Updates the internal runner to in turn update the output images.
 */
void ConfigEditor::UpdateImageOnly() {
    this->runner.Iterate();
    this->out = this->runner.GetOutputImage();
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
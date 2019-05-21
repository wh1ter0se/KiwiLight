#include "UI.h"

/**
 * Source file for the ConfigEditor class.
 * Written By: Brach Knutson
 */

using namespace cv;
using namespace KiwiLight;

/**
 * Creates a window to edit the bassed file.
 */
ConfigEditor::ConfigEditor(std::string fileName) {
    this->runner = Runner(fileName, true);
    this->editorMode = EditorMode::USE_RUNNER;
    this->currentDoc = XMLDocument(fileName);

    this->window = Window(GTK_WINDOW_TOPLEVEL);
        this->content = Panel(true, 5);
                Panel settingsContent = Panel(false, 0);
                    Label settingsHeader = Label("Camera Settings");
                        settingsHeader.SetName("header");
                        settingsContent.Pack_start(settingsHeader.GetWidget(), false, false, 0);
                    this->cameraSettings = Settings(0);
                        settingsContent.Pack_start(this->cameraSettings.GetWidget(), false, false, 0);
                    this->content.Pack_start(settingsContent.GetWidget(), true, true, 0);

                Panel targetContent = Panel(false, 0);
                    Label targetHeader = Label("Targeting");
                        targetHeader.SetName("header");
                        targetContent.Pack_start(targetHeader.GetWidget(), false, false, 0);
                    this->targetEditor = ConfigTargetEditor(fileName, this->runner);
                        targetContent.Pack_start(this->targetEditor.GetWidget(), false, false, 0);
                    this->content.Pack_start(targetContent.GetWidget(), true, true, 0);

                Panel runnerContent = Panel(false, 0);
                    Label runnerHeader = Label("Runner");
                        runnerHeader.SetName("header");
                        runnerContent.Pack_start(runnerHeader.GetWidget(), false, false, 0);
                    this->runnerEditor = ConfigRunnerEditor(fileName);
                        runnerContent.Pack_start(this->runnerEditor.GetWidget(), false, false, 0);
                    this->content.Pack_start(runnerContent.GetWidget(), true, true, 0);

            this->window.SetPane(this->content);

    this->window.Show();
    this->configeditor = this->window.GetWidget();
    this->Update();
}

/**
 * Updates the editor and checks for button presses, etc.
 */
void ConfigEditor::Update() {
    //update the trinity for its functionality
    this->cameraSettings.Update();
    this->targetEditor.Update();
    this->runnerEditor.Update();

    if(this->editorMode == EditorMode::USE_RUNNER) {
        this->runner.Iterate();
        this->out = this->runner.GetOutputImage();
    } else if(this->editorMode == EditorMode::USE_LEARNER) {
        this->learner.Update();
        this->out = this->learner.GetOutputImage();
    }

    // //check for flag signals
    if(Flags::GetFlag("StartLearner")) {
        Flags::LowerFlag("StartLearner");
        
        //create a new learner and then switch to learner mode
        XMLTag preprocessorTag = this->currentDoc.GetTagsByName("configuration")[0].GetTagsByName("preprocessor")[0];
        this->learner = ConfigLearner(preprocessorTag, this->runner.GetVideoStream());
        this->editorMode = EditorMode::USE_LEARNER;
    }

    if(Flags::GetFlag("StopLearner")) {
        Flags::LowerFlag("StopLearner");
        this->editorMode = EditorMode::USE_RUNNER;
    }
}

/**
 * Causes the editor to save the config to file.
 */
void ConfigEditor::Save() {
    XMLDocument newDoc = XMLDocument();

    std::cout << "iiii" << std::endl;
    std::cout.flush();
}


void ConfigEditor::SetName(std::string name) {
    gtk_widget_set_name(this->configeditor, name.c_str());
}


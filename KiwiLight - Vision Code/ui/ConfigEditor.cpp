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
    
    if(this->editorMode == EditorMode::USE_RUNNER) {
        //set all runner preprocessor settings to the values in the target editor
        this->runner.SetPreprocessorProperty(PreProcessorProperty::THRESHOLD, this->targetEditor.GetPreProcessorProperty(PreProcessorProperty::THRESHOLD));
        this->runner.SetPreprocessorProperty(PreProcessorProperty::THRESH_VALUE, this->targetEditor.GetPreProcessorProperty(PreProcessorProperty::THRESH_VALUE));
        this->runner.SetPreprocessorProperty(PreProcessorProperty::DILATION, this->targetEditor.GetPreProcessorProperty(PreProcessorProperty::DILATION));
        this->runner.SetPreprocessorProperty(PreProcessorProperty::COLOR_HUE, this->targetEditor.GetPreProcessorProperty(PreProcessorProperty::DILATION));
        this->runner.SetPreprocessorProperty(PreProcessorProperty::COLOR_SATURATION, this->targetEditor.GetPreProcessorProperty(PreProcessorProperty::COLOR_SATURATION));
        this->runner.SetPreprocessorProperty(PreProcessorProperty::COLOR_VALUE, this->targetEditor.GetPreProcessorProperty(PreProcessorProperty::COLOR_VALUE));

        //set all runner targeting settings to the ones in the target editor
        for(int i=0; i<this->targetEditor.NumContours(); i++) {
            this->runner.SetPostProcessorContourProperty(i, TargetProperty::DIST_X, this->targetEditor.GetTargetPropertyValue(i, TargetProperty::DIST_X));
            this->runner.SetPostProcessorContourProperty(i, TargetProperty::DIST_Y, this->targetEditor.GetTargetPropertyValue(i, TargetProperty::DIST_Y));
            this->runner.SetPostProcessorContourProperty(i, TargetProperty::ANGLE, this->targetEditor.GetTargetPropertyValue(i, TargetProperty::ANGLE));
            this->runner.SetPostProcessorContourProperty(i, TargetProperty::ASPECT_RATIO, this->targetEditor.GetTargetPropertyValue(i, TargetProperty::ASPECT_RATIO));
            this->runner.SetPostProcessorContourProperty(i, TargetProperty::SOLIDITY, this->targetEditor.GetTargetPropertyValue(i, TargetProperty::SOLIDITY));
            this->runner.SetPostProcessorContourProperty(i, TargetProperty::MINIMUM_AREA, this->targetEditor.GetTargetPropertyValue(i, TargetProperty::MINIMUM_AREA));
        }

        this->runner.Iterate();
        this->out = this->runner.GetOutputImage();
    } else if(this->editorMode == EditorMode::USE_LEARNER) {
        /**
         * TODO:
         * Fix below 3 lines so minimumArea is returned as a settingPair, or add accessor for just minimumArea value.
         */

        // double minimumArea = this->targetEditor.GetTargetPropertyValue(0, TargetProperty::MINIMUM_AREA);
        this->learner.Update(500); //use minimumArea in above line instead of constant 500
        this->out = this->learner.GetOutputImage();
    }

    this->cameraSettings.Update();
    this->targetEditor.Update();
    this->runnerEditor.Update(this->runner.GetOriginalImage(), this->out);


    //update preprocessor settings in runner

    //update target settings in runner

    //update image resize settings in runner
    int newSizeX = (int) this->runnerEditor.GetProperty(RunnerProperty::IMAGE_WIDTH);
    int newSizeY = (int) this->runnerEditor.GetProperty(RunnerProperty::IMAGE_HEIGHT);
    Size newSize = Size(newSizeX, newSizeY);
    this->runner.SetImageResize(newSize);

    //update image distance info in runner

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


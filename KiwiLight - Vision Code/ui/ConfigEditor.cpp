#include "UI.h"

/**
 * Source file for the ConfigEditor class.
 * Written By: Brach Knutson
 */

using namespace cv;
using namespace KiwiLight;


static void PageNext() {
    Flags::RaiseFlag("EditorNextPage");
}

static void PagePrev() {
    Flags::RaiseFlag("EditorPrevPage");
}

/**
 * Creates a window to edit the bassed file.
 */
ConfigEditor::ConfigEditor(std::string fileName) {
    this->runner = Runner(fileName, true);
    this->currentFrame = ConfigEditorFrame::EDITOR_CAMERA;

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

    if(Flags::GetFlag("EditorNextPage")) {
        Flags::LowerFlag("EditorNextPage");
        this->NextPage();
    }
    if(Flags::GetFlag("EditorPrevPage")) {
        Flags::LowerFlag("EditorPrevPage");
        this->PrevPage();
    }

    this->runner.Iterate();
    this->out = this->runner.GetOutputImage();
}

/**
 * Causes the editor to save the config to file.
 */
void ConfigEditor::Save() {
    std::cout << "save" << std::endl;
}


void ConfigEditor::SetName(std::string name) {
    gtk_widget_set_name(this->configeditor, name.c_str());
}


/**
 * ORDER OF PAGES: camera - target - runner
 */

/**
 * Shifts to the next page
 */
void ConfigEditor::NextPage() {
    // switch(this->currentFrame) {
    //     case ConfigEditorFrame::EDITOR_CAMERA:
    //         std::cout << "yeet 2" << std::endl;
    //         this->currentFrame = ConfigEditorFrame::EDITOR_TARGET;
    //         this->contentFrame.Unpack(this->cameraSettings.GetWidget());
    //         this->contentFrame.Pack(this->targetEditor.GetWidget());
    //     break;

    //     case ConfigEditorFrame::EDITOR_TARGET:
    //         std::cout << "yeet 3" << std::endl;
    //         this->currentFrame = ConfigEditorFrame::EDITOR_RUNNER;
    //         this->contentFrame.Unpack(this->targetEditor.GetWidget());
    //         this->contentFrame.Pack(this->runnerEditor.GetWidget());
    //     break;
    // }
}

/**
 * Shifts to the previous page
 */
void ConfigEditor::PrevPage() {
    // switch(this->currentFrame) {
    //     case ConfigEditorFrame::EDITOR_RUNNER:
    //         std::cout << "yeet 2" << std::endl;
    //         this->currentFrame = ConfigEditorFrame::EDITOR_TARGET;
    //         this->contentFrame.Unpack(this->runnerEditor.GetWidget());
    //         this->contentFrame.Pack(this->targetEditor.GetWidget());
    //     break;

    //     case ConfigEditorFrame::EDITOR_TARGET:
    //         std::cout << "yeet 1" << std::endl;
    //         this->currentFrame = ConfigEditorFrame::EDITOR_CAMERA;
    //         this->contentFrame.Unpack(this->targetEditor.GetWidget());
    //         this->contentFrame.Pack(this->cameraSettings.GetWidget());
    //     break;
    // }
}
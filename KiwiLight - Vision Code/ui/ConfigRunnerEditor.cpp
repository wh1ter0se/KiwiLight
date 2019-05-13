#include "UI.h"

/**
 * Source file for the ConfigRunnerEditor class.
 * Written By: Brach Knutson
 */

using namespace cv;
using namespace KiwiLight;


ConfigRunnerEditor::ConfigRunnerEditor(std::string fileName) {
    this->panel = Panel(false, 0);
        Label lbl = Label("Config Runner Editor");
        this->panel.Pack_start(lbl.GetWidget(), true, false, 0);

    this->configrunnereditor = panel.GetWidget();
}


void ConfigRunnerEditor::Update() {

}


ConfigurationSettingsList ConfigRunnerEditor::GetSettings() {

}


void ConfigRunnerEditor::SetName(std::string name) {
    gtk_widget_set_name(this->configrunnereditor, name.c_str());
}
#include "UI.h"

/**
 * Source file for the ConfigTargetEditor class.
 * Written By: Brach Knutson
 */

using namespace cv;
using namespace KiwiLight;

/**
 * Creates a new ConfigTargetEditor editing the targets in the file at fileName
 */
ConfigTargetEditor::ConfigTargetEditor(std::string fileName) {
    this->panel = Panel(false, 0);
        this->ContourDistX = LabeledSlider("Distance from Center X (inches): ", 0.0, 100.0, 0.1, 0.0);

    this->configtargeteditor = this->panel.GetWidget();
}

/**
 * Updates the ConfigTargetEditor's values and the output image
 */
void ConfigTargetEditor::Update() {

}

/**
 * Returns an XMLTag describing the target being edited by this editor.
 */
XMLTag ConfigTargetEditor::GetTarget() {

}


void ConfigTargetEditor::SetName(std::string name) {
    gtk_widget_set_name(this->configtargeteditor, name.c_str());
}
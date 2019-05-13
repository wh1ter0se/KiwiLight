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

        //create editor for the preprocessor
        Panel preprocessorPanel = Panel(false, 0);
            Label preprocessorPanelHeader = Label("Preprocessor");
                preprocessorPanel.Pack_start(preprocessorPanelHeader.GetWidget(), false, false, 0);

            //chooser for whether or not the preprocessor is full or partial
            Panel preprocessorTypeChooser = Panel(true, 5);
                Label preprocessorTypeHeader = Label("Type:");
                    preprocessorTypeChooser.Pack_start(preprocessorTypeHeader.GetWidget(), false, false, 0);

                this->fullPreProcessor = CheckBox("Full", true);
                    preprocessorTypeChooser.Pack_start(this->fullPreProcessor.GetWidget(), false, false, 0);

                this->partialPreProcessor = CheckBox("Partial", false);
                    preprocessorTypeChooser.Pack_start(this->partialPreProcessor.GetWidget(), false, false, 0);

                preprocessorPanel.Pack_start(preprocessorTypeChooser.GetWidget(), false, false, 0);
            
            Label colorHeader = Label("Color");
                preprocessorPanel.Pack_start(colorHeader.GetWidget(), false, false, 0);

            Panel preprocessorColorChooser = Panel(true, 3);
                this->colorH = NumberBox(0.0, 100.0, 0.0);
                    preprocessorColorChooser.Pack_start(this->colorH.GetWidget(), false, false, 0);

                this->colorS = NumberBox(0.0, 255.0, 0.0);
                    preprocessorColorChooser.Pack_start(this->colorS.GetWidget(), false, false, 0);

                this->colorV = NumberBox(0.0, 255.0, 0.0);
                    preprocessorColorChooser.Pack_start(this->colorV.GetWidget(), false, false, 0);

                preprocessorPanel.Pack_start(preprocessorColorChooser.GetWidget(), false, false, 0);

            this->thresholdValue = LabeledSlider("Threshold Value:", 0.0, 255.0, 1.0, 0.0);
                preprocessorPanel.Pack_start(this->thresholdValue.GetWidget(), false, false, 0);

            this->dilationFactor = LabeledSlider("Dilation:", 1.0, 50.0, 0.5, 0.0);
                preprocessorPanel.Pack_start(this->dilationFactor.GetWidget(), false, false, 0);

            this->panel.Pack_start(preprocessorPanel.GetWidget(), false, false, 0);
                
    this->configtargeteditor = this->panel.GetWidget();
}

/**
 * Updates the ConfigTargetEditor's values and the output image
 */
void ConfigTargetEditor::Update() {
    // //check for invalid selections in preprocessor area (i.e both are checked, none are checked)
    if(this->fullPreProcessor.GetState() && this->partialPreProcessor.GetState()) {
        //if both are checked, switch to the unselected option
        if(this->currentPreProcessor == PreProcessorType::FULL) {
            this->fullPreProcessor.SetState(false);
            this->currentPreProcessor = PreProcessorType::PARTIAL;
        } else {
            this->partialPreProcessor.SetState(false);
            this->currentPreProcessor = PreProcessorType::FULL;
        }
    }

    if(!this->fullPreProcessor.GetState() && !this->partialPreProcessor.GetState()) {
        if(this->currentPreProcessor == PreProcessorType::FULL) {
            this->fullPreProcessor.SetState(true);
        } else {
            this->partialPreProcessor.SetState(true);
        }
    }
}

/**
 * Returns an XMLTag describing the target being edited by this editor.
 */
XMLTag ConfigTargetEditor::GetTarget() {

}


void ConfigTargetEditor::SetName(std::string name) {
    gtk_widget_set_name(this->configtargeteditor, name.c_str());
}
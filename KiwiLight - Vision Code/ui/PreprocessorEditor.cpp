#include "UI.h"

/**
 * Source file for the PreprocessorEditor class.
 * Written By: Brach Knuston
 */

using namespace KiwiLight;

/**
 * Creates a new PreProcessorEditor using the values from "preprocessor" as the inital values.
 */
PreprocessorEditor::PreprocessorEditor(PreProcessor preprocessor) {
    this->lastIsFull = preprocessor.GetProperty(PreProcessorProperty::IS_FULL) == 1.0;
    Panel editor = Panel(false, 0);

        Panel preprocessorTypePanel = Panel(false, 0);
            double processorIsFullDouble = preprocessor.GetProperty(PreProcessorProperty::IS_FULL);
            bool processorIsFullBool = (processorIsFullDouble == 1.0 ? true : false);

            Label preprocessorTypePanelHeader = Label("Type");
                preprocessorTypePanelHeader.SetName("subHeader");
                preprocessorTypePanel.Pack_start(preprocessorTypePanelHeader.GetWidget(), true, true, 0);

            Panel preprocessorPanelContents = Panel(true, 0);
                this->isFull = CheckBox("Full", processorIsFullBool);
                    preprocessorPanelContents.Pack_start(this->isFull.GetWidget(), false, false, 0);

                this->isPartial = CheckBox("Partial", !processorIsFullBool);
                    preprocessorPanelContents.Pack_start(this->isPartial.GetWidget(), false, false, 0);

                preprocessorTypePanel.Pack_start(preprocessorPanelContents.GetWidget(), true, true, 0);

            editor.Pack_start(preprocessorTypePanel.GetWidget(), true, true, 0);

        Panel targetColorPanel = Panel(false, 0);
            Label targetColorPanelHeader = Label("Target Color (HSV)");
                targetColorPanelHeader.SetName("subHeader");
                targetColorPanel.Pack_start(targetColorPanelHeader.GetWidget(), true, true, 0);

            Panel targetColorPanelContents = Panel(true, 0);
                double realColorH = preprocessor.GetProperty(PreProcessorProperty::COLOR_HUE);
                this->colorH = NumberBox(0, 100, realColorH);
                    targetColorPanelContents.Pack_start(this->colorH.GetWidget(), true, true, 0);

                double realColorS = preprocessor.GetProperty(PreProcessorProperty::COLOR_SATURATION);
                this->colorS = NumberBox(0, 255, realColorS);
                    targetColorPanelContents.Pack_start(this->colorS.GetWidget(), true, true, 0);

                double realColorV = preprocessor.GetProperty(PreProcessorProperty::COLOR_VALUE);
                this->colorV = NumberBox(0, 255, realColorV);
                    targetColorPanelContents.Pack_start(this->colorV.GetWidget(), true, true, 0);

                this->colorPreview = Image(ImageColorspace::RGB);
                    targetColorPanelContents.Pack_start(this->colorPreview.GetWidget(), false, false, 0);

                targetColorPanel.Pack_start(targetColorPanelContents.GetWidget(), true, true, 0);

            editor.Pack_start(targetColorPanel.GetWidget(), true, true, 0);

        Label imageProcLabel = Label("Image Processing");
            imageProcLabel.SetName("subHeader");
            editor.Pack_start(imageProcLabel.GetWidget(), true, true, 0);

        double realColorError = preprocessor.GetProperty(PreProcessorProperty::COLOR_ERROR);
        this->colorError = LabeledSlider("Color Range", 0.0, 255.0, 1.0, realColorError);
            editor.Pack_start(this->colorError.GetWidget(), true, true, 0);

        double realThreshonld = preprocessor.GetProperty(PreProcessorProperty::THRESHOLD);
        this->threshold = LabeledSlider("Threshold", 1.0, 100.0, 1.0, realThreshonld);
            editor.Pack_start(this->threshold.GetWidget(), true, true, 0);

        double realErosion = preprocessor.GetProperty(PreProcessorProperty::EROSION);
        this->erosion = LabeledSlider("Erosion", 1.0, 25.0, 1.0, realErosion);
            editor.Pack_start(this->erosion.GetWidget(), true, true, 0);

        double realDilation = preprocessor.GetProperty(PreProcessorProperty::DILATION);
        this->dilation = LabeledSlider("Dilation", 1.0, 25.0, 1.0, realDilation);
            editor.Pack_start(this->dilation.GetWidget(), true, true, 0);

    this->widget = editor.GetWidget();
}

/**
 * Updates the UI of the editor.
 */
void PreprocessorEditor::Update() {
    //set checkboxes to appropriate states based on which mode is selected.
    if(this->isFull.GetState() && this->isPartial.GetState()) {
        //both checkboxes checked, toggle intended
        bool intendedStateIsFull = !this->lastIsFull;

        if(intendedStateIsFull) {
            this->isFull.SetState(true);
            this->isPartial.SetState(false);
        } else {
            this->isFull.SetState(false);
            this->isPartial.SetState(true);
        }

        this->lastIsFull = intendedStateIsFull;
    }

    if(!this->isFull.GetState() && !this->isPartial.GetState()) {
        //if no checkboxes are checked just revert back to whatever the last state was
        if(this->lastIsFull) {
            this->isFull.SetState(true);
            this->isPartial.SetState(false);
        } else {
            this->isFull.SetState(false);
            this->isPartial.SetState(true);
        }
    }

    //update the color in the box to the desired color
    int hue        = (int) this->colorH.GetValue();
    int saturation = (int) this->colorS.GetValue();
    int value      = (int) this->colorV.GetValue();

    Mat previewImage = Mat(Size(30, 30), CV_8UC3, Scalar(hue, saturation, value));
    cvtColor(previewImage, previewImage, COLOR_HSV2BGR_FULL);
    this->colorPreview.Update(previewImage);
}

/**
 * Returns the value of the editor property
 */
double PreprocessorEditor::GetProperty(PreProcessorProperty prop) {
    switch(prop) {
        case PreProcessorProperty::IS_FULL:
            return (this->isFull.GetState() ? 1.0 : 0.0);
        case PreProcessorProperty::THRESHOLD:
            return this->threshold.GetValue();
        case PreProcessorProperty::EROSION:
            return this->erosion.GetValue();
        case PreProcessorProperty::DILATION:
            return this->dilation.GetValue();
        case PreProcessorProperty::COLOR_HUE:
            return this->colorH.GetValue();
        case PreProcessorProperty::COLOR_SATURATION:
            return this->colorS.GetValue();
        case PreProcessorProperty::COLOR_VALUE:
            return this->colorV.GetValue();
        case PreProcessorProperty::COLOR_ERROR:
            return this->colorError.GetValue();
    }
}

/**
 * Sets the editor property to the value "value"
 */
void PreprocessorEditor::SetProperty(PreProcessorProperty prop, double value) {
    switch(prop) {
        case PreProcessorProperty::IS_FULL:
            this->isFull.SetState(value == 1.0);
            break;
        case PreProcessorProperty::THRESHOLD:
            this->threshold.SetValue(value);
            break;
        case PreProcessorProperty::EROSION:
            this->erosion.SetValue(value);
            break;
        case PreProcessorProperty::DILATION:
            this->dilation.SetValue(value);
            break;
        case PreProcessorProperty::COLOR_HUE:
            this->colorH.SetValue(value);
            break;
        case PreProcessorProperty::COLOR_SATURATION:
            this->colorS.SetValue(value);
            break;
        case PreProcessorProperty::COLOR_VALUE:
            this->colorV.SetValue(value);
            break;
        case PreProcessorProperty::COLOR_ERROR:
            this->colorError.SetValue(value);
            break;
    }
}
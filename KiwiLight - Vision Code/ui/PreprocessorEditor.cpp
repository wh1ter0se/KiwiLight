#include "UI.h"

/**
 * Source file for the PreprocessorEditor class.
 * Written By: Brach Knuston
 */

using namespace KiwiLight;


PreprocessorEditor::PreprocessorEditor(PreProcessor preprocessor) {
    Panel editor = Panel(false, 0);

        Panel preprocessorTypePanel = Panel(false, 0);
            double processorIsFullDouble = preprocessor.GetProperty(PreProcessorProperty::IS_FULL);
            bool processorIsFullBool = (processorIsFullDouble == 1.0 ? true : false);

            Label preprocessorTypePanelHeader = Label("Type");
                preprocessorTypePanelHeader.SetName("gray");
                preprocessorTypePanel.Pack_start(preprocessorTypePanelHeader.GetWidget(), false, false, 0);

            Panel preprocessorPanelContents = Panel(true, 0);
                this->isFull = CheckBox("Full:", processorIsFullBool);
                    preprocessorPanelContents.Pack_start(this->isFull.GetWidget(), false, false, 0);

                this->isPartial = CheckBox("Partial:", !processorIsFullBool);
                    preprocessorPanelContents.Pack_start(this->isPartial.GetWidget(), false, false, 0);

                preprocessorTypePanel.Pack_start(preprocessorPanelContents.GetWidget(), false, false, 0);

            editor.Pack_start(preprocessorTypePanel.GetWidget(), false, false, 0);

        Panel targetColorPanel = Panel(false, 0);
            Label targetColorPanelHeader = Label("Target Color (HSV)");
                targetColorPanelHeader.SetName("gray");
                targetColorPanel.Pack_start(targetColorPanelHeader.GetWidget(), false, false, 0);

            Panel targetColorPanelContents = Panel(true, 0);
                double realColorH = preprocessor.GetProperty(PreProcessorProperty::COLOR_HUE);
                this->colorH = NumberBox(0, 100, realColorH);
                    targetColorPanelContents.Pack_start(this->colorH.GetWidget(), false, false, 0);

                double realColorS = preprocessor.GetProperty(PreProcessorProperty::COLOR_SATURATION);
                this->colorS = NumberBox(0, 255, realColorS);
                    targetColorPanelContents.Pack_start(this->colorS.GetWidget(), false, false, 0);

                double realColorV = preprocessor.GetProperty(PreProcessorProperty::COLOR_VALUE);
                this->colorV = NumberBox(0, 255, realColorV);
                    targetColorPanelContents.Pack_start(this->colorV.GetWidget(), false, false, 0);

                this->colorPreview = Image(ImageColorspace::RGB);
                    targetColorPanelContents.Pack_start(this->colorPreview.GetWidget(), false, false, 0);

                targetColorPanel.Pack_start(targetColorPanelContents.GetWidget(), false, false, 0);

            editor.Pack_start(targetColorPanel.GetWidget(), false, false, 0);

        double realColorError = preprocessor.GetProperty(PreProcessorProperty::COLOR_ERROR);
        this->colorError = LabeledSlider("Color Range", 0.0, 255.0, 1.0, realColorError);
            editor.Pack_start(this->colorError.GetWidget(), false, false, 0);

        double realThreshonld = preprocessor.GetProperty(PreProcessorProperty::THRESHOLD);
        this->threshold = LabeledSlider("Threshold", 1.0, 100.0, 1.0, realThreshonld);
            editor.Pack_start(this->threshold.GetWidget(), false, false, 0);

        double realErosion = preprocessor.GetProperty(PreProcessorProperty::EROSION);
        this->erosion = LabeledSlider("Erosion", 1.0, 25.0, 1.0, realErosion);
            editor.Pack_start(this->erosion.GetWidget(), false, false, 0);

        double realDilation = preprocessor.GetProperty(PreProcessorProperty::DILATION);
        this->dilation = LabeledSlider("Dilation", 1.0, 25.0, 1.0, realDilation);
            editor.Pack_start(this->dilation.GetWidget(), false, false, 0);

    this->preprocessoreditor = editor.GetWidget();
}


void PreprocessorEditor::SetName(std::string name) {
    gtk_widget_set_name(this->preprocessoreditor, name.c_str());
}
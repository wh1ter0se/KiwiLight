#include "UI.h"

/**
 * Source file for the PostprocessorEditor class.
 * Written By: Brach Knutson
 */

using namespace KiwiLight;


PostprocessorEditor::PostprocessorEditor(PostProcessor postprocessor) {
    Panel editor = Panel(false, 0);
        Panel contourChooserPanel = Panel(true, 0);
            Label contourChooserPanelHeader = Label("Contour: ");
                contourChooserPanelHeader.SetName("gray");
                contourChooserPanel.Pack_start(contourChooserPanelHeader.GetWidget(), true, true, 0);

            this->contourchooser = NumberBox(0, 10, 0);
                contourChooserPanel.Pack_start(this->contourchooser.GetWidget(), true, true, 0);
            
            editor.Pack_start(contourChooserPanel.GetWidget(), true, true, 0);

        Panel distXPanel = Panel(true, 0);
            Label distXPanelHeader = Label("HD:");
                distXPanel.Pack_start(distXPanelHeader.GetWidget(), true, true, 0);

            double realDistX = postprocessor.GetTargetContourProperty(0, TargetProperty::DIST_X).Value();
            this->distX = NumberBox(-10.0, 10.0, 0.05, realDistX);
                distXPanel.Pack_start(this->distX.GetWidget(), false, false, 0);

            double realDistXErr = postprocessor.GetTargetContourProperty(0, TargetProperty::DIST_X).Error();
            this->distXErr = LabeledSlider("Range", 0.0, 10.0, 0.05, realDistXErr);
                distXPanel.Pack_start(this->distXErr.GetWidget(), true, true, 0);

            editor.Pack_start(distXPanel.GetWidget(), true, true, 0);

        Panel distYPanel = Panel(true, 0);
            Label distYPanelHeader = Label("VD:");
                distYPanel.Pack_start(distYPanelHeader.GetWidget(), true, true, 0);

            double realDistY = postprocessor.GetTargetContourProperty(0, TargetProperty::DIST_Y).Value();
            this->distY = NumberBox(-10.0, 10.0, 0.05, realDistY);
                distYPanel.Pack_start(this->distY.GetWidget(), false, false, 0);

            double realDistYErr = postprocessor.GetTargetContourProperty(0, TargetProperty::DIST_Y).Error();
            this->distYErr = LabeledSlider("Range", 0.0, 10.0, 0.05, realDistYErr);
                distYPanel.Pack_start(this->distYErr.GetWidget(), true, true, 0);

            editor.Pack_start(distYPanel.GetWidget(), true, true, 0);

        Panel anglePanel = Panel(true, 0);
            double realAngle = postprocessor.GetTargetContourProperty(0, TargetProperty::ANGLE).Value();
            this->angle = LabeledSlider("Angle", -90.0, 90.0, 0.05, realAngle);
                anglePanel.Pack_start(this->angle.GetWidget(), true, true, 0);

            double realAngleErr = postprocessor.GetTargetContourProperty(0, TargetProperty::ANGLE).Error();
            this->angleErr = LabeledSlider("Range", -90.0, 90.0, 0.05, realAngleErr);
                anglePanel.Pack_start(this->angleErr.GetWidget(), true, true, 0);

            editor.Pack_start(anglePanel.GetWidget(), true, true, 0);

        Panel arPanel = Panel(true, 0);
            double realAR = postprocessor.GetTargetContourProperty(0, TargetProperty::ASPECT_RATIO).Value();
            this->ar = LabeledSlider("Aspect Ratio", 0.01, 10.0, 0.01, realAR);
                arPanel.Pack_start(this->ar.GetWidget(), true, true, 0);

            double realArErr = postprocessor.GetTargetContourProperty(0, TargetProperty::ASPECT_RATIO).Error();
            this->arErr = LabeledSlider("Range", 0.01, 10.0, 0.01, realArErr);
                arPanel.Pack_start(this->arErr.GetWidget(), true, true, 0);
            
            editor.Pack_start(arPanel.GetWidget(), true, true, 0);

        Panel solidityPanel = Panel(true, 0);
            double realSolidity = postprocessor.GetTargetContourProperty(0, TargetProperty::SOLIDITY).Value();
            this->solidity = LabeledSlider("Solidity", 0.0, 1.0, 0.01, realSolidity);
                solidityPanel.Pack_start(this->solidity.GetWidget(), true, true, 0);

            double realSolidityErr = postprocessor.GetTargetContourProperty(0, TargetProperty::SOLIDITY).Error();
            this->solidityErr = LabeledSlider("Range", 0.0, 1.0, 0.1, realSolidityErr);
                solidityPanel.Pack_start(this->solidityErr.GetWidget(), true, true, 0);

            editor.Pack_start(solidityPanel.GetWidget(), true, true, 0);

        double realMinArea = postprocessor.GetTargetContourProperty(0, TargetProperty::MINIMUM_AREA).Value();
        this->minimumArea = LabeledSlider("Minimum Area", 5.0, 10000.0, 5.0, realMinArea);
            editor.Pack_start(this->minimumArea.GetWidget(), true, true, 0);

    this->postprocessoreditor = editor.GetWidget();
}


void PostprocessorEditor::SetName(std::string name) {
    gtk_widget_set_name(this->postprocessoreditor, name.c_str());
}
#include "UI.h"

/**
 * Source file for the PostprocessorEditor class.
 * Written By: Brach Knutson
 */

using namespace KiwiLight;


PostprocessorEditor::PostprocessorEditor(PostProcessor postprocessor) {
    //create a runner to store values for all contours rather than having arrays and things
    this->storageRunner = Runner("confs/generic.xml", true, false);

    //init the values in the storage runner because they are all generic right now
    for(int i=0; i<postprocessor.GetExampleTargetByID(0).Contours().size(); i++) {
        this->storageRunner.SetPostProcessorContourProperty(i, TargetProperty::DIST_X, postprocessor.GetTargetContourProperty(i, TargetProperty::DIST_X));
        this->storageRunner.SetPostProcessorContourProperty(i, TargetProperty::DIST_Y, postprocessor.GetTargetContourProperty(i, TargetProperty::DIST_Y));
        this->storageRunner.SetPostProcessorContourProperty(i, TargetProperty::ANGLE, postprocessor.GetTargetContourProperty(i, TargetProperty::ANGLE));
        this->storageRunner.SetPostProcessorContourProperty(i, TargetProperty::ASPECT_RATIO, postprocessor.GetTargetContourProperty(i, TargetProperty::ASPECT_RATIO));
        this->storageRunner.SetPostProcessorContourProperty(i, TargetProperty::SOLIDITY, postprocessor.GetTargetContourProperty(i, TargetProperty::SOLIDITY));
        this->storageRunner.SetPostProcessorContourProperty(i, TargetProperty::MINIMUM_AREA, postprocessor.GetTargetContourProperty(i, TargetProperty::MINIMUM_AREA));
    }

    Panel editor = Panel(false, 0);
        Panel contourChooserPanel = Panel(true, 0);
            Label contourChooserPanelHeader = Label("Contour: ");
                contourChooserPanelHeader.SetName("gray");
                contourChooserPanel.Pack_start(contourChooserPanelHeader.GetWidget(), true, true, 0);

            this->contourchooser = NumberBox(0, 10, 0);
                contourChooserPanel.Pack_start(this->contourchooser.GetWidget(), true, true, 0);
            
            editor.Pack_start(contourChooserPanel.GetWidget(), true, true, 0);

        //distance editor
        Label distanceFromCenterHeader = Label("Distance from Center");
            distanceFromCenterHeader.SetName("subHeader");
            editor.Pack_start(distanceFromCenterHeader.GetWidget(), true, true, 0);

        Panel distXPanel = Panel(true, 0);
            Label distXPanelHeader = Label("Horizontal:");
                distXPanel.Pack_start(distXPanelHeader.GetWidget(), true, true, 0);

            double realDistX = postprocessor.GetTargetContourProperty(0, TargetProperty::DIST_X).Value();
            this->distX = NumberBox(-10.0, 10.0, 0.05, realDistX);
                distXPanel.Pack_start(this->distX.GetWidget(), false, false, 0);

            double realDistXErr = postprocessor.GetTargetContourProperty(0, TargetProperty::DIST_X).Error();
            this->distXErr = LabeledSlider("Range", 0.0, 10.0, 0.05, realDistXErr);
                distXPanel.Pack_start(this->distXErr.GetWidget(), true, true, 0);

            editor.Pack_start(distXPanel.GetWidget(), true, true, 0);

        Panel distYPanel = Panel(true, 0);
            Label distYPanelHeader = Label("Vertical:");
                distYPanel.Pack_start(distYPanelHeader.GetWidget(), true, true, 0);

            double realDistY = postprocessor.GetTargetContourProperty(0, TargetProperty::DIST_Y).Value();
            this->distY = NumberBox(-10.0, 10.0, 0.05, realDistY);
                distYPanel.Pack_start(this->distY.GetWidget(), false, false, 0);

            double realDistYErr = postprocessor.GetTargetContourProperty(0, TargetProperty::DIST_Y).Error();
            this->distYErr = LabeledSlider("Range", 0.0, 10.0, 0.05, realDistYErr);
                distYPanel.Pack_start(this->distYErr.GetWidget(), true, true, 0);

            editor.Pack_start(distYPanel.GetWidget(), true, true, 0);

        Label angleHeader = Label("Contour Shape");
            angleHeader.SetName("subHeader");
            editor.Pack_start(angleHeader.GetWidget(), true, true, 0);

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


int PostprocessorEditor::GetNumContours() {
    return this->storageRunner.GetExampleTargetByID(0).Contours().size();
}

/**
 * Updates the editor.
 */
void PostprocessorEditor::Update() {
    int currentContour = (int) this->contourchooser.GetValue();

    //set dist x
    SettingPair distXPair = SettingPair(this->distX.GetValue(), this->distXErr.GetValue());
        this->storageRunner.SetPostProcessorContourProperty(currentContour, TargetProperty::DIST_X, distXPair);

    //set dist y
    SettingPair distYPair = SettingPair(this->distY.GetValue(), this->distYErr.GetValue());
        this->storageRunner.SetPostProcessorContourProperty(currentContour, TargetProperty::DIST_Y, distYPair);

    //set angle
    SettingPair anglePair = SettingPair(this->angle.GetValue(), this->angleErr.GetValue());
        this->storageRunner.SetPostProcessorContourProperty(currentContour, TargetProperty::ANGLE, anglePair);

    //set aspect ratio
    SettingPair arPair = SettingPair(this->ar.GetValue(), this->arErr.GetValue());
        this->storageRunner.SetPostProcessorContourProperty(currentContour, TargetProperty::ASPECT_RATIO, arPair);

    //set solidity
    SettingPair solidityPair = SettingPair(this->solidity.GetValue(), this->solidityErr.GetValue());
        this->storageRunner.SetPostProcessorContourProperty(currentContour, TargetProperty::SOLIDITY, solidityPair);

    //set min area
    SettingPair minAreaPair = SettingPair(this->minimumArea.GetValue(), 0);
        this->storageRunner.SetPostProcessorContourProperty(currentContour, TargetProperty::MINIMUM_AREA, minAreaPair);
}

/**
 * Returns the value and range of the specified property.
 */
SettingPair PostprocessorEditor::GetProperty(int contour, TargetProperty prop) {
    return this->storageRunner.GetPostProcessorContourProperty(contour, prop);
}

/**
 * Sets the value and range of the specified property.
 */
void PostprocessorEditor::SetProperty(int contour, TargetProperty prop, SettingPair value) {
    this->storageRunner.SetPostProcessorContourProperty(contour, prop, value);

    //it is a current value and should be changed on screen
    if(this->contourchooser.GetValue() == contour) {
        switch(prop) {
            case TargetProperty::DIST_X:
                this->distX.SetValue(value.Value());
                this->distXErr.SetValue(value.Error());
                break;
            case TargetProperty::DIST_Y:
                this->distY.SetValue(value.Value());
                this->distYErr.SetValue(value.Error());
                break;
            case TargetProperty::ANGLE:
                this->angle.SetValue(value.Value());
                this->angleErr.SetValue(value.Error());
                break;
            case TargetProperty::ASPECT_RATIO:
                this->ar.SetValue(value.Value());
                this->arErr.SetValue(value.Error());
                break;
            case TargetProperty::SOLIDITY:
                this->solidity.SetValue(value.Value());
                this->solidityErr.SetValue(value.Error());
                break;
            case TargetProperty::MINIMUM_AREA:
                this->minimumArea.SetValue(value.Value());
                break;
        }
    }
}


void PostprocessorEditor::SetName(std::string name) {
    gtk_widget_set_name(this->postprocessoreditor, name.c_str());
}
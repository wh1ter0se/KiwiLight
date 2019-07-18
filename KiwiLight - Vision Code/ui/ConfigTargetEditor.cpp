#include "UI.h"

/**
 * Source file for the ConfigTargetEditor class.
 * Written By: Brach Knutson
 */

using namespace cv;
using namespace KiwiLight;

static void learnTargetPressed() {
    Flags::RaiseFlag("StartLearner");

    ConfirmationDialog confirmLearn = ConfirmationDialog("Ensure that the target is focused in the camera's view so that a blue box is drawn around it.\nWhen this is done, press OK.");
    bool userWantsLearn = confirmLearn.ShowAndGetResponse();

    if(userWantsLearn) {
        Flags::RaiseFlag("StopLearnerAndLearn");
    } else {
        Flags::RaiseFlag("StopLearner");
    }
}


static void troubleshootTargetPressed() {
    ConfirmationDialog confirmTroubleshoot = ConfirmationDialog("Ensure that the area you are in has as little ambient noise as possible. Then press OK.");
    bool userWantsTroubleshoot = confirmTroubleshoot.ShowAndGetResponse();

    if(userWantsTroubleshoot) {
        Flags::RaiseFlag("TroubleshootTarget"); //handled in ConfigEditor.cpp
    }
}

/**
 * Creates a new ConfigTargetEditor editing the targets in the file at fileName
 */
ConfigTargetEditor::ConfigTargetEditor(std::string fileName, Runner runner) {
    XMLDocument document = XMLDocument(fileName);
    this->currentPreProcessor = PreProcessorType::FULL; //because the checkbox defaults to full
    this->panel = Panel(false, 0);

    this->runner = Runner(fileName, true, false);
        
        XMLTag configurationTag = document.GetTagsByName("configuration")[0];

        //creates a vertical scroll window
        Scrollable scroll = Scrollable(false, true);
            Panel scrollableEditor = Panel(false, 0);

                //create editor for the preprocessor
                Panel preprocessorPanel = Panel(false, 0);

                    XMLTag preprocessorTag = configurationTag.GetTagsByName("preprocessor")[0];
                    
                    Label preprocessorPanelHeader = Label("Preprocessor");
                        preprocessorPanelHeader.SetName("subHeader");
                        preprocessorPanel.Pack_start(preprocessorPanelHeader.GetWidget(), false, false, 0);

                    //chooser for whether or not the preprocessor is full or partial
                    Panel preprocessorTypeChooser = Panel(true, 5);
                        Label preprocessorTypeHeader = Label("Type:");
                            preprocessorTypeChooser.Pack_start(preprocessorTypeHeader.GetWidget(), false, false, 0);

                        if(preprocessorTag.GetAttributesByName("type")[0].Value() == "full") {
                            this->currentPreProcessor = PreProcessorType::FULL;
                        } else {
                            this->currentPreProcessor = PreProcessorType::PARTIAL;
                        }

                        this->fullPreProcessor = CheckBox("Full", false);
                            preprocessorTypeChooser.Pack_start(this->fullPreProcessor.GetWidget(), false, false, 0);

                        this->partialPreProcessor = CheckBox("Partial", false);
                            preprocessorTypeChooser.Pack_start(this->partialPreProcessor.GetWidget(), false, false, 0);

                        preprocessorPanel.Pack_start(preprocessorTypeChooser.GetWidget(), false, false, 0);
                    
                    Label colorHeader = Label("Target Color(HSV):");
                        preprocessorPanel.Pack_start(colorHeader.GetWidget(), false, false, 0);

                    Panel preprocessorColorChooser = Panel(true, 3);

                        //target color tag where the hsv is stored
                        XMLTag targetColor = preprocessorTag.GetTagsByName("targetColor")[0];

                        //create the H, S, and V and asisgn them the values from the file.
                        this->colorH = NumberBox(0.0, 100.0, 0.0);
                            int realColorH = std::stoi(targetColor.GetTagsByName("h")[0].Content());
                            this->colorH.SetValue((double) realColorH);
                            preprocessorColorChooser.Pack_start(this->colorH.GetWidget(), false, false, 0);

                        this->colorS = NumberBox(0.0, 255.0, 0.0);
                            int realColorS = std::stoi(targetColor.GetTagsByName("s")[0].Content());
                            this->colorS.SetValue((double) realColorS);
                            preprocessorColorChooser.Pack_start(this->colorS.GetWidget(), false, false, 0);

                        this->colorV = NumberBox(0.0, 255.0, 0.0);
                            int realColorV = std::stoi(targetColor.GetTagsByName("v")[0].Content());
                            this->colorV.SetValue((double) realColorV);
                            preprocessorColorChooser.Pack_start(this->colorV.GetWidget(), false, false, 0);

                        this->colorResult = Image(ImageColorspace::RGB);
                            preprocessorColorChooser.Pack_start(colorResult.GetWidget(), false, false, 0);

                        preprocessorPanel.Pack_start(preprocessorColorChooser.GetWidget(), false, false, 0);

                    this->colorError = LabeledSlider("Color Error", 0, 100, 1, 0);
                        int realColorError = std::stoi(targetColor.GetAttributesByName("error")[0].Value());
                        this->colorError.SetValue(realColorError);
                        preprocessorPanel.Pack_start(this->colorError.GetWidget(), false, false, 0);

                    //now create threshold and dilation sliders and set the values to the ones from the file
                    this->thresholdValue = LabeledSlider("Threshold Value:", 0.0, 255.0, 1.0, 0.0);
                        int realThreshold = std::stoi(preprocessorTag.GetTagsByName("threshold")[0].Content());
                        this->thresholdValue.SetValue((double) realThreshold);
                        preprocessorPanel.Pack_start(this->thresholdValue.GetWidget(), false, false, 0);

                    this->erosionFactor = LabeledSlider("Erosion:", 1.0, 50.0, 0.5, 0.0);
                        int realErosion = std::stoi(preprocessorTag.GetTagsByName("erosion")[0].Content());
                        this->erosionFactor.SetValue(realErosion);
                        preprocessorPanel.Pack_start(this->erosionFactor.GetWidget(), false, false, 0);

                    this->dilationFactor = LabeledSlider("Dilation:", 1.0, 50.0, 0.5, 0.0);
                        double realDilation = std::stod(preprocessorTag.GetTagsByName("dilation")[0].Content());
                        this->dilationFactor.SetValue(realDilation);
                        preprocessorPanel.Pack_start(this->dilationFactor.GetWidget(), false, false, 0);

                    scrollableEditor.Pack_start(preprocessorPanel.GetWidget(), false, false, 0);

                    Separator sep = Separator(true);
                        scrollableEditor.Pack_start(sep.GetWidget(), false, false, 5);

                    Panel postprocessorPanel = Panel(false, 3);
                        Label postprocessorPanelHeader = Label("Postprocessor");
                            postprocessorPanelHeader.SetName("subHeader");
                            postprocessorPanel.Pack_start(postprocessorPanelHeader.GetWidget(), false, false, 0);

                        Label contourHeader = Label("Contours");
                            contourHeader.SetName("subHeader");
                            postprocessorPanel.Pack_start(contourHeader.GetWidget(), false, false, 0);

                        //the panel where the user can chooser which contour they are editing
                        Panel postprocessorContourChooser = Panel(true, 3);
                            Label postprocessorChooserHeader = Label("Contour:");
                                postprocessorChooserHeader.SetName("gray");
                                postprocessorContourChooser.Pack_start(postprocessorChooserHeader.GetWidget(), false ,false, 0);

                            this->ContourID = NumberBox(0.0, 8.0, 0.0);
                                postprocessorContourChooser.Pack_start(this->ContourID.GetWidget(), false ,false, 0);

                            postprocessorPanel.Pack_start(postprocessorContourChooser.GetWidget(), false, false, 0);

                        scrollableEditor.Pack_start(postprocessorPanel.GetWidget(), true, true, 0);

                        XMLTag postprocessorTag = configurationTag.GetTagsByName("postprocessor")[0];
                        XMLTag firstTarget;
                        XMLTag firstContour;
                        std::vector<XMLTag> potentialTargets = postprocessorTag.GetTagsByName("target");

                        //find the target with the id "0", the default target
                        for(int i=0; i<potentialTargets.size(); i++) {
                            if(potentialTargets[i].GetAttributesByName("id")[0].Value() == "0") {
                                //the target at i is the default target
                                firstTarget = potentialTargets[i];
                                break;
                            }
                        }

                        //find the contour of the target with the id "0" (default contour)
                        std::vector<XMLTag> potentialContours = firstTarget.GetTagsByName("contour");
                        for(int i=0; i<potentialContours.size(); i++) {
                            if(potentialContours[i].GetAttributesByName("id")[0].Value() == "0") {
                                firstContour = potentialContours[i];
                                break;
                            }
                        }
                        
                        //assemble contour editor
                        Panel distXPanel = Panel(true, 3);
                            Label xLabel = Label("X Distance: ");
                                distXPanel.Pack_start(xLabel.GetWidget(), false, false, 0);

                            this->ContourDistX = NumberBox(-10.0, 10.0, 0.1, 0.0);
                                distXPanel.Pack_start(this->ContourDistX.GetWidget(), false, false, 0);

                            this->ContourDistXErr = LabeledSlider("Error", 0.0, 100.0, 0.05, 0.5);
                                distXPanel.Pack_start(this->ContourDistXErr.GetWidget(), true, true, 0);

                            scrollableEditor.Pack_start(distXPanel.GetWidget(), true, true, 0);

                        Panel distYPanel = Panel(true, 3);
                            Label yLabel = Label("Y Distance: ");
                                distYPanel.Pack_start(yLabel.GetWidget(), false, false, 0);

                            this->ContourDistY = NumberBox(-10.0, 10.0, 0.1, 0.0);
                                distYPanel.Pack_start(this->ContourDistY.GetWidget(), false, false, 0);

                            this->ContourDistYErr = LabeledSlider("Error", 0.0, 100.0, 0.05, 0.5);
                                distYPanel.Pack_start(this->ContourDistYErr.GetWidget(), true ,true, 0);

                            scrollableEditor.Pack_start(distYPanel.GetWidget(), true, true, 0);

                        //angle editor
                        Panel anglePanel = Panel(true, 0);
                            this->ContourAngle = LabeledSlider("Angle", -90.0, 90.0, 0.05, 0);
                                anglePanel.Pack_start(this->ContourAngle.GetWidget(), true, true, 0);

                            this->ContourAngleErr = LabeledSlider("Angle Error", 0.0, 90.0, 0.05, 12.0);
                                anglePanel.Pack_start(this->ContourAngleErr.GetWidget(), true, true, 0);

                            scrollableEditor.Pack_start(anglePanel.GetWidget(), true, true, 0);
                        
                        //solidity editor
                        Panel solidPanel = Panel(true, 0);
                            this->ContourSolidity = LabeledSlider("Solidity", 0.0, 1.0, 0.01, 0.9);
                                solidPanel.Pack_start(this->ContourSolidity.GetWidget(), true, true, 0);

                            this->ContourSolidityErr = LabeledSlider("Solidity Error", 0.0, 1.0, 0.01, 0.3);
                                double realSolidErr = std::stod(firstContour.GetTagsByName("solidity")[0].GetAttributesByName("error")[0].Value());
                                this->ContourSolidityErr.SetValue(realSolidErr);
                                solidPanel.Pack_start(this->ContourSolidityErr.GetWidget(), true, true, 0);

                            scrollableEditor.Pack_start(solidPanel.GetWidget(), true, true, 0);

                        //AR editor
                        Panel ARPanel = Panel(true, 0);
                            this->ContourAR = LabeledSlider("Aspect Ratio", 0.1, 10.0, 0.01, 1.0);
                                ARPanel.Pack_start(this->ContourAR.GetWidget(), true, true, 0);
                            
                            this->ContourARErr = LabeledSlider("Aspect Ratio Error", 0.0, 10.0, 0.01, 0.2);
                                ARPanel.Pack_start(this->ContourARErr.GetWidget(), true, true, 0);

                            scrollableEditor.Pack_start(ARPanel.GetWidget(), true, true, 0);

                        //minimum area editor
                        this->ContourMinArea = LabeledSlider("Minimum Area:", 5, 10000, 5, 350);
                            scrollableEditor.Pack_start(this->ContourMinArea.GetWidget(), false, false, 0);

                        scroll.PackWidget(scrollableEditor.GetWidget());
                    this->panel.Pack_start(scroll.GetWidget(), true, true, 0);

                this->learnTarget = Button("Learn Target", learnTargetPressed);
                    this->panel.Pack_start(this->learnTarget.GetWidget(), false, false, 0);

                this->troubleshootTarget = Button("Troubleshoot", troubleshootTargetPressed);
                    this->panel.Pack_start(this->troubleshootTarget.GetWidget(), false, false, 0);

    //set the sliders to the values for contour 0
    this->SetValues(0);
    this->configtargeteditor = this->panel.GetWidget();
}

/**
 * Updates the ConfigTargetEditor's values and the output image
 */
void ConfigTargetEditor::Update() {
    //update the color result image
    int currentColorH = (int) this->colorH.GetValue();
    int currentColorS = (int) this->colorS.GetValue();
    int currentColorv = (int) this->colorV.GetValue();

    Mat newColorImage = Mat(Size(30, 30), CV_8UC3, Scalar(currentColorH, currentColorS, currentColorv));
    cvtColor(newColorImage, newColorImage, COLOR_HSV2BGR_FULL);

    this->colorResult.Update(newColorImage);

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

    int currentRequestedContour = this->ContourID.GetValue();
    if(currentRequestedContour != this->lastRequestedContour) {
        //the requested contour has changed in last frame, update values
        SetValues(currentRequestedContour);
        this->lastRequestedContour = currentRequestedContour;
    }

    //set the preprocessor values in the runner
    double preprocessorIsFull = (this->currentPreProcessor == PreProcessorType::FULL ? 0 : 1);
    this->runner.SetPreprocessorProperty(PreProcessorProperty::IS_FULL, preprocessorIsFull);
    this->runner.SetPreprocessorProperty(PreProcessorProperty::THRESHOLD, this->thresholdValue.GetValue());
    this->runner.SetPreprocessorProperty(PreProcessorProperty::THRESH_VALUE, 255);
    this->runner.SetPreprocessorProperty(PreProcessorProperty::EROSION, this->erosionFactor.GetValue());
    this->runner.SetPreprocessorProperty(PreProcessorProperty::DILATION, this->dilationFactor.GetValue());
    this->runner.SetPreprocessorProperty(PreProcessorProperty::COLOR_HUE, this->colorH.GetValue());
    this->runner.SetPreprocessorProperty(PreProcessorProperty::COLOR_SATURATION, this->colorS.GetValue());
    this->runner.SetPreprocessorProperty(PreProcessorProperty::COLOR_VALUE, this->colorV.GetValue());
    this->runner.SetPreprocessorProperty(PreProcessorProperty::COLOR_ERROR, this->colorError.GetValue());

    //set the runner values to the slider
    if(this->lastRequestedContour >= 0 && this->lastRequestedContour < this->NumContours()) {
        SettingPair distXPair = SettingPair(this->ContourDistX.GetValue(), this->ContourDistXErr.GetValue());
            this->runner.SetPostProcessorContourProperty((int) this->ContourID.GetValue(), TargetProperty::DIST_X, distXPair);
        
        SettingPair distYPair = SettingPair(this->ContourDistY.GetValue(), this->ContourDistYErr.GetValue());
            this->runner.SetPostProcessorContourProperty((int) this->ContourID.GetValue(), TargetProperty::DIST_Y, distYPair);
        
        SettingPair anglePair = SettingPair(this->ContourAngle.GetValue(), this->ContourAngleErr.GetValue());
            this->runner.SetPostProcessorContourProperty((int) this->ContourID.GetValue(), TargetProperty::ANGLE, anglePair);
        
        SettingPair ARPair = SettingPair(this->ContourAR.GetValue(), this->ContourARErr.GetValue());
            this->runner.SetPostProcessorContourProperty((int) this->ContourID.GetValue(), TargetProperty::ASPECT_RATIO, ARPair);
        
        SettingPair solidityPair = SettingPair(this->ContourSolidity.GetValue(), this->ContourSolidityErr.GetValue());
            this->runner.SetPostProcessorContourProperty((int) this->ContourID.GetValue(), TargetProperty::SOLIDITY, solidityPair);
        
        SettingPair minimumAreaPair = SettingPair(this->ContourMinArea.GetValue(), 0);
            this->runner.SetPostProcessorContourProperty((int) this->ContourID.GetValue(), TargetProperty::MINIMUM_AREA, minimumAreaPair);
    }
}

/**
 * Returns the number of contours that this editor currently represents.
 */
int ConfigTargetEditor::NumContours() {
    return this->runner.GetExampleTargetByID(0).Contours().size();
}


double ConfigTargetEditor::GetPreProcessorProperty(PreProcessorProperty prop) {
    return this->runner.GetPreprocessorProperty(prop);
}


void ConfigTargetEditor::SetPreProcessorProperty(PreProcessorProperty prop, double value) {
    this->runner.SetPreprocessorProperty(prop, value);
}


SettingPair ConfigTargetEditor::GetTargetPropertyValue(int contour, TargetProperty property) {
    return this->runner.GetPostProcessorContourProperty(contour, property);
}


void ConfigTargetEditor::SetTargetPropertyValue(int contour, TargetProperty property, SettingPair values) {
    this->runner.SetPostProcessorContourProperty(contour, property, values);

    //set the value on the slider
    if(this->lastRequestedContour == contour) {
        switch(property) {
            case TargetProperty::DIST_X:
                this->ContourDistX.SetValue(values.Value());
                break;
            case TargetProperty::DIST_Y:
                this->ContourDistY.SetValue(values.Value());
                break;
            case TargetProperty::ANGLE:
                this->ContourAngle.SetValue(values.Value());
                break;
            case TargetProperty::SOLIDITY:
                this->ContourSolidity.SetValue(values.Value());
                break;
            case TargetProperty::ASPECT_RATIO:
                this->ContourAR.SetValue(values.Value());
                break;
            case TargetProperty::MINIMUM_AREA:
                this->ContourMinArea.SetValue(values.Value());
                break;
        }
    }
}


void ConfigTargetEditor::SetExampleTarget(int targetID, ExampleTarget target) {
    this->runner.SetExampleTarget(targetID, target);
}


void ConfigTargetEditor::SetName(std::string name) {
    gtk_widget_set_name(this->configtargeteditor, name.c_str());
}


/**
 * Sets the slider values for contour with the passed id.
 */
void ConfigTargetEditor::SetValues(int contourID) {
    if(contourID >= 0 && contourID < this->NumContours()) {
        //set contour dist X values
        this->ContourDistX.SetValue(this->runner.GetPostProcessorContourProperty(contourID, TargetProperty::DIST_X).Value());
        this->ContourDistXErr.SetValue(this->runner.GetPostProcessorContourProperty(contourID, TargetProperty::DIST_X).Error());
        
        //set contour dist Y values
        this->ContourDistY.SetValue(this->runner.GetPostProcessorContourProperty(contourID, TargetProperty::DIST_Y).Value());
        this->ContourDistYErr.SetValue(this->runner.GetPostProcessorContourProperty(contourID, TargetProperty::DIST_Y).Error());

        //set contour angle values
        this->ContourAngle.SetValue(this->runner.GetPostProcessorContourProperty(contourID, TargetProperty::ANGLE).Value());
        this->ContourAngleErr.SetValue(this->runner.GetPostProcessorContourProperty(contourID, TargetProperty::ANGLE).Error());

        //set contour solidity values
        this->ContourSolidity.SetValue(this->runner.GetPostProcessorContourProperty(contourID, TargetProperty::SOLIDITY).Value());
        this->ContourSolidityErr.SetValue(this->runner.GetPostProcessorContourProperty(contourID, TargetProperty::SOLIDITY).Error());

        //set contour aspect ratio values
        this->ContourAR.SetValue(this->runner.GetPostProcessorContourProperty(contourID, TargetProperty::ASPECT_RATIO).Value());
        this->ContourARErr.SetValue(this->runner.GetPostProcessorContourProperty(contourID, TargetProperty::ASPECT_RATIO).Error());

        //set contour minimum area values
        this->ContourMinArea.SetValue(this->runner.GetPostProcessorContourProperty(contourID, TargetProperty::MINIMUM_AREA).Value());
    } else {
        std::cout << "sorry bud, theres no contour with id " << contourID << "." << std::endl;
    }
}

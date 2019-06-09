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
ConfigEditor::ConfigEditor(std::string fileName, VideoCapture cap) {
    this->runner = Runner(fileName, true, cap);
    this->editorMode = EditorMode::USE_RUNNER;
    this->currentDoc = XMLDocument(fileName);
    this->fileName = fileName;
    this->confName = this->currentDoc.GetTagsByName("configuration")[0].GetAttributesByName("name")[0].Value();

    this->window = Window(GTK_WINDOW_TOPLEVEL, false);
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
        this->runner.SetPreprocessorProperty(PreProcessorProperty::IS_FULL, this->targetEditor.GetPreProcessorProperty(PreProcessorProperty::IS_FULL));
        this->runner.SetPreprocessorProperty(PreProcessorProperty::THRESHOLD, this->targetEditor.GetPreProcessorProperty(PreProcessorProperty::THRESHOLD));
        this->runner.SetPreprocessorProperty(PreProcessorProperty::THRESH_VALUE, this->targetEditor.GetPreProcessorProperty(PreProcessorProperty::THRESH_VALUE));
        this->runner.SetPreprocessorProperty(PreProcessorProperty::DILATION, this->targetEditor.GetPreProcessorProperty(PreProcessorProperty::DILATION));
        this->runner.SetPreprocessorProperty(PreProcessorProperty::COLOR_HUE, this->targetEditor.GetPreProcessorProperty(PreProcessorProperty::COLOR_HUE));
        this->runner.SetPreprocessorProperty(PreProcessorProperty::COLOR_SATURATION, this->targetEditor.GetPreProcessorProperty(PreProcessorProperty::COLOR_SATURATION));
        this->runner.SetPreprocessorProperty(PreProcessorProperty::COLOR_VALUE, this->targetEditor.GetPreProcessorProperty(PreProcessorProperty::COLOR_VALUE));
        this->runner.SetPreprocessorProperty(PreProcessorProperty::COLOR_ERROR, this->targetEditor.GetPreProcessorProperty(PreProcessorProperty::COLOR_ERROR));

        //set all runner targeting settings to the ones in the target editor
        // std::cout << "upate real runner" << std::endl;
        for(int i=0; i<this->targetEditor.NumContours(); i++) {
            this->runner.SetPostProcessorContourProperty(i, TargetProperty::DIST_X, this->targetEditor.GetTargetPropertyValue(i, TargetProperty::DIST_X));
            this->runner.SetPostProcessorContourProperty(i, TargetProperty::DIST_Y, this->targetEditor.GetTargetPropertyValue(i, TargetProperty::DIST_Y));
            this->runner.SetPostProcessorContourProperty(i, TargetProperty::ANGLE, this->targetEditor.GetTargetPropertyValue(i, TargetProperty::ANGLE));
            this->runner.SetPostProcessorContourProperty(i, TargetProperty::ASPECT_RATIO, this->targetEditor.GetTargetPropertyValue(i, TargetProperty::ASPECT_RATIO));
            this->runner.SetPostProcessorContourProperty(i, TargetProperty::SOLIDITY, this->targetEditor.GetTargetPropertyValue(i, TargetProperty::SOLIDITY));
            this->runner.SetPostProcessorContourProperty(i, TargetProperty::MINIMUM_AREA, this->targetEditor.GetTargetPropertyValue(i, TargetProperty::MINIMUM_AREA));
            // std::cout << "min area editor: " << this->targetEditor.GetTargetPropertyValue(i, TargetProperty::MINIMUM_AREA).Value() << std::endl;
        }

        this->runner.SetRunnerProperty(RunnerProperty::TRUE_WIDTH, this->runnerEditor.GetProperty(RunnerProperty::TRUE_WIDTH));
        this->runner.SetRunnerProperty(RunnerProperty::PERCEIVED_WIDTH, this->runnerEditor.GetProperty(RunnerProperty::PERCEIVED_WIDTH));
        this->runner.SetRunnerProperty(RunnerProperty::CALIBRATED_DISTANCE, this->runnerEditor.GetProperty(RunnerProperty::CALIBRATED_DISTANCE));
        this->runner.SetRunnerProperty(RunnerProperty::ERROR_CORRECTION, this->runnerEditor.GetProperty(RunnerProperty::ERROR_CORRECTION));

        this->runnerEditor.Update(this->runner.GetOriginalImage(), this->out, this->runner.GetClosestTargetToCenter().Distance());
    } else if(this->editorMode == EditorMode::USE_LEARNER) {
        // this->runnerEditor.Update(this->learner.GetOriginalImage(), this->learner.GetOutputImage(), -1);
        double minimumArea = this->targetEditor.GetTargetPropertyValue(0, TargetProperty::MINIMUM_AREA).Value();
        this->learner.Update(minimumArea); //use minimumArea in above line instead of constant 500
        this->out = this->learner.GetOutputImage();
        this->runnerEditor.Update(this->learner.GetOriginalImage(), this->learner.GetOutputImage(), -1);
    }

    this->cameraSettings.Update();
    this->targetEditor.Update();

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
        this->learner.SetConstantResize(newSize);
        this->editorMode = EditorMode::USE_LEARNER;
    }

    if(Flags::GetFlag("StopLearner")) {
        Flags::LowerFlag("StopLearner");
        this->editorMode = EditorMode::USE_RUNNER;
    }
}


void ConfigEditor::UpdateImageOnly() {
    if(this->editorMode == EditorMode::USE_RUNNER) {
        this->runner.Iterate();
        this->out = this->runner.GetOutputImage();
    }
}

/**
 * Causes the editor to save the config to file.
 */
void ConfigEditor::Save() {
    PopupTextBox namePopup = PopupTextBox("Enter Name", "Enter the name of the configuration.", this->confName);
        std::string nameResult = namePopup.Show();

    XMLDocument newDocument = XMLDocument();
        XMLTag cameraTag = XMLTag("camera");
            XMLTagAttribute cameraIndex = XMLTagAttribute("index", std::to_string(this->runner.GetCameraIndex()));
                cameraTag.AddAttribute(cameraIndex);

            XMLTag camResolution = XMLTag("resolution");
                XMLTag camResWidth = XMLTag("width", std::to_string(this->cameraSettings.GetWidth()));
                    camResolution.AddTag(camResWidth);

                XMLTag camResHeight = XMLTag("height", std::to_string(this->cameraSettings.GetHeight()));
                    camResolution.AddTag(camResHeight);

                cameraTag.AddTag(camResolution);

            cameraTag.AddTag(this->cameraSettings.GetFinishedTag());
            newDocument.AddTag(cameraTag);
        
        XMLTag configurationTag = XMLTag("configuration");
            XMLTagAttribute confNameAttr = XMLTagAttribute("name", nameResult);
            configurationTag.AddAttribute(confNameAttr);

            XMLTag constantResizeTag = XMLTag("constantResize");
                XMLTag resizeWidth = XMLTag("width", std::to_string((int) this->runnerEditor.GetProperty(RunnerProperty::IMAGE_WIDTH)));
                    constantResizeTag.AddTag(resizeWidth);

                XMLTag resizeHeight = XMLTag("height", std::to_string((int) this->runnerEditor.GetProperty(RunnerProperty::IMAGE_HEIGHT)));
                    constantResizeTag.AddTag(resizeHeight);

                configurationTag.AddTag(constantResizeTag);
            
            XMLTag preprocessorTag = XMLTag("preprocessor");
                double preprocessorTypeDouble = this->runner.GetPreprocessorProperty(PreProcessorProperty::IS_FULL);
                std::string preprocessorTypeString = (preprocessorTypeDouble == 0.0 ? "full" : "partial");
                XMLTagAttribute preprocessorTypeAttr = XMLTagAttribute("type", preprocessorTypeString);
                preprocessorTag.AddAttribute(preprocessorTypeAttr);

                XMLTag targetThreshold = XMLTag("targetThreshold");
                    XMLTag thresh = XMLTag("threshold", std::to_string((int) this->runner.GetPreprocessorProperty(PreProcessorProperty::THRESHOLD)));
                        targetThreshold.AddTag(thresh);

                    XMLTag maxValue = XMLTag("maxValue", std::to_string((int) this->runner.GetPreprocessorProperty(PreProcessorProperty::THRESH_VALUE)));
                        targetThreshold.AddTag(maxValue);

                    XMLTag threshType = XMLTag("type", "0");
                        targetThreshold.AddTag(threshType);

                    preprocessorTag.AddTag(targetThreshold);

                XMLTag dilationTag = XMLTag("dilation", std::to_string((int) this->runner.GetPreprocessorProperty(PreProcessorProperty::DILATION)));
                    preprocessorTag.AddTag(dilationTag);

                XMLTag colorTag = XMLTag("targetColor");
                    XMLTag hueTag = XMLTag("h", std::to_string((int) this->runner.GetPreprocessorProperty(PreProcessorProperty::COLOR_HUE)));
                        XMLTagAttribute hueError = XMLTagAttribute("error", std::to_string((int) this->runner.GetPreprocessorProperty(PreProcessorProperty::COLOR_ERROR)));
                        hueTag.AddAttribute(hueError);
                        colorTag.AddTag(hueTag);

                    XMLTag saturationTag = XMLTag("s", std::to_string((int) this->runner.GetPreprocessorProperty(PreProcessorProperty::COLOR_SATURATION)));
                        XMLTagAttribute saturationError = XMLTagAttribute("error", std::to_string((int) this->runner.GetPreprocessorProperty(PreProcessorProperty::COLOR_ERROR)));
                        saturationTag.AddAttribute(saturationError);
                        colorTag.AddTag(saturationTag);

                    XMLTag valueTag = XMLTag("v", std::to_string((int) this->runner.GetPreprocessorProperty(PreProcessorProperty::COLOR_VALUE)));
                        XMLTagAttribute valueError = XMLTagAttribute("error", std::to_string((int) this->runner.GetPreprocessorProperty(PreProcessorProperty::COLOR_ERROR)));
                        valueTag.AddAttribute(valueError);
                        colorTag.AddTag(valueTag);

                    preprocessorTag.AddTag(colorTag);

                configurationTag.AddTag(preprocessorTag);

            XMLTag postprocessorTag = XMLTag("postprocessor");
                XMLTag centerOffset = XMLTag("centerOffset", "0");
                    postprocessorTag.AddTag(centerOffset);

                XMLTag targetTag = XMLTag("target");
                    XMLTagAttribute targetIDAttr = XMLTagAttribute("id", "0");
                    targetTag.AddAttribute(targetIDAttr);

                    for(int i=0; i<this->runner.GetNumberOfTargets(); i++) {
                        XMLTag newContour = XMLTag("contour");
                            XMLTagAttribute ContourIDAttr = XMLTagAttribute("id", std::to_string(i));
                            newContour.AddAttribute(ContourIDAttr);

                            XMLTag contourX = XMLTag("x", std::to_string(this->runner.GetPostProcessorContourProperty(i, TargetProperty::DIST_X).Value()));
                                XMLTagAttribute contourXErr = XMLTagAttribute("error", std::to_string(this->runner.GetPostProcessorContourProperty(i, TargetProperty::DIST_X).Error()));
                                contourX.AddAttribute(contourXErr);
                                newContour.AddTag(contourX);

                            XMLTag contourY = XMLTag("y", std::to_string(this->runner.GetPostProcessorContourProperty(i, TargetProperty::DIST_Y).Value()));
                                XMLTagAttribute contourYErr = XMLTagAttribute("error", std::to_string(this->runner.GetPostProcessorContourProperty(i, TargetProperty::DIST_Y).Error()));
                                contourY.AddAttribute(contourYErr);
                                newContour.AddTag(contourY);

                            XMLTag contourAngle = XMLTag("angle", std::to_string(this->runner.GetPostProcessorContourProperty(i, TargetProperty::ANGLE).Value()));
                                XMLTagAttribute contourAngleErr = XMLTagAttribute("error", std::to_string(this->runner.GetPostProcessorContourProperty(i, TargetProperty::ANGLE).Error()));
                                contourAngle.AddAttribute(contourAngleErr);
                                newContour.AddTag(contourAngle);

                            XMLTag contourSolidity = XMLTag("solidity", std::to_string(this->runner.GetPostProcessorContourProperty(i, TargetProperty::SOLIDITY).Value()));
                                XMLTagAttribute contourSolidityErr = XMLTagAttribute("error", std::to_string(this->runner.GetPostProcessorContourProperty(i, TargetProperty::SOLIDITY).Error()));
                                contourSolidity.AddAttribute(contourSolidityErr);
                                newContour.AddTag(contourSolidity);

                            XMLTag contourAR = XMLTag("aspectRatio", std::to_string(this->runner.GetPostProcessorContourProperty(i, TargetProperty::ASPECT_RATIO).Value()));
                                XMLTagAttribute contourARErr = XMLTagAttribute("error", std::to_string(this->runner.GetPostProcessorContourProperty(i, TargetProperty::ASPECT_RATIO).Error()));
                                contourAR.AddAttribute(contourARErr);
                                newContour.AddTag(contourAR);

                            XMLTag contourMinArea = XMLTag("minimumArea", std::to_string(this->runner.GetPostProcessorContourProperty(i, TargetProperty::MINIMUM_AREA).Value()));
                                newContour.AddTag(contourMinArea);

                            targetTag.AddTag(newContour);
                    }

                    XMLTag targetKnownWidth = XMLTag("knownWidth", std::to_string((int) this->runner.GetRunnerProperty(RunnerProperty::TRUE_WIDTH)));
                        targetTag.AddTag(targetKnownWidth);

                    XMLTag targetFocalWidth = XMLTag("focalWidth", std::to_string((int) this->runner.GetRunnerProperty(RunnerProperty::PERCEIVED_WIDTH)));
                        targetTag.AddTag(targetFocalWidth);

                    XMLTag targetCalibratedDistance = XMLTag("calibratedDistance", std::to_string((int) this->runner.GetRunnerProperty(RunnerProperty::CALIBRATED_DISTANCE)));
                        targetTag.AddTag(targetCalibratedDistance);

                    XMLTag targetErrCorrect = XMLTag("distErrorCorrect", std::to_string((int) this->runner.GetRunnerProperty(RunnerProperty::ERROR_CORRECTION)));
                        targetTag.AddTag(targetErrCorrect);

                    postprocessorTag.AddTag(targetTag);

                    XMLTag UDPTag = XMLTag("UDP");
                        XMLTag udpAddrTag = XMLTag("address", this->runnerEditor.GetUDPAddress());
                            UDPTag.AddTag(udpAddrTag);

                        XMLTag udpPortTag = XMLTag("port", std::to_string(this->runnerEditor.GetUDPPort()));
                            UDPTag.AddTag(udpPortTag);

                        postprocessorTag.AddTag(UDPTag);

                configurationTag.AddTag(postprocessorTag);
            newDocument.AddTag(configurationTag);

    //use accessor so we don't accidently modify the member value
    std::string fileToWrite = this->GetFileName();
    if(fileToWrite == "confs/generic.xml") {
        fileToWrite = nameResult + ".xml";
        //prompt the user for a file to write to with FileChooser
        std::string defaultFile = "";
        for(int i=0; i<fileToWrite.length(); i++) {
            char nextChar = fileToWrite.at(i);
            if(nextChar == ' ') {
                defaultFile += "_";
            } else {
                defaultFile += nextChar;
            }
        }

        FileChooser fileChooser = FileChooser(true, defaultFile);
        fileToWrite = fileChooser.Show();
    }
    newDocument.WriteFile(fileToWrite);
}


void ConfigEditor::Close() {
    gtk_widget_destroy(this->configeditor);
}


void ConfigEditor::SetName(std::string name) {
    gtk_widget_set_name(this->configeditor, name.c_str());
}


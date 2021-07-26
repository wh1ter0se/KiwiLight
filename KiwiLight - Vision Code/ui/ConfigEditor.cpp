#include "../KiwiLight.h"

/**
 * Source file for the ConfigEditor class.
 * Written By: Brach Knutson
 */

using namespace cv;
using namespace KiwiLight;

static const int LEARNER_FRAMES = 50;
static const int MAX_CONTOURS_BEFORE_SUPPRESSION = 6;

/**
 * Called when the "Learn Target" editor button is pressed. 
 * This method activates the learner.
 */
static void LearnTargetButtonPressed() {
    KiwiLightApp::StartEditorLearningTarget();
}

/**
 * Called when the "Learn Distance" editor button is pressed.
 * Activates the distance learner.
 */
static void LearnDistanceButtonPressed() {
    KiwiLightApp::StartEditorLearningDistance();
}

/**
 * Called when the "Just Close" editor button is pressed.
 * Closes the editor window without saving the current configuration.
 */
static void JustCloseButtonPressed() {
    // KiwiLightApp::StopStreamingThread();
    KiwiLightApp::CloseEditor(false);
    // KiwiLightApp::LaunchStreamingThread(AppMode::UI_RUNNER);
}

/**
 * Called when the "Save and Close" editor button is pressed.
 * Saves the configuration being edited and closes the editor window.
 */
static void SaveAndCloseButtonPressed() {
    //save previous file name in case the editor fails to close due to save issues
    KiwiLightApp::CloseEditor(true);
}

/**
 * Creates a window to edit the passed file.
 */
ConfigEditor::ConfigEditor(std::string fileName) {
    this->learnerActivated = false;
    this->learnerFinished = false;
    std::vector<ExampleContour> sampleContourVector;
    sampleContourVector.push_back(ExampleContour(0));
    this->learnerResult = ExampleTarget(0, sampleContourVector, 0.0, 0.0, 0.0, 0.0, DistanceCalcMode::BY_WIDTH, 5);
    this->distanceLearnerRunning = false;
    this->runner = Runner(fileName, true);
    this->currentDoc = XMLDocument(fileName);
    this->fileName = fileName;
    this->lastIterationResult = "";
    this->out = Mat(Size(50, 50), CV_8UC3);
    this->updateShouldSkip = false;
    this->updating = false;
    this->confName = this->currentDoc.GetTagsByName("configuration")[0].GetAttributesByName("name")[0].Value();
    
    this->window = Window(GTK_WINDOW_TOPLEVEL, false);
        this->content = Panel(true, 0);
            Panel overviewPanel = Panel(false, 5);
                this->configOverview = OverviewPanel(this->currentDoc);
                    overviewPanel.Pack_start(this->configOverview.GetWidget(), true ,true, 5);

                Panel learnerPanel = Panel(true, 0);
                    Button learnTargetButton = Button("Learn Target", LearnTargetButtonPressed);
                        learnerPanel.Pack_start(learnTargetButton.GetWidget(), true, true, 0);

                    Button learnDistanceButton = Button("Learn Distance", LearnDistanceButtonPressed);
                        learnerPanel.Pack_start(learnDistanceButton.GetWidget(), true, true, 0);

                    overviewPanel.Pack_start(learnerPanel.GetWidget(), true, true, 0);

                Panel exitPanel = Panel(true, 0);
                    Button justCloseButton = Button("Close", JustCloseButtonPressed);
                        exitPanel.Pack_start(justCloseButton.GetWidget(), true, true, 0);

                    Button saveAndCloseButton = Button("Save And Close", SaveAndCloseButtonPressed);
                        exitPanel.Pack_start(saveAndCloseButton.GetWidget(), true, true, 0);

                    overviewPanel.Pack_start(exitPanel.GetWidget(), true, true, 0);

            Panel cameraSettingsPanel = Panel(false, 5);
                Label cameraSettingsHeader = Label("Camera");
                    cameraSettingsHeader.SetName("header");
                    cameraSettingsPanel.Pack_start(cameraSettingsHeader.GetWidget(), true, true, 0);
                    
                this->cameraSettings = Settings(this->currentDoc);
                    cameraSettingsPanel.Pack_start(this->cameraSettings.GetWidget(), true, false, 0);
            
            Panel preprocessorSettingsPanel = Panel(false, 5);
                Label preprocessorSettingsPanelHeader = Label("Preprocessor");
                    preprocessorSettingsPanelHeader.SetName("header");
                    preprocessorSettingsPanel.Pack_start(preprocessorSettingsPanelHeader.GetWidget(), true, true, 0);
                
                this->preprocessorSettings = PreprocessorEditor(this->runner.GetPreProcessor());
                    preprocessorSettingsPanel.Pack_start(this->preprocessorSettings.GetWidget(), true, true, 0);
            
            Panel postprocessorSettingsPanel = Panel(false, 5);
                Label postprocessorSettingsPanelHeader = Label("Postprocessor");
                    postprocessorSettingsPanelHeader.SetName("header");
                    postprocessorSettingsPanel.Pack_start(postprocessorSettingsPanelHeader.GetWidget(), true, true, 0);
                
                this->postprocessorSettings = PostprocessorEditor(this->runner.GetPostProcessor());
                    postprocessorSettingsPanel.Pack_start(this->postprocessorSettings.GetWidget(), true, true, 0);

            Panel runnerSettingsPanel = Panel(false, 5);
                Label runnerSettingsPanelHeader = Label("Misc.");
                    runnerSettingsPanelHeader.SetName("header");
                    runnerSettingsPanel.Pack_start(runnerSettingsPanelHeader.GetWidget(), true, true, 0);

                this->runnerSettings = RunnerEditor(this->runner);
                    runnerSettingsPanel.Pack_start(this->runnerSettings.GetWidget(), true, true, 0);

            this->tabs = TabView("Overview", overviewPanel.GetWidget());
            this->tabs.AddTab("Camera", cameraSettingsPanel.GetWidget());
            this->tabs.AddTab("Preprocessor", preprocessorSettingsPanel.GetWidget());
            this->tabs.AddTab("Postprocessor", postprocessorSettingsPanel.GetWidget());
            this->tabs.AddTab("Misc.", runnerSettingsPanel.GetWidget());
            this->content.Pack_start(this->tabs.GetWidget(), true, true, 0);

            Panel imageAndServicePanel = Panel(false, 0);
                this->outputImage = Image(ImageColorspace::RGB);
                    imageAndServicePanel.Pack_start(this->outputImage.GetWidget(), false, false, 0);

                this->serviceMonitor = Label("No Service Running.");
                    imageAndServicePanel.Pack_start(this->serviceMonitor.GetWidget(), false, false, 0);

                this->serviceLabel = Label("");
                    imageAndServicePanel.Pack_start(this->serviceLabel.GetWidget(), false, false, 0);
                
                this->content.Pack_start(imageAndServicePanel.GetWidget(), false, false, 0);


        this->window.SetPane(this->content);
    this->window.SetOnWindowClosed(JustCloseButtonPressed);
    this->window.SetCSS("ui/Style.css");
    this->window.Show();
    this->widget = this->window.GetWidget();
}

/**
 * Updates the editor tools and checks for button presses, etc.
 */
void ConfigEditor::Update() {
    if(!this->updateShouldSkip) {
        this->updating = true;
        Mat displayable;
    
        try {
            vconcat(this->original, this->out, displayable);
            this->outputImage.Update(displayable);
        } catch(cv::Exception ex) {
        }

        //set new ExampleTarget is the learner is finished
        if(this->learnerFinished) {
            SetTarget(this->learnerResult);
            this->learnerFinished = false;
        }

        //update the different tabs
        this->configOverview.SetTargetInformationLabelsFromString(this->lastIterationResult);
        this->preprocessorSettings.Update();
        this->postprocessorSettings.Update();
        this->runnerSettings.Update(this->runner.GetClosestTargetToCenter().Distance());
    
        //apply the preprocessor settings
        this->runner.SetPreprocessorProperty(PreProcessorProperty::IS_FULL, this->preprocessorSettings.GetProperty(PreProcessorProperty::IS_FULL));
        this->runner.SetPreprocessorProperty(PreProcessorProperty::THRESHOLD, this->preprocessorSettings.GetProperty(PreProcessorProperty::THRESHOLD));
        this->runner.SetPreprocessorProperty(PreProcessorProperty::EROSION, this->preprocessorSettings.GetProperty(PreProcessorProperty::EROSION));
        this->runner.SetPreprocessorProperty(PreProcessorProperty::DILATION, this->preprocessorSettings.GetProperty(PreProcessorProperty::DILATION));
        this->runner.SetPreprocessorProperty(PreProcessorProperty::COLOR_HUE, this->preprocessorSettings.GetProperty(PreProcessorProperty::COLOR_HUE));
        this->runner.SetPreprocessorProperty(PreProcessorProperty::COLOR_SATURATION, this->preprocessorSettings.GetProperty(PreProcessorProperty::COLOR_SATURATION));
        this->runner.SetPreprocessorProperty(PreProcessorProperty::COLOR_VALUE, this->preprocessorSettings.GetProperty(PreProcessorProperty::COLOR_VALUE));
        this->runner.SetPreprocessorProperty(PreProcessorProperty::COLOR_ERROR, this->preprocessorSettings.GetProperty(PreProcessorProperty::COLOR_ERROR));
    
        //apply all contour settings to the runner. First, make sure we have all contours needed.
        int numberOfContours = this->postprocessorSettings.GetNumContours();
        if(this->runner.NumberOfContours() != numberOfContours) {
            std::vector<ExampleContour> newContours;
            for(int i=0; i<numberOfContours; i++) {
                ExampleContour newContour = ExampleContour(i);
                newContours.push_back(newContour);
            }
            ExampleTarget newTarget = ExampleTarget(0, newContours, 0.0, 0.0, 0.0, 0.0, DistanceCalcMode::BY_WIDTH, 5);
            this->runner.SetExampleTarget(newTarget);
        }
    
        for(int i=0; i<numberOfContours; i++) {
            this->runner.SetPostProcessorContourProperty(i, TargetProperty::DIST_X, this->postprocessorSettings.GetProperty(i, TargetProperty::DIST_X));
            this->runner.SetPostProcessorContourProperty(i, TargetProperty::DIST_Y, this->postprocessorSettings.GetProperty(i, TargetProperty::DIST_Y));
            this->runner.SetPostProcessorContourProperty(i, TargetProperty::ANGLE, this->postprocessorSettings.GetProperty(i, TargetProperty::ANGLE));
            this->runner.SetPostProcessorContourProperty(i, TargetProperty::ASPECT_RATIO, this->postprocessorSettings.GetProperty(i, TargetProperty::ASPECT_RATIO));
            this->runner.SetPostProcessorContourProperty(i, TargetProperty::SOLIDITY, this->postprocessorSettings.GetProperty(i, TargetProperty::SOLIDITY));
            this->runner.SetPostProcessorContourProperty(i, TargetProperty::MINIMUM_AREA, this->postprocessorSettings.GetProperty(i, TargetProperty::MINIMUM_AREA));
        }
    
        //apply runner properties
        this->runner.SetRunnerProperty(RunnerProperty::OFFSET_X, this->runnerSettings.GetProperty(RunnerProperty::OFFSET_X));
        this->runner.SetRunnerProperty(RunnerProperty::OFFSET_Y, this->runnerSettings.GetProperty(RunnerProperty::OFFSET_Y));
        this->runner.SetRunnerProperty(RunnerProperty::IMAGE_WIDTH, this->runnerSettings.GetProperty(RunnerProperty::IMAGE_WIDTH));
        this->runner.SetRunnerProperty(RunnerProperty::IMAGE_HEIGHT, this->runnerSettings.GetProperty(RunnerProperty::IMAGE_HEIGHT));
        this->runner.SetRunnerProperty(RunnerProperty::TRUE_WIDTH, this->runnerSettings.GetProperty(RunnerProperty::TRUE_WIDTH));
        this->runner.SetRunnerProperty(RunnerProperty::PERCEIVED_WIDTH, this->runnerSettings.GetProperty(RunnerProperty::PERCEIVED_WIDTH));
        this->runner.SetRunnerProperty(RunnerProperty::CALIBRATED_DISTANCE, this->runnerSettings.GetProperty(RunnerProperty::CALIBRATED_DISTANCE));
        this->runner.SetRunnerProperty(RunnerProperty::ERROR_CORRECTION, this->runnerSettings.GetProperty(RunnerProperty::ERROR_CORRECTION));
        this->runner.SetRunnerProperty(RunnerProperty::CALC_DIST_BY_HEIGHT, this->runnerSettings.GetProperty(RunnerProperty::CALC_DIST_BY_HEIGHT));
        this->runner.SetRunnerProperty(RunnerProperty::MAX_CONTOURS, this->runnerSettings.GetProperty(RunnerProperty::MAX_CONTOURS));
        KiwiLightApp::SetUDPMaxSendRate(this->runnerSettings.GetMaxSendRate());
    
        //set service labels
        if(this->learnerActivated && this->learner.GetLearning()) {
            this->serviceMonitor.SetText("Learning Target");
    
            std::string progressString = "Capturing Frames (" +
                                      std::to_string(this->learner.GetFramesLearned()) +
                                      "/" +
                                      std::to_string(LEARNER_FRAMES) + 
                                      ")";
            
            this->serviceLabel.SetText(progressString);
        } else if(this->distanceLearnerRunning) {
            this->serviceMonitor.SetText("Learning Distance Constants");
    
            std::string progressString = "Capturing Frames (" +
                                      std::to_string(this->distanceLearner.GetFramesLearned()) +
                                      "/" +
                                      std::to_string(LEARNER_FRAMES) + 
                                      ")";
            this->serviceLabel.SetText(progressString);
        } else {
            this->serviceMonitor.SetText("No Service Running.");
            this->serviceLabel.SetText("");
        }

        //check target. If there are too many contours, performance errors will occur.
        if(this->runner.GetExampleTarget().Contours().size() > MAX_CONTOURS_BEFORE_SUPPRESSION) {
            //restore generic target
            ExampleTarget genericTarget = Runner(Util::ResolveGenericConfFilePath(), false, false).GetExampleTarget();
            SetTarget(genericTarget);
            
            //alert the user that the new target was discarded
            ConfirmationDialog alert = ConfirmationDialog(
                std::string("The Target KiwiLight just tried to learn had too many contours!\n") + 
                std::string("Please ensure that the target has less than " + std::to_string(MAX_CONTOURS_BEFORE_SUPPRESSION) + " contours.\n") +
                std::string("Reverting to generic target.")
            );
            alert.ShowAndGetResponse();
        }
        this->updating = false;
    }
}


/**
 * Updates the internal runner to in turn update the output images.
 * Also updates services that are dependent on the images.
 */
bool ConfigEditor::UpdateImageOnly() {
    bool successful = false;

    //update services
    if(this->learnerActivated) {
        if(LearnDialogActive()) { //if the learn dialog is active, use the minimum area slider on it to set minimum area.
            int minimumArea = (int) this->learnDialogMinArea.GetValue();
            this->learner.SetMinimumArea(minimumArea);
        }

        this->learner.Feed();

        if(this->learner.GetLearning()) {
            if(this->learner.GetFramesLearned() >= LEARNER_FRAMES) {
                //process new target
                this->updateShouldSkip = true;
                //wait for current update to finish
                while(this->updating) {
                    usleep(1000);
                }
                int minimumArea = (int) this->postprocessorSettings.GetProperty(postprocessorSettings.GetCurrentContour(), TargetProperty::MINIMUM_AREA).Value();
                ExampleTarget newTarget = this->learner.StopLearning();
                this->learnerActivated = false;

                if(newTarget.Contours().size() > 0) {
                    //mark as done
                    this->learnerFinished = true;
                    this->learnerResult = newTarget;
                }
                this->updateShouldSkip = false;
            }

            if(this->learner.GetHasFailed()) {
                //alert the user
                ConfirmationDialog alert = ConfirmationDialog(
                    std::string("The utility has failed due to a video error.\n") +
                    std::string("Is the camera plugged in?")
                );
                alert.ShowAndGetResponse();
                this->learnerActivated = false;
            }
        }
    }
    
    if(this->distanceLearnerRunning) {
        if(this->runner.GetLastFrameTargets().size() > 0) {
            this->distanceLearner.FeedTarget(this->runner.GetClosestTargetToCenter());
        } else {
            this->distanceLearner.FeedBlank();
        }

        if(this->distanceLearner.GetFramesLearned() >= LEARNER_FRAMES) {
            this->updateShouldSkip = true;

            //wait for current update to finish
            while(this->updating) {
                usleep(1000);
            }

            double trueDistance = this->runnerSettings.GetProperty(RunnerProperty::CALIBRATED_DISTANCE);
            double trueWidth = this->runnerSettings.GetProperty(RunnerProperty::TRUE_WIDTH);
            double newFocalWidth = this->distanceLearner.GetFocalWidth(trueDistance, trueWidth);
            if(newFocalWidth > -1) {
                this->runnerSettings.SetProperty(RunnerProperty::PERCEIVED_WIDTH, newFocalWidth);
            }

            this->distanceLearnerRunning = false;
            this->updateShouldSkip = false;
        }
    }

    //return without success if last target had too many contours.
    if(this->runner.GetExampleTarget().Contours().size() > MAX_CONTOURS_BEFORE_SUPPRESSION) {
        return false;
    }

    //return with success of runner if the target learner is not running
    if(!this->learnerActivated) { 
        //this if statement prevents the spaz attack that happens when the "learn target" button was pressed.
        //previously, the images kept switching between the learner ouput and the real output.
        this->lastIterationResult = this->runner.Iterate();
        successful = this->runner.GetLastFrameSuccessful();
        this->original = this->runner.GetOriginalImage();
        this->out = this->runner.GetOutputImage();
    } else {
        //otherwise (if the target learner is running), return with the success of the target learner.
        successful = !learner.GetHasFailed();
        this->out = this->learner.GetOutputImageFromLastFeed();
        this->original = this->learner.GetOriginalImageFromLastFeed();
    }

    return successful;
}

/**
 * Returns the string result from the last runner iteration, or an empty string if no iterations happened.
 */
std::string ConfigEditor::GetLastFrameResult() {
    return this->lastIterationResult;
}

/**
 * Causes the editor to save the config to it's file.
 */
bool ConfigEditor::Save() {
    //assemble the file structure and write it into a file which the user may designate
    XMLDocument doc = XMLDocument();
    
        //<camera>
        XMLTag camera = XMLTag("camera");
            XMLTagAttribute cameraIndex = XMLTagAttribute("index", std::to_string(this->cameraSettings.GetCameraIndex()));
                camera.AddAttribute(cameraIndex);
                                
            /**
             * <camera>
             *  <resolution>
             */
            XMLTag resolution = XMLTag("resolution");
                XMLTag resolutionWidth = XMLTag("width", std::to_string(this->cameraSettings.GetSettingValueFromID(CAP_PROP_FRAME_WIDTH)));
                    resolution.AddTag(resolutionWidth);

                XMLTag resolutionHeight = XMLTag("height", std::to_string(this->cameraSettings.GetSettingValueFromID(CAP_PROP_FRAME_HEIGHT)));
                    resolution.AddTag(resolutionHeight);

                camera.AddTag(resolution);
                                
            /**
             * <camera>
             *  <settings>
             */
            XMLTag settings = this->cameraSettings.GetFinishedTag();
                camera.AddTag(settings);
            
            doc.AddTag(camera);
                        
        /**
         * <configuration>
         */
        XMLTag configuration = XMLTag("configuration");
            XMLTagAttribute configurationName = XMLTagAttribute("name", this->configOverview.GetConfigName());
                configuration.AddAttribute(configurationName);
                                
            /**
             * <configuration>
             *  <cameraOffset>
             */
            XMLTag cameraOffset = XMLTag("cameraOffset");
                XMLTag horizontalOffset = XMLTag("horizontal", std::to_string((int) this->runnerSettings.GetProperty(RunnerProperty::OFFSET_X)));
                    cameraOffset.AddTag(horizontalOffset);

                XMLTag verticalOffset = XMLTag("vertical", std::to_string((int) this->runnerSettings.GetProperty(RunnerProperty::OFFSET_Y)));
                    cameraOffset.AddTag(verticalOffset);

                configuration.AddTag(cameraOffset);
                                
            /**
             * <configuration>
             *  <constantResize>
             */
            XMLTag constantResize = XMLTag("constantResize");
                XMLTag resizeWidth = XMLTag("width", std::to_string((int) this->runnerSettings.GetProperty(RunnerProperty::IMAGE_WIDTH)));
                    constantResize.AddTag(resizeWidth);

                XMLTag resizeHeight = XMLTag("height", std::to_string((int) this->runnerSettings.GetProperty(RunnerProperty::IMAGE_HEIGHT)));
                    constantResize.AddTag(resizeHeight);

                configuration.AddTag(constantResize);
                                
            /**
             * <configuration>
             *  <preprocessor>
             */
            XMLTag preprocessor = XMLTag("preprocessor");
                XMLTagAttribute preprocessorType = XMLTagAttribute("type", (this->preprocessorSettings.GetProperty(PreProcessorProperty::IS_FULL) == 1.0 ? "full" : "partial"));
                    preprocessor.AddAttribute(preprocessorType);
                    
                //<threshold>
                XMLTag threshold = XMLTag("threshold", std::to_string((int) this->preprocessorSettings.GetProperty(PreProcessorProperty::THRESHOLD)));
                    preprocessor.AddTag(threshold);
                    
                //<erosion>
                XMLTag erosion = XMLTag("erosion", std::to_string((int) this->preprocessorSettings.GetProperty(PreProcessorProperty::EROSION)));
                    preprocessor.AddTag(erosion);
                    
                //<dilation>
                XMLTag dilation = XMLTag("dilation", std::to_string((int) this->preprocessorSettings.GetProperty(PreProcessorProperty::DILATION)));
                    preprocessor.AddTag(dilation);
                    
                /**
                 * <configuration>
                 *  <preprocessor>
                 *      <targetColor>
                 */
                XMLTag targetColor = XMLTag("targetColor");
                    XMLTagAttribute targetColorError = XMLTagAttribute("error", std::to_string((int) this->preprocessorSettings.GetProperty(PreProcessorProperty::COLOR_ERROR)));
                        targetColor.AddAttribute(targetColorError);
                
                    //<h>
                    XMLTag h = XMLTag("h", std::to_string((int) this->preprocessorSettings.GetProperty(PreProcessorProperty::COLOR_HUE)));
                        targetColor.AddTag(h);

                    //<s>
                    XMLTag s = XMLTag("s", std::to_string((int) this->preprocessorSettings.GetProperty(PreProcessorProperty::COLOR_SATURATION)));
                        targetColor.AddTag(s);

                    //<v>
                    XMLTag v = XMLTag("v", std::to_string((int) this->preprocessorSettings.GetProperty(PreProcessorProperty::COLOR_VALUE)));
                        targetColor.AddTag(v);

                    preprocessor.AddTag(targetColor);
                configuration.AddTag(preprocessor);
            
            /**
             * <configuration>
             *  <postprocessor>
             */
            XMLTag postprocessor = XMLTag("postprocessor");
                //<target>
                XMLTag target = XMLTag("target");
                    XMLTagAttribute targetID = XMLTagAttribute("id", "0"); //this remains constant for now until multiple target support is added.
                        target.AddAttribute(targetID);
                    
                    for(int i=0; i<this->postprocessorSettings.GetNumContours(); i++) {
                        /**
                         * <configuration>
                         *  <postprocessor>
                         *      <target>
                         *          <contour>
                         */
                        XMLTag contour = XMLTag("contour");
                            XMLTagAttribute contourID = XMLTagAttribute("id", std::to_string(i));
                                contour.AddAttribute(contourID);

                            //<x>
                            SettingPair distXValues = this->postprocessorSettings.GetProperty(i, TargetProperty::DIST_X);
                            XMLTag x = XMLTag("x", std::to_string((double) distXValues.Value()));
                                XMLTagAttribute xError = XMLTagAttribute("error", std::to_string((double) distXValues.Error()));
                                    x.AddAttribute(xError);
                                contour.AddTag(x);

                            //<y>
                            SettingPair distYValues = this->postprocessorSettings.GetProperty(i, TargetProperty::DIST_Y);
                            XMLTag y = XMLTag("y", std::to_string((double) distYValues.Value()));
                                XMLTagAttribute yError = XMLTagAttribute("error", std::to_string((double) distYValues.Error()));
                                    y.AddAttribute(yError);
                                contour.AddTag(y);
                            
                            //<angle>
                            SettingPair angleValues = this->postprocessorSettings.GetProperty(i, TargetProperty::ANGLE);
                            XMLTag angle = XMLTag("angle", std::to_string((double) angleValues.Value()));
                                XMLTagAttribute angleError = XMLTagAttribute("error", std::to_string((double) angleValues.Error()));
                                    angle.AddAttribute(angleError);
                                contour.AddTag(angle);

                            //<solidity>
                            SettingPair solidityValues = this->postprocessorSettings.GetProperty(i, TargetProperty::SOLIDITY);
                            XMLTag solidity = XMLTag("solidity", std::to_string((double) solidityValues.Value()));
                                XMLTagAttribute solidityError = XMLTagAttribute("error", std::to_string((double) solidityValues.Error()));
                                    solidity.AddAttribute(solidityError);
                                contour.AddTag(solidity);
                            
                            //<aspectRatio>
                            SettingPair aspectRatioValues = this->postprocessorSettings.GetProperty(i, TargetProperty::ASPECT_RATIO);
                            XMLTag aspectRatio = XMLTag("aspectRatio", std::to_string((double) aspectRatioValues.Value()));
                                XMLTagAttribute aspectRatioError = XMLTagAttribute("error", std::to_string((double) aspectRatioValues.Error()));
                                    aspectRatio.AddAttribute(aspectRatioError);
                                contour.AddTag(aspectRatio);
                            
                            //<minimumArea>
                            SettingPair minimumAreaValues = this->postprocessorSettings.GetProperty(i, TargetProperty::MINIMUM_AREA);
                            XMLTag minimumArea = XMLTag("minimumArea", std::to_string((int) minimumAreaValues.Value()));
                                contour.AddTag(minimumArea);

                            target.AddTag(contour);
                    }
                    
                    //<knownWidth>
                    XMLTag knownWidth = XMLTag("knownWidth", std::to_string((double) this->runnerSettings.GetProperty(RunnerProperty::TRUE_WIDTH)));
                        target.AddTag(knownWidth);

                    //<focalWidth>
                    XMLTag focalWidth = XMLTag("focalWidth", std::to_string((double) this->runnerSettings.GetProperty(RunnerProperty::PERCEIVED_WIDTH)));
                        target.AddTag(focalWidth);

                    //<calibratedDistance>
                    XMLTag calibratedDistance = XMLTag("calibratedDistance", std::to_string((double) this->runnerSettings.GetProperty(RunnerProperty::CALIBRATED_DISTANCE)));
                        target.AddTag(calibratedDistance);

                    //<distErrorCorrect>
                    XMLTag distErrorCorrect = XMLTag("distErrorCorrect", std::to_string((double) this->runnerSettings.GetProperty(RunnerProperty::ERROR_CORRECTION)));
                        target.AddTag(distErrorCorrect);

                    //<calcByHeight>
                    XMLTag calcByHeight = XMLTag("calcByHeight", (this->runnerSettings.GetProperty(RunnerProperty::CALC_DIST_BY_HEIGHT) == 1 ? "true" : "false"));
                        target.AddTag(calcByHeight);

                    //<maxContours>
                    XMLTag maxContours = XMLTag("maxContours", std::to_string((int) this->runnerSettings.GetProperty(RunnerProperty::MAX_CONTOURS)));
                        target.AddTag(maxContours);

                    postprocessor.AddTag(target);
                                    
                /**
                 * <configuration>
                 *  <postprocessor>
                 *      <UDP>
                 */
                XMLTag UDP = XMLTag("UDP");
                    //<address>
                    XMLTag address = XMLTag("address", this->runnerSettings.GetUDPAddr());
                        UDP.AddTag(address);

                    //<port>
                    XMLTag port = XMLTag("port", std::to_string(this->runnerSettings.GetUDPPort()));
                        UDP.AddTag(port);

                    //<maxSendRate>
                    XMLTag maxSendRate = XMLTag("maxSendRate", std::to_string(this->runnerSettings.GetMaxSendRate()));
                        UDP.AddTag(maxSendRate);
                        
                    postprocessor.AddTag(UDP);
                configuration.AddTag(postprocessor);
            doc.AddTag(configuration);
                
    //to prompt file name or not to promt file name
    std::string fileToSave = this->fileName;
    std::vector<std::string> fileParts = Util::SplitString(fileToSave, '/');
    
    if(fileParts[fileParts.size() - 1] == "generic.xml") {
        FileChooser chooser = FileChooser(true, "kiwilight-config.xml");
        fileToSave = chooser.Show();
        if(fileToSave == "") {
            //user pressed "cancel" or did not select a file, abort
            return false;
        }

        this->fileName = fileToSave;
    }
    
    doc.WriteFile(fileToSave);

    return true;
}

/**
 * Closes and destroys the editor window.
 */
void ConfigEditor::Close() {
    updateShouldSkip = true;

    //wait for updating to stop
    while(updating) {
        usleep(1000);
    }
    gtk_widget_destroy(this->widget);
}

/**
 * Sets the color of the target, in HSV colorspace.
 */
void ConfigEditor::SetTargetColor(int h, int s, int v) {
    preprocessorSettings.SetProperty(PreProcessorProperty::COLOR_HUE, h);
    preprocessorSettings.SetProperty(PreProcessorProperty::COLOR_SATURATION, s);
    preprocessorSettings.SetProperty(PreProcessorProperty::COLOR_VALUE, v);
}

/**
 * Activates the target learning tool and prompts instructions for the user to follow to learn the target.
 */
void ConfigEditor::StartLearningTarget() {
    if(KiwiLightApp::CameraOpen()) {
        //reinstantiate the learner to apply the preprocessor settings
        this->learner = ConfigLearner(this->runner.GetPreProcessor(), this->runner.GetConstantSize());
        this->learnerActivated = true;
        
        ConfirmationDialog confirmLearn = ConfirmationDialog(
            std::string("Position the target in the center of the image and press OK.\n") +
            std::string("It should be highlighted with a blue box. If not, adjust the\n") +
            std::string("area of the contours by using the slider.")
        );
        Panel minimumAreaPanel = Panel(true, 0);
            double realMinimumArea = postprocessorSettings.GetProperty(postprocessorSettings.GetCurrentContour(), TargetProperty::MINIMUM_AREA).Value();
            this->learnDialogMinArea = LabeledSlider("Minimum Area", 5, 2500, 5, realMinimumArea);
                minimumAreaPanel.Pack_start(learnDialogMinArea.GetWidget(), true, true, 5);
            
            confirmLearn.SetBody(minimumAreaPanel);

        bool shouldLearn = confirmLearn.ShowAndGetResponse();
        
        if(shouldLearn) {
            this->serviceMonitor.SetText("Learning Target");
            this->serviceLabel.SetText("Capturing Frames");
            this->learner.StartLearning();
        } else {
            this->learnerActivated = false;
        }
    } else {
        //the camera is not opened, alert the user
        ConfirmationDialog alert = ConfirmationDialog(
            std::string("The utility could not start because of a video error.\n") +
            std::string("Is the camera plugged in and working correctly on your device?")
        );

        alert.ShowAndGetResponse();
    }
}

/**
 * Activates the Distance learner and prompts instructions for the user to follow to learn distance.
 */
void ConfigEditor::StartLearningDistance() {
    //check for video errors
    if(KiwiLightApp::CameraOpen()) {
        ConfirmationDialog informationDialog = ConfirmationDialog("Learn Distance");
        Panel dialogPanel = Panel(false, 0);
            Panel trueWidthPanel = Panel(true, 0);
                Label trueWidthPanelHeader = Label("Target Width: ");
                    trueWidthPanel.Pack_start(trueWidthPanelHeader.GetWidget(), false, false, 0);

                double realTrueWidth = this->runnerSettings.GetProperty(RunnerProperty::TRUE_WIDTH);
                NumberBox trueWidthPanelValue = NumberBox(0.0, 120.0, 0.1, realTrueWidth);
                    trueWidthPanel.Pack_start(trueWidthPanelValue.GetWidget(), false, false, 0);

                dialogPanel.Pack_start(trueWidthPanel.GetWidget(), false, false, 0);

            Panel distancePanel = Panel(true, 0);
                Label distancePanelHeader = Label("Distance From Camera: ");
                    distancePanel.Pack_start(distancePanelHeader.GetWidget(), false, false, 0);

                double realDistance = this->runnerSettings.GetProperty(RunnerProperty::CALIBRATED_DISTANCE);
                NumberBox distanceValue = NumberBox(6.0, 240.0, 0.1, realDistance);
                    distancePanel.Pack_start(distanceValue.GetWidget(), false, false, 0);

                dialogPanel.Pack_start(distancePanel.GetWidget(), false, false, 0);
            informationDialog.SetBody(dialogPanel);

        //show the dialog anc ask the question, but do not destroy the dialog immediately when the user presses OK
        bool shouldLearn = informationDialog.ShowButDontClose();
        double targetTrueWidth = trueWidthPanelValue.GetValue();
        double targetDistance = distanceValue.GetValue();
        informationDialog.Destroy();

        if(shouldLearn) {
            this->runnerSettings.SetProperty(RunnerProperty::TRUE_WIDTH, targetTrueWidth);
            this->runnerSettings.SetProperty(RunnerProperty::CALIBRATED_DISTANCE, targetDistance);

            this->distanceLearner = TargetDistanceLearner(this->runner.GetPreProcessor(), this->runner.GetPostProcessor());
            this->distanceLearnerRunning = true;

            this->serviceMonitor.SetText("Learning Distance Constants");
            this->serviceLabel.SetText("Capturing Frames");
        }
    } else {
        //the camera is not opened, alert the user
        ConfirmationDialog alert = ConfirmationDialog(
            std::string("The utility could not be opened because of a video error.\n") +
            std::string("Is the camera plugged in and working correctly on your device?")
        );

        alert.ShowAndGetResponse();
    }
}

/**
 * Reconnects the KiwiLight socket sender to the IPv4 address and port found in the Runner tab.
 */
void ConfigEditor::ReconnectUDPFromEditor() {
    std::string newUDPAddr = this->runnerSettings.GetUDPAddr();
    int newUDPPort = this->runnerSettings.GetUDPPort();
    KiwiLightApp::ReconnectUDP(newUDPAddr, newUDPPort);

    //set the things in the overview panel
    this->configOverview.SetUDPAddr(newUDPAddr);
    this->configOverview.SetUDPPort(newUDPPort);
}

/**
 * Sets the "Enable/Disable" UDP button texts based on the value of "UDPEnabled"
 */
void ConfigEditor::SetUDPEnabledLabels(bool UDPEnabled) {
    this->configOverview.SetUDPEnabledLabels(UDPEnabled);
    this->runnerSettings.SetUDPEnabledLabels(UDPEnabled);
}

/**
 * Applies the settings from the camera tab to the camera itself.
 */
void ConfigEditor::ApplyCameraSettings() {
    std::vector<int> settingIDs = this->cameraSettings.GetSettingIDs();
    int newCameraIndex = this->cameraSettings.GetCameraIndex();

    //open a new camera because the user wants to
    KiwiLightApp::OpenNewCameraOnIndex(newCameraIndex);
    
    //tell the runner to apply each setting
    for(int i=0; i<settingIDs.size(); i++) {
        int id = settingIDs[i];
        double value = this->cameraSettings.GetSettingValueFromID(id);
        KiwiLightApp::SetCameraProperty(id, value);
    }
    
    //get the settings and apply them to the menu to tell the user if they were rejected or not
    for(int i=0; i<settingIDs.size(); i++) {
        int id = settingIDs[i];
        double value = KiwiLightApp::GetCameraProperty(id);
        this->cameraSettings.SetSettingValueFromID(id, value);
    }
}


void ConfigEditor::ApplyFRCCameraSettings() {
    this->cameraSettings.ApplyFRCSettings();
}

/**
 * Sets the text of the Camera ID number boxes to the value of "index."
 */
void ConfigEditor::SetCameraIndexBoxes(int index) {
    this->configOverview.SetCameraIndex(index);
    this->cameraSettings.SetCameraIndex(index);
}

/**
 * Reconnects the KiwiLight socket sender to the IPv4 address and port found in the Runner tab.
 */
void ConfigEditor::ReconnectUDPFromOverview() {
    std::string newAddr = this->configOverview.GetUDPAddr();
    int newPort = this->configOverview.GetUDPPort();
    KiwiLightApp::ReconnectUDP(newAddr, newPort);

    //set the properties in the actual editor
    this->runnerSettings.SetUDPAddr(newAddr);
    this->runnerSettings.SetUDPPort(newPort);
}

/**
 * Sets the index of the KiwiLight camera to the value found on the Overview ID widget.
 */
void ConfigEditor::OpenNewCameraFromOverview() {
    int newCameraIndex = this->configOverview.GetCameraIndex();

    //the camera needs to be reconnected because indexes are different
    this->cameraSettings.SetCameraIndex(newCameraIndex); //set real index
    KiwiLightApp::OpenNewCameraOnIndex(newCameraIndex);
}

/**
 * Updates the Image widget found to the right side of the editor.
 */
void ConfigEditor::UpdateImage() {
    this->runner.Iterate();
    Mat displayable;

    try {
        vconcat(this->original, this->out, displayable);
        this->outputImage.Update(displayable);
    } catch(cv::Exception ex) {
        std::cout << "cv exception in config editor" << std::endl;
    }
}

void ConfigEditor::SetTarget(ExampleTarget target) {
    std::vector<ExampleContour> newContours = target.Contours();
    this->postprocessorSettings.SetNumContours(newContours.size());
    this->runner.SetExampleTarget(target);

    for(int i=0; i<newContours.size(); i++) {
        this->postprocessorSettings.SetProperty(i, TargetProperty::DIST_X, newContours[i].DistX());
        this->postprocessorSettings.SetProperty(i, TargetProperty::DIST_Y, newContours[i].DistY());
        this->postprocessorSettings.SetProperty(i, TargetProperty::ANGLE, newContours[i].Angle());
        this->postprocessorSettings.SetProperty(i, TargetProperty::SOLIDITY, newContours[i].Solidity());
        this->postprocessorSettings.SetProperty(i, TargetProperty::ASPECT_RATIO, newContours[i].AspectRatio());
        this->postprocessorSettings.SetProperty(i, TargetProperty::MINIMUM_AREA, SettingPair(newContours[i].MinimumArea(), 0));
    }
}

/**
 * Returns true if the learn confirmation dialog is active, and false otherwise.
 */
bool ConfigEditor::LearnDialogActive() {
    return learnerActivated && !learner.GetLearning();
}

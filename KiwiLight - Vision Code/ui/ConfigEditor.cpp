#include "../KiwiLight.h"

/**
 * Source file for the ConfigEditor class.
 * Written By: Brach Knutson
 */

using namespace cv;
using namespace KiwiLight;

static int LEARNER_FRAMES = 50;

static void LearnTargetButtonPressed() {
    KiwiLightApp::StartEditorLearningTarget();
}

static void LearnDistanceButtonPressed() {
    KiwiLightApp::StartEditorLearningDistance();
}

static void JustCloseButtonPressed() {
    KiwiLightApp::CloseEditor(false);
}

static void SaveAndCloseButtonPressed() {
    KiwiLightApp::CloseEditor(true);
}

/**
 * Creates a window to edit the bassed file.
 */
ConfigEditor::ConfigEditor(std::string fileName, VideoCapture cap) {
    this->learnerActivated = false;
    this->distanceLearnerRunning = false;
    this->runner = Runner(fileName, true, cap);
    this->currentDoc = XMLDocument(fileName);
    this->fileName = fileName;
    this->lastIterationResult = "";
    this->out = Mat(Size(50, 50), CV_8UC3);
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
                Label cameraSettingsHeader = Label("Camera Settings");
                    cameraSettingsHeader.SetName("header");
                    cameraSettingsPanel.Pack_start(cameraSettingsHeader.GetWidget(), true, true, 0);
                this->cameraSettings = Settings(cap, this->currentDoc);
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
                Label runnerSettingsPanelHeader = Label("Runner");
                    runnerSettingsPanelHeader.SetName("header");
                    runnerSettingsPanel.Pack_start(runnerSettingsPanelHeader.GetWidget(), true, true, 0);

                this->runnerSettings = RunnerEditor(this->runner);
                    runnerSettingsPanel.Pack_start(this->runnerSettings.GetWidget(), true, true, 0);

            this->tabs = TabView("Overview", overviewPanel.GetWidget());
            this->tabs.AddTab("Camera", cameraSettingsPanel.GetWidget());
            this->tabs.AddTab("Preprocessor", preprocessorSettingsPanel.GetWidget());
            this->tabs.AddTab("Postprocessor", postprocessorSettingsPanel.GetWidget());
            this->tabs.AddTab("Runner", runnerSettingsPanel.GetWidget());
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
    this->window.SetOnWindowClosed(ConfigEditor::Closed);
    this->window.SetCSS("ui/Style.css");
    this->window.Show();

    this->configeditor = this->window.GetWidget();
}

/**
 * Updates the editor and checks for button presses, etc.
 */
void ConfigEditor::Update() {
    Mat displayable;

    try {
        vconcat(this->original, this->out, displayable);
        this->outputImage.Update(displayable);
    } catch(cv::Exception ex) {
    }
    
    this->cameraSettings.Update();
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

    //apply all contour settings to the runner
    for(int i=0; i<this->postprocessorSettings.GetNumContours(); i++) {
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
}


/**
 * Updates the internal runner to in turn update the output images.
 */
bool ConfigEditor::UpdateImageOnly() {
    this->lastIterationResult = this->runner.Iterate();
    bool retval = this->runner.GetLastFrameSuccessful();
    this->out = this->runner.GetOutputImage();
    this->original = this->runner.GetOriginalImage();
        
    if(this->learnerActivated) {
        int minimumArea = (int) this->postprocessorSettings.GetProperty(0, TargetProperty::MINIMUM_AREA).Value();
        this->learner.FeedImage(this->original, minimumArea);
        this->out = this->learner.GetOutputImageFromLastFeed();

        if(this->learner.GetLearning()) {
            std::string progressString = "Capturing Frames (" +
                                      std::to_string(this->learner.GetFramesLearned()) +
                                      "/" +
                                      std::to_string(LEARNER_FRAMES) + 
                                      ")";
            
            this->serviceLabel.SetText(progressString);

            if(this->learner.GetFramesLearned() >= LEARNER_FRAMES) {
                int minimumArea = (int) this->postprocessorSettings.GetProperty(0, TargetProperty::MINIMUM_AREA).Value();
                ExampleTarget newTarget = this->learner.StopLearning(minimumArea);

                std::vector<ExampleContour> newContours = newTarget.Contours();
                for(int i=0; i<newContours.size(); i++) {
                    this->postprocessorSettings.SetProperty(i, TargetProperty::DIST_X, newContours[i].DistX());
                    this->postprocessorSettings.SetProperty(i, TargetProperty::DIST_Y, newContours[i].DistY());
                    this->postprocessorSettings.SetProperty(i, TargetProperty::ANGLE, newContours[i].Angle());
                    this->postprocessorSettings.SetProperty(i, TargetProperty::SOLIDITY, newContours[i].Solidity());
                    this->postprocessorSettings.SetProperty(i, TargetProperty::ASPECT_RATIO, newContours[i].AspectRatio());
                    this->postprocessorSettings.SetProperty(i, TargetProperty::MINIMUM_AREA, SettingPair(newContours[i].MinimumArea(), 0));
                }

                this->serviceMonitor.SetText("No Service Running.");
                this->serviceLabel.SetText("");
                this->learnerActivated = false;
            }

            if(this->learner.GetHasFailed()) {
                this->serviceMonitor.SetText("No Service Running.");
                this->serviceLabel.SetText("");
                this->learnerActivated = false;

                //alert the user
                ConfirmationDialog alert = ConfirmationDialog(
                    std::string("The utility has failed due to a video error.\n") +
                    std::string("Is the camera plugged in?")
                );
                alert.ShowAndGetResponse();
            }
        }
    }
    
    if(this->distanceLearnerRunning) {
        this->distLearner.FeedTarget(this->runner.GetClosestTargetToCenter());

        std::string distProgressString = "Capturing Frames (" +
                                         std::to_string(this->distanceLearner.GetFramesLearned()) +
                                         "/" +
                                         std::to_string(LEARNER_FRAMES) +
                                         ")";
        
        this->serviceLabel.SetText(distProgressString);

        if(this->distLearner.GetFramesLearned() >= LEARNER_FRAMES) {
            double trueDistance = this->runnerSettings.GetProperty(RunnerProperty::CALIBRATED_DISTANCE);
            double trueWidth = this->runnerSettings.GetProperty(RunnerProperty::TRUE_WIDTH);
            double newFocalWidth = this->distLearner.GetFocalWidth(trueDistance, trueWidth);
            this->runnerSettings.SetProperty(RunnerProperty::PERCEIVED_WIDTH, newFocalWidth);

            //reset the labels
            this->serviceMonitor.SetText("No Service Running.");
            this->serviceLabel.SetText("");
            this->distanceLearnerRunning = false;
        }
    }
    
    return retval;
}

/**
 * Returns the string result from the last runner iteration, or an empty string if no iterations happened.
 */
std::string ConfigEditor::GetLastFrameResult() {
    return this->lastIterationResult;
}

/**
 * Causes the editor to save the config to file.
 */
void ConfigEditor::Save() {
    std::cout << "cs1" << std::endl;
    //assemble the file structure and write it into a file which the user may designate
    XMLDocument doc = XMLDocument();
    
    std::cout << "cs1a" << std::endl;

        //<camera>
        XMLTag camera = XMLTag("camera");
            XMLTagAttribute cameraIndex = XMLTagAttribute("index", std::to_string(this->runner.GetCameraIndex()));
                camera.AddAttribute(cameraIndex);
                
            std::cout << "cs1b" << std::endl;

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
                
                std::cout << "cs1c" << std::endl;

            /**
             * <camera>
             *  <settings>
             */
            XMLTag settings = this->cameraSettings.GetFinishedTag();
                std::cout << "cs1ca" << std::endl;
                camera.AddTag(settings);
            
            std::cout << "cs1cb" << std::endl;
            doc.AddTag(camera);
            
            std::cout << "cs1d" << std::endl;

        /**
         * <configuration>
         */
        XMLTag configuration = XMLTag("configuration");
            XMLTagAttribute configurationName = XMLTagAttribute("name", this->configOverview.GetConfigName());
                configuration.AddAttribute(configurationName);
                
                std::cout << "cs1e" << std::endl;

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
                
                std::cout << "cs1f" << std::endl;

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
                
                std::cout << "cs1g" << std::endl;

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
                    
                    std::cout << "cs1e" << std::endl;

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
                
                std::cout << "cs1f" << std::endl;

            /**
             * <configuration>
             *  <postprocessor>
             */
            XMLTag postprocessor = XMLTag("postprocessor");
                //<target>
                XMLTag target = XMLTag("target");
                    XMLTagAttribute targetID = XMLTagAttribute("id", "0"); //this remains constant for now until multiple target support is added.
                        target.AddAttribute(targetID);
                        
                        std::cout << "cs1g" << std::endl;

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
                    
                    std::cout << "cs1h" << std::endl;

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

                    postprocessor.AddTag(target);
                    
                    std::cout << "cs1i" << std::endl;
                
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
                        
                        std::cout << "cs1j" << std::endl;

                    postprocessor.AddTag(UDP);
                configuration.AddTag(postprocessor);
            doc.AddTag(configuration);
            
            std::cout << "cs1k" << std::endl;
    
    //to prompt file name or not to promt file name
    std::string fileToSave = this->fileName;
    std::vector<std::string> fileParts = StringUtils::SplitString(fileToSave, '/');
    
    std::cout << "cs1l" << std::endl;

    if(fileParts[fileParts.size() - 1] == "generic.xml") {
        FileChooser chooser = FileChooser(true, "config.xml");
        fileToSave = chooser.Show();
    }
    
    std::cout << "cs1m" << std::endl;

    doc.WriteFile(fileToSave);
    
    std::cout << "cs2" << std::endl;
}


void ConfigEditor::Close() {
    gtk_widget_destroy(this->configeditor);
}


void ConfigEditor::StartLearningTarget() {
    if(this->runner.GetVideoStream().isOpened()) {
        //reinstantiate the learner to apply the preprocessor settings
        this->learner = ConfigLearner(this->runner.GetPreProcessor());
        this->learnerActivated = true;
        
        ConfirmationDialog confirmLearn = ConfirmationDialog(
            std::string("Position the target in the center of the image and press OK.\n") +
            std::string("It should be highlighted with a blue box.")
        );
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


void ConfigEditor::StartLearningDistance() {
    //check for video errors
    if(this->runner.GetVideoStream().isOpened()) {
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

        //show the dialog anc ask the question, but do not destroy the dialog when the user presses OK
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


void ConfigEditor::RecheckUDP() {
    std::string newUDPAddr = this->runnerSettings.GetUDPAddr();
    int newUDPPort = this->runnerSettings.GetUDPPort();
    this->runner.ReconnectUDP(newUDPAddr, newUDPPort);
}

/**
 * Applies the settings from the camera tab to the camera itself.
 */
void ConfigEditor::ApplyCameraSettings() {
    std::vector<int> settingIDs = this->cameraSettings.GetSettingIDs();
    
    //tell the runner to apply each setting
    for(int i=0; i<settingIDs.size(); i++) {
        int id = settingIDs[i];
        double value = this->cameraSettings.GetSettingValueFromID(id);
        this->runner.SetCameraProperty(id, value);
    }
    
    //get the settings and apply them to the menu to tell the user if they were rejected or not
    for(int i=0; i<settingIDs.size(); i++) {
        int id = settingIDs[i];
        double value = this->runner.GetCameraProperty(id);
        this->cameraSettings.SetSettingValueFromID(id, value);
    }
}


void ConfigEditor::ReconnectUDPFromOverview() {
    std::string newAddr = this->configOverview.GetUDPAddr();
    int newPort = this->configOverview.GetUDPPort();
    this->runner.ReconnectUDP(newAddr, newPort);

    //set the properties in the actual editor
    this->runnerSettings.SetUDPAddr(newAddr);
    this->runnerSettings.SetUDPPort(newPort);
}


void ConfigEditor::ResetResolutionFromOverview() {
    Size newRes = this->configOverview.GetImageResolution();
    this->runner.SetResolution(newRes);

    //set the resolution in the camera settings menu
    this->cameraSettings.SetSettingValueFromID(CAP_PROP_FRAME_WIDTH, newRes.width);
    this->cameraSettings.SetSettingValueFromID(CAP_PROP_FRAME_HEIGHT, newRes.height);
}


void ConfigEditor::ResetRunnerResolution() {
    int camResX = this->cameraSettings.GetSettingValueFromID(CAP_PROP_FRAME_WIDTH);
    int camResY = this->cameraSettings.GetSettingValueFromID(CAP_PROP_FRAME_HEIGHT);
    Size newRes = Size(camResX, camResY);
    this->runner.SetResolution(newRes);
}


void ConfigEditor::SetName(std::string name) {
    gtk_widget_set_name(this->configeditor, name.c_str());
}

/**
 * Called when the X in the corner is pressed
 */
void ConfigEditor::Closed() {
    KiwiLightApp::CloseEditor(false);
}


void ConfigEditor::UpdateImage() {
    this->runner.Iterate();
    Mat displayable;

    try {
        vconcat(this->original, this->out, displayable);
        this->outputImage.Update(displayable);
    } catch(cv::Exception ex) {
        std::cout << "cv exception in ce" << std::endl;
    }
}

#include "KiwiLight.h"

/**
 * Source file for the KiwiLightApp class.
 * Written By: Brach Knutson
 */

using namespace cv;
using namespace KiwiLight;

//define some static vars for KiwiLightApp. Vars will be defined for real on call to Create();
VideoCapture KiwiLightApp::camera;
UDP          KiwiLightApp::udpSender;
Runner       KiwiLightApp::runner;
ConfigEditor KiwiLightApp::configeditor;
CronWindow   KiwiLightApp::cronWindow;
Logger       KiwiLightApp::logger;
LogViewer    KiwiLightApp::logViewer;
GThread     *KiwiLightApp::streamingThread;
AppMode      KiwiLightApp::mode = AppMode::UI_HEADLESS;
bool         KiwiLightApp::lastImageGrabSuccessful = false;
bool         KiwiLightApp::udpEnabled = true;
bool         KiwiLightApp::streamThreadEnabled = true; //acts as a kind of "enable switch" for the streamthread because it seems to like starting when its not supposed to
bool         KiwiLightApp::lfgImgInUse = false;
bool         KiwiLightApp::uiInitalized = false;
Mat          KiwiLightApp::lastFrameGrabImage;
Mat          KiwiLightApp::defaultOutImage;
Window       KiwiLightApp::win;
UDPPanel     KiwiLightApp::udpPanel;
ConfigPanel  KiwiLightApp::confInfo;
NumberBox    KiwiLightApp::cameraIndexBox;
Label        KiwiLightApp::cameraStatusLabel;
Image        KiwiLightApp::outputImage;
SubMenuItem  KiwiLightApp::runHeadlessly;
Button       KiwiLightApp::toggleRunningButton;
int          KiwiLightApp::cameraFailures = 0;
int          KiwiLightApp::currentCameraIndex = 100;

extern void RunConfigs(std::vector<std::string> filePaths); // from Main.cpp

/**
 *  TODO: REMOVE streamThreadEnabled when we determine that it is not important
 */

/**
 * Initializes GTK and builds KiwiLight
 */
void KiwiLightApp::Create(int argc, char *argv[]) {
    KiwiLightApp::mode = AppMode::UI_PAUSING;
    KiwiLightApp::cameraFailures = 0;
    KiwiLightApp:currentCameraIndex = 100; //just to define index. Camera will only open if currentCameraindex != 100
    KiwiLightApp::udpSender = UDP("127.0.0.1", 3695, false);
    KiwiLightApp::defaultOutImage = imread("noimg.png");
    
    gtk_init(&argc, &argv);
    win = Window(GTK_WINDOW_TOPLEVEL);
        Panel content = Panel(false, 0);
            MenuBar menubar = KiwiLightApp::CreateMenuBar();
                content.Pack_start(menubar.GetWidget(), false, false, 0);

            Panel ribbon = Panel(true, 0);
                Image logo = Image("banner_small.png");
                    ribbon.Pack_start(logo.GetWidget(), false, false, 0);

                Separator sep = Separator(false);
                    ribbon.Pack_start(sep.GetWidget(), false, false, 0);

                Panel cameraSelectionPanel = Panel(false, 0);
                    Panel numberBoxPanel = Panel(true, 0);
                        Label numberBoxLabel = Label("Camera: ");
                            numberBoxPanel.Pack_start(numberBoxLabel.GetWidget(), false, false, 0);

                        KiwiLightApp::cameraIndexBox = NumberBox(0, 10, 1, 0);
                            numberBoxPanel.Pack_start(KiwiLightApp::cameraIndexBox.GetWidget(), false, false, 0);

                        cameraSelectionPanel.Pack_start(numberBoxPanel.GetWidget(), false, false, 0);
                    
                    Button openCameraButton = Button("Open", KiwiLightApp::OpenNewCameraFromMainIndex);
                        cameraSelectionPanel.Pack_start(openCameraButton.GetWidget(), false, false, 0);

                    ribbon.Pack_start(cameraSelectionPanel.GetWidget(), false, false, 0);

                KiwiLightApp::cameraStatusLabel = Label("");
                    ribbon.Pack_start(cameraStatusLabel.GetWidget(), false, false, 0);

                Separator sep2 = Separator(false);
                    ribbon.Pack_start(sep2.GetWidget(), false, false, 5);

                KiwiLightApp::udpPanel = UDPPanel(udpEnabled);
                    ribbon.Pack_start(udpPanel.GetWidget(), false, false, 0);

                Separator sep3 = Separator(false);
                    ribbon.Pack_start(sep3.GetWidget(), false, false, 5);

            content.Pack_start(ribbon.GetWidget(), false, false, 0);

            Panel body = Panel(true, 0);
                Panel configInfoPanel = Panel(false, 0);
                    KiwiLightApp::confInfo = ConfigPanel(XMLDocument(""));
                        configInfoPanel.Pack_start(KiwiLightApp::confInfo.GetWidget(), true, true, 0);

                    Panel configButtonPanel = Panel(true, 0);
                        Button editButton = Button("Edit", KiwiLightApp::EditConfiguration);
                            configButtonPanel.Pack_start(editButton.GetWidget(), true, true, 0);

                        KiwiLightApp::toggleRunningButton = Button("Run", KiwiLightApp::RunHeadlesslyCallback);
                            configButtonPanel.Pack_start(toggleRunningButton.GetWidget() ,true, true, 0);

                        configInfoPanel.Pack_start(configButtonPanel.GetWidget(), true, false, 0);
                    body.Pack_start(configInfoPanel.GetWidget(), true, true, 0);
                
                KiwiLightApp::outputImage = Image(ImageColorspace::RGB);
                    body.Pack_start(KiwiLightApp::outputImage.GetWidget(), true, true, 0);

                content.Pack_start(body.GetWidget(), false, false, 0);

    win.SetPane(content);
    win.SetCSS("ui/Style.css");
    win.SetOnAppClosed(KiwiLightApp::Quit);
    win.SetInterval(75, KiwiLightApp::UpdateApp);
    uiInitalized = true;
    
    OpenNewCameraOnIndex(0);
}

/**
 * Starts KiwiLight.
 */
void KiwiLightApp::Start() {
    win.Show();
    KiwiLightApp::LaunchStreamingThread(AppMode::UI_STREAM);
    win.Main();
}


void KiwiLightApp::WaitForSocket() {
    while(!udpSender.Connected()) {
        udpSender.AttemptToConnect();
    }
}

/**
 * Configures and starts the KiwiLight log.
 */
void KiwiLightApp::ConfigureHeadless(std::string runnerNames, std::string runnerFiles) {
    //find HOME so we can put the file in /home/<usr>/KiwiLightData/logs/KiwiLight-Runner-Log-DD-MM-YY-HH-MM-SS.xml
    std::string logFileBase = "";
    char *home = getenv("HOME");
    if(home != NULL) {
        logFileBase = std::string(home) + "/KiwiLightData/logs/";
    } else {
        std::cout << "WARNING: The HOME Environment variable could not be found! The Log file will not be generated!" << std::endl;
    }
    std::string logFileName = logFileBase + "KiwiLight-Runner-Log-" + Clock::GetDateString() + ".xml";
    KiwiLightApp::logger = Logger(logFileName);
    KiwiLightApp::logger.SetConfName(runnerNames, runnerFiles);
    KiwiLightApp::logger.Start();
    KiwiLightApp::mode = AppMode::UI_HEADLESS;
}

/**
 * Reports a runner iteration result to KiwiLight to log.
 * If ConfigureHeadless() has not yet been called, log data will be reported to the junk log.
 * Junk log path: /home/<usr>/KiwiLightData/logs/junk.xml
 */
void KiwiLightApp::ReportHeadless(std::string runnerOutput) {
    KiwiLightApp::logger.Log(runnerOutput);
}

/**
 * Returns KiwiLight's runner.
 */
Runner KiwiLightApp::GetRunner() { return runner; }

/**
 * Returns KiwiLight's config editor.
 */
ConfigEditor KiwiLightApp::GetEditor() { return configeditor; }

/**
 * Returns true if the UI is initalized, false otherwise.
 */
bool KiwiLightApp::UIInitalized() { return uiInitalized; }

/**
 * Takes an image with the KiwiLight camera and returns it
 */
Mat KiwiLightApp::TakeImage() {
    Mat img;
    if(KiwiLightApp::camera.isOpened()) {
        bool success = KiwiLightApp::camera.grab();
        
        if(success) {
            success = KiwiLightApp::camera.retrieve(img);
        }
        KiwiLightApp::lastImageGrabSuccessful = success;
        
        //additionally...
        if(img.empty()) {
            lastImageGrabSuccessful = false;
        }
    }

    return img;
}

/**
 * Returns the value of KiwiLight's camera's property on the given ID (ID from opencv constants).
 */
double KiwiLightApp::GetCameraProperty(int propId) {
    return KiwiLightApp::camera.get(propId);
}

/**
 * Returns true if KiwiLight's camera is open, false otherwise
 */
bool KiwiLightApp::CameraOpen() {
    return KiwiLightApp::camera.isOpened();
}

/**
 * Sets a property value on KiwiLight's camera to value.
 */
void KiwiLightApp::SetCameraProperty(int propId, double value) {
    KiwiLightApp::camera.set(propId, value);
}

/**
 * Returns true if last image take was successful, false otherwise.
 */
bool KiwiLightApp::LastImageCaptureSuccessful() {
    return KiwiLightApp::lastImageGrabSuccessful;
}

/**
 * Returns the current KiwiLight App mode.
 */
AppMode KiwiLightApp::CurrentMode() {
    return KiwiLightApp::mode;
}

/**
 * Returns KiwiLight's UDP sender.
 */
UDP KiwiLightApp::GetUDP() {
    return KiwiLightApp::udpSender;
}

/**
 * Returns true if the UDP sender is enabled, false otherwise.
 */
bool KiwiLightApp::GetUDPEnabled() {
    return KiwiLightApp::udpEnabled;
}

/**
 * Returns the path of the file that KiwiLight is currently running/editing, or "" if no file is loaded.
 */
std::string KiwiLightApp::GetCurrentFile() {
    return KiwiLightApp::confInfo.GetConfigFile();
}

/**
 * Returns KiwiLight's MenuBar widget. This goes at the top of the GUI.
 */
MenuBar KiwiLightApp::CreateMenuBar() {
    MenuBar menubar = MenuBar();
        menubar.SetName("hover_gray");
        MenuItem file = MenuItem("File");
            SubMenuItem addConfig = SubMenuItem("New Configuration", KiwiLightApp::NewConfiguration);
                file.AddSubmenuItem(addConfig);

            SubMenuItem openConfig = SubMenuItem("Open Configuration", KiwiLightApp::OpenConfiguration);
                file.AddSubmenuItem(openConfig);

            SubMenuItem editConfig = SubMenuItem("Edit Configuration", KiwiLightApp::EditConfiguration);
                file.AddSubmenuItem(editConfig);

            SubMenuItem stopConfig = SubMenuItem("Close Configuration", KiwiLightApp::CloseConfiguration);
                file.AddSubmenuItem(stopConfig);

            SubMenuItem quit = SubMenuItem("Quit", KiwiLightApp::Quit);
                file.AddSubmenuItem(quit);

            menubar.AddItem(file);

        MenuItem config = MenuItem("Configuration");
            SubMenuItem confCron = SubMenuItem("Configure Auto-Start", KiwiLightApp::ShowCronMenu);
                config.AddSubmenuItem(confCron);

            //NOTE: runHeadlessly is a member of KiwiLight because the text needs to be changed in runtime. As a result, it is addressed differently.
            KiwiLightApp::runHeadlessly = SubMenuItem("Run Headlessly", KiwiLightApp::RunHeadlesslyCallback);
                config.AddSubmenuItem(runHeadlessly);

            SubMenuItem viewLog = SubMenuItem("View Log", KiwiLightApp::ShowLog);
                config.AddSubmenuItem(viewLog);

            menubar.AddItem(config);

        MenuItem help = MenuItem("Help");
            SubMenuItem about = SubMenuItem("About", KiwiLightApp::ShowAboutWindow);
                help.AddSubmenuItem(about);

            SubMenuItem helpme = SubMenuItem("How to Use", KiwiLightApp::ShowHelpWindow);
                help.AddSubmenuItem(helpme);

            menubar.AddItem(help);

    return menubar;
}

/**
 * Starts the thread that constantly updates KiwiLight's streams.
 */
void KiwiLightApp::LaunchStreamingThread(AppMode newMode) {
    if(newMode != AppMode::UI_PAUSING) {
        // streamThreadEnabled = true;
        KiwiLightApp::mode = newMode;
        KiwiLightApp::streamingThread = g_thread_new("stream monitor", GThreadFunc(KiwiLightApp::UpdateStreamsConstantly), NULL);
    }
}

/**
 * Messages the streaming thread to stop and waits until it does
 */
void KiwiLightApp::StopStreamingThread() {
        KiwiLightApp::mode = AppMode::UI_PAUSING;
        if(KiwiLightApp::streamingThread != NULL) {
            g_thread_join(KiwiLightApp::streamingThread);
        }
}

/**
 * Closes the config editor (if it is open)
 * @param saveFirst when true, causes the editor to save the open configuration to file.
 * @return Whether or not the config editor was closed (true if it was, false otherwise).
 * The config editor will not close if the user presses "cancel" on the save dialog.
 */
bool KiwiLightApp::CloseEditor(bool saveFirst) {
    if(saveFirst) {
        bool fileSaved = KiwiLightApp::configeditor.Save();
        if(!fileSaved) {
            //user did not choose a file to save, and most likely hit "cancel." Abort the operation
            std::cout << "No file specified when saving editor. Not closing." << std::endl;
            return false;
        }
    }
    
    KiwiLightApp::StopStreamingThread();
    KiwiLightApp::configeditor.Close();
    OpenConfigurationFromFile(KiwiLightApp::configeditor.GetFileName());
    // KiwiLightApp::LaunchStreamingThread(fileSaved ? AppMode::UI_RUNNER : AppMode::UI_EDITOR);
    KiwiLightApp::LaunchStreamingThread(AppMode::UI_RUNNER);
    return true;
}

/**
 * Causes the opened editor to begin the target learn process.
 */
void KiwiLightApp::StartEditorLearningTarget() {
    KiwiLightApp::configeditor.StartLearningTarget();
}

/**
 * Causes the editor to start learning distance constants.
 */
void KiwiLightApp::StartEditorLearningDistance() {
    KiwiLightApp::configeditor.StartLearningDistance();
}

/**
 * Causes the editor to recheck and reapply the user's UDP address and port
 */
void KiwiLightApp::EditorReconnectUDP() {
    KiwiLightApp::configeditor.ReconnectUDPFromEditor();
}

/**
 * Causes the editor to connect the UDP from the overview panel.
 */
void KiwiLightApp::EditorConnectUDPFromOverview() {
    KiwiLightApp::configeditor.ReconnectUDPFromOverview();
}

/**
 * Causes KiwiLight to open a new camera on the given index.
 */
void KiwiLightApp::OpenNewCameraOnIndex(int index) {
    if(KiwiLightApp::currentCameraIndex != index || !KiwiLightApp::camera.isOpened()) {
        KiwiLightApp::currentCameraIndex = index;
        AppMode currentMode = KiwiLightApp::mode;
        KiwiLightApp::camera = VideoCapture(index);
        
        //set the auto exposure menu in shell because opencv cant do it
        //if this is not set then exposure cannot be set
        
        Shell::ExecuteCommand(
            std::string("v4l2-ctl -d ") + 
            std::to_string(index) + 
            std::string(" --set-ctrl=exposure_auto=1")
        );

        if(uiInitalized) {
            //set the text box on main UI
            KiwiLightApp::cameraIndexBox.SetValue((double) index);
        
            //set editor text boxes if necessary
            if(currentMode == AppMode::UI_EDITOR) {
                KiwiLightApp::configeditor.SetCameraIndexBoxes(index);
            }
        }
    }
}

/**
 * Sets the IPv4 address and port of KiwiLight's socket sender.
 */
void KiwiLightApp::ReconnectUDP(std::string newAddress, int newPort) {
    ReconnectUDP(newAddress, newPort, false);
}

/**
 * Sets the IPv4 address and port of KiwiLight's socket sender. Optionally, you can also set it to wait until it is connected.
 */
void KiwiLightApp::ReconnectUDP(std::string newAddress, int newPort, bool block) {
    int oldSendRate = KiwiLightApp::udpSender.MaxSendRate();
    KiwiLightApp::udpSender = UDP(newAddress, newPort, block, oldSendRate);
    if(KiwiLightApp::uiInitalized) {
        udpPanel.SetAddress(udpSender.GetAddress());
        udpPanel.SetPort(udpSender.GetPort());
        udpPanel.SetConnected(udpSender.Connected());
    }
}

void KiwiLightApp::SetUDPMaxSendRate(int maxSendRate) {
    KiwiLightApp::udpSender.SetMaxSendRate(maxSendRate);
}

/**
 * Sends "message" over KiwiLight's socket sender.
 */
void KiwiLightApp::SendOverUDP(std::string message) {
    KiwiLightApp::udpSender.Send(message);
}

/**
 * Callback for when a button is pressed on the cron window table
 */
void KiwiLightApp::CronDeleteButtonPressed(void* data, GtkWidget *widget) {
    int lineToDelete = GPOINTER_TO_INT(data);
    std::string rule = KiwiLightApp::cronWindow.GetRuleByLine(lineToDelete);
    std::string file = KiwiLightApp::cronWindow.GetFileNameFromRule(rule);
    ConfirmationDialog confirmation = ConfirmationDialog("Really Remove " + file + "?");
    bool shouldDelete = confirmation.ShowAndGetResponse();
    if(shouldDelete) {
        KiwiLightApp::cronWindow.SaveRule(file, false);
    }
}

void KiwiLightApp::CronAddButtonPressed() {
    FileChooser filechooser = FileChooser(false, "");
    std::string fileToRun = filechooser.Show();
    KiwiLightApp::cronWindow.SaveRule(fileToRun, true);
}

/**
 * Causes the editor to apply all camera settings to the video stream.
 */
void KiwiLightApp::EditorApplyCameraSettings() {
    KiwiLightApp::StopStreamingThread();
    KiwiLightApp::configeditor.ApplyCameraSettings();
    KiwiLightApp::LaunchStreamingThread(AppMode::UI_EDITOR);
}

/**
 * Causes the editor to connect tot a different camera using index from the overview panel.
 */
void KiwiLightApp::EditorOpenNewCameraFromOverview() {
    //we don't need to do stream things here because it is all handled in OpenNewCameraOnIndex()
    KiwiLightApp::configeditor.OpenNewCameraFromOverview();
}

/**
 * Applies FRC settings on the camera.
 */
void KiwiLightApp::EditorApplyFRCSettings() {
    KiwiLightApp::configeditor.ApplyFRCCameraSettings();
}

/**
 * Changes the editor target color value.
 */
void KiwiLightApp::EditorSetTargetColorHSV(int h, int s, int v) {
    KiwiLightApp::configeditor.SetTargetColor(h, s, v);
}

/**
 * Shows the log plot on the LogViewer
 */
void KiwiLightApp::ToggleLogPlot() {
    KiwiLightApp::logViewer.TogglePlotShowing();
}


void KiwiLightApp::GenerateLogPlot() {
    KiwiLightApp::logViewer.GenerateAndShowPlot();
}

/**
 * Updates KiwiLight's utilities depending on it's state.
 */
void KiwiLightApp::UpdateApp() {
    try {
        if(KiwiLightApp::mode == AppMode::UI_EDITOR) {
            KiwiLightApp::configeditor.Update();
        }

        if(KiwiLightApp::mode != AppMode::UI_HEADLESS) {
            //update the camera error label based on how successful thread is being
            if(lastImageGrabSuccessful) {
                //wait for the output image to be updated by other thread
                while(KiwiLightApp::lfgImgInUse) {
                    usleep(100);
                }
                KiwiLightApp::lfgImgInUse = true;
                KiwiLightApp::outputImage.Update(KiwiLightApp::lastFrameGrabImage);
                KiwiLightApp::lfgImgInUse = false;
                KiwiLightApp::cameraStatusLabel.SetText(""); 
            } else {
                KiwiLightApp::cameraStatusLabel.SetText("Camera Error!");
                KiwiLightApp::cameraFailures++;
                KiwiLightApp::outputImage.Update(KiwiLightApp::defaultOutImage);

                if(KiwiLightApp::cameraFailures > 50) {
                    //attempt to reconnect the camera stream
                    OpenNewCameraFromMainIndex();
                    KiwiLightApp::cameraFailures = 0;
                }
            }
        } else {
            KiwiLightApp::cameraStatusLabel.SetText("Running Headlessly");
        }
    } catch(cv::Exception ex) {
        std::cout << "An OpenCv Exception was encountered while running the Main thread!" << std::endl;
        std::cout << "ex.what(): " << ex.what() << std::endl;
        KiwiLightApp::lastImageGrabSuccessful = false;
    } catch(std::invalid_argument ex) {
        std::cout << "A std::invalid_argument Exception was encountered while running the Main thread!" << std::endl;
        std::cout << "ex.what(): " << ex.what() << std::endl;
    }
}

/**
 * Updates the streams nonstop until mode is set to UI_QUITTING
 */
void KiwiLightApp::UpdateStreamsConstantly() {
    //now because some cameras like the jevois take a little longer for the stream to start, we will wait until it gives us a good frame
    //to avoid the VIDIOC_QBUF: Invalid Argument barage.
    bool retrieveSuccess = false;
    while(!retrieveSuccess && KiwiLightApp::mode != AppMode::UI_PAUSING) {
        usleep(250000); //give camera some time to adjust and do things
        bool grabSuccess = KiwiLightApp::camera.grab();
        if(grabSuccess) {
            Mat img;
            retrieveSuccess = KiwiLightApp::camera.retrieve(img);
        }
    }

    while(KiwiLightApp::mode != AppMode::UI_PAUSING) {
        KiwiLightApp::UpdateStreams();
    }
}

/**
 * Updates KiwiLight's video streams.
 */
void KiwiLightApp::UpdateStreams() {
    try {
        //attempt to loop the things
        Mat displayImage;
        switch(KiwiLightApp::mode) {
            case AppMode::UI_STREAM: {
                    displayImage = KiwiLightApp::TakeImage();
                }
                break;
            case AppMode::UI_RUNNER: {
                    std::string output = KiwiLightApp::runner.Iterate();
                    KiwiLightApp::runner.GetLastFrameSuccessful(); //boolean
                    displayImage = KiwiLightApp::runner.GetOutputImage();
                    
                    //if the udp is enabled, send the message
                    if(KiwiLightApp::udpEnabled) {
                        KiwiLightApp::udpSender.Send(output);
                    }
                }
                break;
            case AppMode::UI_EDITOR: {
                    KiwiLightApp::configeditor.UpdateImageOnly();
                    displayImage = KiwiLightApp::configeditor.GetOutputImage();
                    
                    std::string output = KiwiLightApp::configeditor.GetLastFrameResult(); //gets the results of the last runner iteration
                    
                    //send if udp enabled
                    if(KiwiLightApp::udpEnabled) {
                        KiwiLightApp::udpSender.Send(output);
                    }
                }
                break;
            default:
                break;
        }
        
        // if successful, update the display image
        if(KiwiLightApp::lastImageGrabSuccessful) {            
            //wait for out image to be used by other thread
            while(KiwiLightApp::lfgImgInUse) {
                usleep(1000);
            }
            KiwiLightApp::lfgImgInUse = true;
            KiwiLightApp::lastFrameGrabImage = displayImage;
            KiwiLightApp::lfgImgInUse = false;
        }
    } catch(cv::Exception ex) {
        std::cout << "An OpenCv Exception was encountered while running the Streaming thread!" << std::endl;
        std::cout << "ex.what(): " << ex.what() << std::endl;
        KiwiLightApp::lastImageGrabSuccessful = false;
    } catch(std::invalid_argument ex) {
        std::cout << "A std::invalid_argument Exception was encountered while running the Streaming thread!" << std::endl;
        std::cout << "ex.what(): " << ex.what() << std::endl;
    }
}

/**
 * Opens a new videostream on the index that the user selects.
 */
void KiwiLightApp::OpenNewCameraFromMainIndex() {
    OpenNewCameraOnIndex((int) KiwiLightApp::cameraIndexBox.GetValue());
}

/**
 * Enables or disables KiwiLight's socket sender.
 */
void KiwiLightApp::ToggleUDP() {
    KiwiLightApp::udpEnabled = !KiwiLightApp::udpEnabled;
    
    //set the button text
    KiwiLightApp::udpPanel.SetEnabled (udpEnabled);

    //set the buttons in the config editor if necessary
    if(KiwiLightApp::mode == AppMode::UI_EDITOR) {
        KiwiLightApp::configeditor.SetUDPEnabledLabels(KiwiLightApp::udpEnabled);
    }
}

/**
 * Causes KiwiLight to create and open a new configuration.
 */
void KiwiLightApp::NewConfiguration() {
    if(!PromptEditorSaveAndClose()) { return; }
    if(!PromptHeadlessStop()) { return; }
    
    //set mode to stream and open a new editor.
    KiwiLightApp::mode = AppMode::UI_STREAM;
    EditConfiguration();
}

/**
 * Causes KiwiLight to open the editor for the selected configuration.
 * @param currentMode The UIMode active when callback was triggered
 */
void KiwiLightApp::EditConfiguration() {
    if(!PromptEditorSaveAndClose()) { return; }
    if(!PromptHeadlessStop()) { return; }

    AppMode currentMode = KiwiLightApp::mode;
    if(currentMode != AppMode::UI_EDITOR) {
        //since this is a callback, close streamer
        std::string pathToOpen = "";
        StopStreamingThread();

        if(currentMode == AppMode::UI_STREAM) {
            pathToOpen = Util::ResolveGenericConfFilePath();            
        } else if(currentMode == AppMode::UI_RUNNER) {
            pathToOpen = KiwiLightApp::runner.GetFileName();
        } else if(currentMode == AppMode::UI_PAUSING) {
            std::cout << "WARNING: UI mode unclear. Make sure EditConfiguration() is called before stream thread is terminated" << std::endl;
        }
        KiwiLightApp::confInfo.LoadConfig(pathToOpen);
        KiwiLightApp::configeditor = ConfigEditor(pathToOpen);
        LaunchStreamingThread(AppMode::UI_EDITOR);
    }
}

/**
 * Causes KiwiLight to open a new configuration after prompting the user with a file dialog.
 */
void KiwiLightApp::OpenConfiguration() {
    if(!PromptEditorSaveAndClose()) { return; }
    if(!PromptHeadlessStop()) { return; }

    FileChooser chooser = FileChooser(false, "");
    std::string fileToOpen = chooser.Show();
    
    if(fileToOpen != "") {
        StopStreamingThread();
        bool success = OpenConfigurationFromFile(fileToOpen);
        if(success) {
            LaunchStreamingThread(AppMode::UI_RUNNER);
        } else {
            //alert the user that the file they selected was bad
            ConfirmationDialog alert = ConfirmationDialog("The File you selected could not be opened.");
            alert.ShowAndGetResponse();
            LaunchStreamingThread(AppMode::UI_STREAM);
        }
    }
}

/**
 * Causes KiwiLight to open the configuration specified by the file
 * @return {@code true} if the operation succeeds, {@code false} otherwise.
 */
bool KiwiLightApp::OpenConfigurationFromFile(std::string fileName) {
    XMLDocument newDoc = XMLDocument(fileName);
    if(Util::configDocumentIsValid(newDoc)) {
        KiwiLightApp::confInfo.LoadConfig(newDoc);
        KiwiLightApp::runner = Runner(fileName, true);
        return true;
    } else {
        return false;
    }
}

/**
 * Causes KiwiLight to close the currently opened configuration.
 */
void KiwiLightApp::CloseConfiguration() {
    if(!PromptEditorSaveAndClose()) { return; }
    if(!PromptHeadlessStop()) { return; }

    KiwiLightApp::confInfo.Clear();
    KiwiLightApp::mode = AppMode::UI_STREAM;
}

/**
 * Prompts the user to close the editor.
 * @return true if the editor was either closed, or not opened in the first place, false otherwise.
 */
bool KiwiLightApp::PromptEditorSaveAndClose() {
    if(KiwiLightApp::mode == AppMode::UI_EDITOR) {
        ConfirmationDialog closeConfirmation = ConfirmationDialog("A Configuration is being edited. Would you like to close it?");
        bool shouldClose = closeConfirmation.ShowAndGetResponse();

        if(shouldClose) {
            ConfirmationDialog saveConfirmation = ConfirmationDialog("Save Configuration before closing?");
            bool shouldSave = saveConfirmation.ShowAndGetResponse();
            CloseEditor(shouldSave);
            return true;
        }
    } else {
        return true;
    }

    return false;
}

/**
 * Prompts the user to stop the headless configuration.
 * @return true if the configuration was stopped, or not started in the first place, false otherwise.
 */
bool KiwiLightApp::PromptHeadlessStop() {
    //If the configuration is headless, prompt to close it to prevent mix ups.
    if(KiwiLightApp::mode == AppMode::UI_HEADLESS) {
        ConfirmationDialog closeConfirmation = ConfirmationDialog("The current configuration is running headlessly. Would you like to stop it?");
        bool shouldClose = closeConfirmation.ShowAndGetResponse();
        if(shouldClose) {
            StopRunningHeadlessly();
        } else {
            return false;
        }
    }
    
    return true;
}

/**
 * Causes KiwiLight to exit.
 */
void KiwiLightApp::Quit() {
    StopStreamingThread();
    gtk_main_quit();
}

/**
 * Shows a pop-up dialog asking whether or not the currently loaded file should be started on boot.
 */
void KiwiLightApp::ShowCronMenu() {
    KiwiLightApp::cronWindow = CronWindow(GTK_WINDOW_TOPLEVEL);
    cronWindow.Show();
}

/**
 * Creates a separate thread and calls RunConfigs() from Main.cpp with the currently loaded file.
 */
void KiwiLightApp::RunHeadlessly() {
    std::string confFile = KiwiLightApp::GetCurrentFile();
    if(confFile == "") {
        std::cout << "WARNING: No conf file detected! Aborting headless run." << std::endl;
        return;
    }

    //assemble the config vector
    std::vector<std::string> configVector;
    configVector.push_back(confFile);
    KiwiLightApp::mode = AppMode::UI_HEADLESS;
    RunConfigs(configVector);
}


void KiwiLightApp::StopRunningHeadlessly() {
    //Terminate the headless task running on the streaming thread by setting mode to pausing, then start streaming thread as normal.
    KiwiLightApp::mode = AppMode::UI_PAUSING;
    g_thread_join(KiwiLightApp::streamingThread);
    runHeadlessly.SetText("Run Headlessly");
    toggleRunningButton.SetText("Run");
    LaunchStreamingThread(AppMode::UI_RUNNER);
}

/**
 * Callback for the "Run Headlessly" buttons.
 */
void KiwiLightApp::RunHeadlesslyCallback() {
    if(mode == AppMode::UI_HEADLESS) {
        StopRunningHeadlessly();
    } else {
        //before stopping thread, check to see that we have a valid configuration loaded. If not, load one.
        if(GetCurrentFile() == "") {
            //no file loaded, prompt user to load one
            OpenConfiguration();
            
            if(GetCurrentFile() == "") {
                // the user has pressed cancel or did not select a file when opening a configuration.
                return;
            }
        }

        if(!PromptEditorSaveAndClose()) { return; }
        StopStreamingThread();

        streamingThread = g_thread_new("headless runner", GThreadFunc(KiwiLightApp::RunHeadlessly), NULL);
        runHeadlessly.SetText("Stop Running");
        toggleRunningButton.SetText("Stop");
        outputImage.Update(defaultOutImage);
    }
}

/**
 * Creates a window which displays information from a specified KiwiLight log.
 */
void KiwiLightApp::ShowLog(XMLDocument log) {
    KiwiLightApp::logViewer.Release();
    KiwiLightApp::logViewer = LogViewer(log);
    logViewer.Show();
}

/**
 * Creates a file dialog for the user to select a log file to show, then shows it.
 */
void KiwiLightApp::ShowLog() {
    FileChooser chooser = FileChooser(false, "");
    std::string fileToOpen = chooser.Show();
    if(fileToOpen != "") {
        XMLDocument log = XMLDocument(fileToOpen);
        if(Util::logDocumentIsValid(log)) {
            ShowLog(fileToOpen);
        } else {
            ConfirmationDialog alert = ConfirmationDialog("The Selected File could not be opened.");
            alert.ShowAndGetResponse();
        }
    }
}

/**
 * Causes KiwiLight to show the about window.
 */
void KiwiLightApp::ShowAboutWindow() {
    AboutWindow win = AboutWindow(GTK_WINDOW_TOPLEVEL);
    win.Show();
}

/**
 * Causes KiwiLight to show the help window.
 */
void KiwiLightApp::ShowHelpWindow() {
    HelpWindow win = HelpWindow(GTK_WINDOW_TOPLEVEL);
    win.Show();
}

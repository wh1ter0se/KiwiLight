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
GThread     *KiwiLightApp::streamingThread;
UIMode       KiwiLightApp::mode = UIMode::UI_STREAM;
bool         KiwiLightApp::lastImageGrabSuccessful = false;
bool         KiwiLightApp::udpEnabled = false;
bool         KiwiLightApp::streamThreadEnabled = true; //acts as a kind of "enable switch" for the streamthread because it seems to like starting when its not supposed to
bool         KiwiLightApp::outImgInUse = false;
Mat          KiwiLightApp::lastFrameGrabImage;
Window       KiwiLightApp::win;
ConfigPanel  KiwiLightApp::confInfo;
NumberBox    KiwiLightApp::cameraIndexBox;
Label        KiwiLightApp::cameraStatusLabel;
Image        KiwiLightApp::outputImage;
Button       KiwiLightApp::toggleUDPButton;
int          KiwiLightApp::cameraFailures;
int          KiwiLightApp::currentCameraIndex = 100;

/**
 * Initializes GTK and builds KiwiLight
 */
void KiwiLightApp::Create(int argc, char *argv[]) {
    KiwiLightApp::mode = UIMode::UI_PAUSING;
    KiwiLightApp::cameraFailures = 0;
    KiwiLightApp:currentCameraIndex = 100; //just to define index. Camera will only open if currentCameraindex != 100

    KiwiLightApp::udpSender = UDP("127.0.0.1", 3695);
    
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

            content.Pack_start(ribbon.GetWidget(), false, false, 0);

            Panel body = Panel(true, 0);
                Panel configInfoPanel = Panel(false, 0);
                    KiwiLightApp::confInfo = ConfigPanel(XMLDocument(""));
                        configInfoPanel.Pack_start(KiwiLightApp::confInfo.GetWidget(), true, true, 0);

                    Panel configButtonPanel = Panel(true, 0);
                        Button editButton = Button("Edit", KiwiLightApp::EditConfiguration);
                            configButtonPanel.Pack_start(editButton.GetWidget(), true, true, 0);

                        KiwiLightApp::toggleUDPButton = Button("Enable UDP", ToggleUDP);
                            configButtonPanel.Pack_start(KiwiLightApp::toggleUDPButton.GetWidget(), true, true, 0);

                        configInfoPanel.Pack_start(configButtonPanel.GetWidget(), true, false, 0);
                    body.Pack_start(configInfoPanel.GetWidget(), true, true, 0);
                
                KiwiLightApp::outputImage = Image(ImageColorspace::RGB);
                    body.Pack_start(KiwiLightApp::outputImage.GetWidget(), true, true, 0);

                content.Pack_start(body.GetWidget(), false, false, 0);

    win.SetPane(content);
    win.SetCSS("ui/Style.css");
    win.SetOnAppClosed(KiwiLightApp::Quit);
    win.SetInterval(75, KiwiLightApp::UpdateApp);
    
    OpenNewCameraOnIndex(0);
}

/**
 * Starts KiwiLight.
 */
void KiwiLightApp::Start() {
    win.Show();
    KiwiLightApp::LaunchStreamingThread(UIMode::UI_STREAM);
    win.Main();
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

UDP KiwiLightApp::GetUDP() {
    return KiwiLightApp::udpSender;
}

/**
 * Returns KiwiLight's menu bar
 */
MenuBar KiwiLightApp::CreateMenuBar() {
    MenuBar menubar = MenuBar();
        menubar.SetName("hover_gray");
        MenuItem file = MenuItem("File");
            SubMenuItem addConfig = SubMenuItem("New Configuration", KiwiLightApp::NewConfiguration);
                file.AddSubmenuItem(addConfig);

            SubMenuItem openConfig = SubMenuItem("Open Configuration", KiwiLightApp::OpenConfiguration);
                file.AddSubmenuItem(openConfig);

            SubMenuItem stopConfig = SubMenuItem("Close Config", KiwiLightApp::CloseConfiguration);
                file.AddSubmenuItem(stopConfig);


            SubMenuItem quit = SubMenuItem("Quit", KiwiLightApp::Quit);
                file.AddSubmenuItem(quit);

            menubar.AddItem(file);

        MenuItem help = MenuItem("Help");
            SubMenuItem about = SubMenuItem("About", KiwiLightApp::ShowAboutWindow);
                help.AddSubmenuItem(about);

            SubMenuItem helpme = SubMenuItem("How the heck does this work?", KiwiLightApp::ShowHelpWindow);
                help.AddSubmenuItem(helpme);

            menubar.AddItem(help);

    return menubar;
}

/**
 * Starts the thread that constantly updates KiwiLight's streams.
 */
void KiwiLightApp::LaunchStreamingThread(UIMode newMode) {
    if(newMode != UIMode::UI_PAUSING) {
        streamThreadEnabled = true;
        KiwiLightApp::mode = newMode;
        std::cout << "Starting Stream Thread..." << std::endl;
        KiwiLightApp::streamingThread = g_thread_new("stream monitor", GThreadFunc(KiwiLightApp::UpdateStreamsConstantly), NULL);
    }
}

/**
 * Messages the streaming thread to stop and waits until it does
 */
void KiwiLightApp::StopStreamingThread() {
    if(streamThreadEnabled) {
        streamThreadEnabled = false;
        KiwiLightApp::mode = UIMode::UI_PAUSING;
        g_thread_join(KiwiLightApp::streamingThread);
    }
}

/**
 * Closes the config editor (if it is open)
 * @param saveFirst when true, causes the editor to save the open configuration to file.
 */
void KiwiLightApp::CloseEditor(bool saveFirst) {
    if(saveFirst) {
        KiwiLightApp::configeditor.Save();
    }
    
    KiwiLightApp::configeditor.Close();
    OpenConfigurationFromFile(KiwiLightApp::configeditor.GetFileName());
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
        UIMode currentMode = KiwiLightApp::mode;
    
        if(KiwiLightApp::camera.isOpened()) {
            KiwiLightApp::camera.~VideoCapture();
        }
        
        KiwiLightApp::camera = VideoCapture(index);
        
        //set the auto exposure menu in shell because opencv cant do it
        //if this is not set then exposure cannot be set
        
        std::cout << "Configuring Auto Exposure setting on new camera" << std::endl;
        Shell::ExecuteCommand(
            std::string("v4l2-ctl -d ") + 
            std::to_string(index) + 
            std::string(" --set-ctrl=exposure_auto=1")
        );
    
        //set the text box on main UI
        KiwiLightApp::cameraIndexBox.SetValue((double) index);
    
        //set editor text boxes if necessary
        if(currentMode == UIMode::UI_EDITOR) {
            KiwiLightApp::configeditor.SetCameraIndexBoxes(index);
        }
    }
}

/**
 * This method is ONLY to be used if KiwiLight is working headless (no UI)!!!!!!!
 * Opens new camera without updating UI elements
 */
void KiwiLightApp::InitCameraOnly(int index) {
    std::cout << "Configuring Auto Exposure setting on new camera" << std::endl;
    Shell::ExecuteCommand(
        std::string("v4l2-ctl -d ") + 
        std::to_string(index) + 
        std::string(" --set-ctrl=exposure_auto=1")
    );

    KiwiLightApp::camera = VideoCapture(index);
}


void KiwiLightApp::ReconnectUDP(std::string newAddress, int newPort) {
    KiwiLightApp::udpSender = UDP(newAddress, newPort);
}


void KiwiLightApp::ReconnectUDP(std::string newAddress, int newPort, bool block) {
    KiwiLightApp::udpSender = UDP(newAddress, newPort, block);
}


void KiwiLightApp::SendOverUDP(std::string message) {
    KiwiLightApp::udpSender.Send(message);
}

/**
 * Causes the editor to apply all camera settings to the video stream.
 */
void KiwiLightApp::EditorApplyCameraSettings() {
    KiwiLightApp::StopStreamingThread();
    KiwiLightApp::configeditor.ApplyCameraSettings();
    KiwiLightApp::LaunchStreamingThread(UIMode::UI_EDITOR);
}

/**
 * Causes the editor to connect tot a different camera using index from the overview panel.
 */
void KiwiLightApp::EditorOpenNewCameraFromOverview(){
    //we don't need to do stream things here because it is all handled in OpenNewCameraOnIndex()
    KiwiLightApp::configeditor.OpenNewCameraFromOverview();
}

/**
 * Updates KiwiLight's utilities depending on it's state.
 */
void KiwiLightApp::UpdateApp() {
    try {
        if(KiwiLightApp::mode == UIMode::UI_EDITOR) {
            KiwiLightApp::configeditor.Update();
        }

        //update the camera error label based on how successful thread is being
        if(lastImageGrabSuccessful) {
            //wait for the output image to be updated by other thread
            while(KiwiLightApp::outImgInUse) {
                usleep(10);
            }
            KiwiLightApp::outImgInUse = true;
            KiwiLightApp::outputImage.Update(KiwiLightApp::lastFrameGrabImage);
            KiwiLightApp::outImgInUse = false;
            KiwiLightApp::cameraStatusLabel.SetText(""); 
        } else {
            KiwiLightApp::cameraStatusLabel.SetText("Camera Error!");
            KiwiLightApp::cameraFailures++;

            if(KiwiLightApp::cameraFailures > 50) {
                //attempt to reconnect the camera stream
                OpenNewCameraFromMainIndex();
                KiwiLightApp::cameraFailures = 0;
            }
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
    while(!retrieveSuccess && KiwiLightApp::mode != UIMode::UI_PAUSING && streamThreadEnabled) {
        usleep(250000); //give camera some time to adjust and do things
        bool grabSuccess = KiwiLightApp::camera.grab();
        if(grabSuccess) {
            Mat img;
            retrieveSuccess = KiwiLightApp::camera.retrieve(img);
        }
    }

    std::cout << "Camera Stream confirmed." << std::endl;

    while(KiwiLightApp::mode != UIMode::UI_PAUSING && streamThreadEnabled) {
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
            case UIMode::UI_STREAM: {
                    displayImage = KiwiLightApp::TakeImage();
                }
                break;
            case UIMode::UI_RUNNER: {
                    std::string output = KiwiLightApp::runner.Iterate();
                    KiwiLightApp::runner.GetLastFrameSuccessful(); //boolean
                    displayImage = KiwiLightApp::runner.GetOutputImage();
                    
                    //if the udp is enabled, send the message
                    if(KiwiLightApp::udpEnabled) {
                        KiwiLightApp::udpSender.Send(output);
                    }
                }
                break;
            case UIMode::UI_EDITOR: {
                    KiwiLightApp::configeditor.UpdateImageOnly(); //boolean
                    displayImage = KiwiLightApp::configeditor.GetOutputImage();
                    
                    std::string output = KiwiLightApp::configeditor.GetLastFrameResult(); //gets the results of the last runner iteration
                    
                    //send if udp enabled
                    if(KiwiLightApp::udpEnabled) {
                        KiwiLightApp::udpSender.Send(output);
                    }
                }
                break;
        }
        // if successful, update the display image
        if(KiwiLightApp::lastImageGrabSuccessful) {            
            //wait for out image to be used by other thread
            while(KiwiLightApp::outImgInUse) {
                usleep(10);
            }
            KiwiLightApp::outImgInUse = true;
            KiwiLightApp::lastFrameGrabImage = displayImage;
            KiwiLightApp::outImgInUse = false;
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
 * Enables or disables the runner's UDP.
 */
void KiwiLightApp::ToggleUDP() {
    KiwiLightApp::udpEnabled = !KiwiLightApp::udpEnabled;
    
    //set the button text
    KiwiLightApp::toggleUDPButton.SetText((KiwiLightApp::udpEnabled ? "Disable UDP" : "Enable UDP"));

    //set the buttons in the config editor if necessary
    if(KiwiLightApp::mode == UIMode::UI_EDITOR) {
        KiwiLightApp::configeditor.SetUDPEnabledLabels(KiwiLightApp::udpEnabled);
    }
}

/**
 * Causes KiwiLight to create and open a new configuration.
 */
void KiwiLightApp::NewConfiguration() {
    //set mode to stream and open a new editor.
    KiwiLightApp::mode = UIMode::UI_STREAM;
    EditConfiguration();
}

/**
 * Causes KiwiLight to open the editor for the selected configuration.
 * @param currentMode The UIMode active when callback was triggered
 */
void KiwiLightApp::EditConfiguration() {
    //since this is a callback, close streamer
    UIMode currentMode = KiwiLightApp::mode;
    std::string pathToOpen = "";
    StopStreamingThread();

    if(currentMode == UIMode::UI_STREAM) {
        //find generic.xml (in /home/user/KiwiLightData/confs)
        char *homedir = getenv("HOME");
        if(homedir == NULL) {
            std::cout << "The HOME environment variable could not be found." << std::endl;
            return;
        } else {
            pathToOpen = std::string(homedir) + "/KiwiLightData/confs/generic.xml";
        }
    } else if(currentMode == UIMode::UI_RUNNER) {
        std::cout << "taking runner path" << std::endl;
        pathToOpen = KiwiLightApp::runner.GetFileName();
    } else if(currentMode == UIMode::UI_PAUSING) {
        std::cout << "WARNING: UI mode unclear. Make sure EditConfiguration() is called before stream thread is terminated" << std::endl;
    }
    KiwiLightApp::configeditor = ConfigEditor(pathToOpen);
    LaunchStreamingThread(UIMode::UI_EDITOR);
}

/**
 * Causes KiwiLight to open a new configuration.
 */
void KiwiLightApp::OpenConfiguration() {    
    StopStreamingThread();
    FileChooser chooser = FileChooser(false, "");
    std::string fileToOpen = chooser.Show();
    
    OpenConfigurationFromFile(fileToOpen);
    LaunchStreamingThread(UIMode::UI_RUNNER);
}

/**
 * Causes KiwiLight to open the configuration specified by the file
 */
void KiwiLightApp::OpenConfigurationFromFile(std::string fileName) {
    XMLDocument newDoc = XMLDocument(fileName);
    UIMode newMode = UIMode::UI_STREAM;
    if(newDoc.HasContents()) {
        KiwiLightApp::confInfo.LoadConfig(newDoc);
        KiwiLightApp::runner = Runner(fileName, true);

        //open the specified camera
        int camIndex = std::stoi(newDoc.GetTagsByName("camera")[0].GetAttributesByName("index")[0].Value());
        OpenNewCameraOnIndex(camIndex);
        newMode = UIMode::UI_RUNNER;
    } else {
        std::cout << "New Document either empty or not specified. Taking no action." << std::endl;
    }
}

/**
 * Causes KiwiLight to close the currently opened configuration.
 */
void KiwiLightApp::CloseConfiguration() {
    KiwiLightApp::confInfo.Clear();
    KiwiLightApp::mode = UIMode::UI_STREAM;
}

/**
 * Causes KiwiLight to exit.
 */
void KiwiLightApp::Quit() {
    StopStreamingThread();
    gtk_main_quit();
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

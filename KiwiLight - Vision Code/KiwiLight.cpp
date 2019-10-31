#include "KiwiLight.h"

/**
 * Source file for the KiwiLightApp class.
 * Written By: Brach Knutson
 */

using namespace cv;
using namespace KiwiLight;

//define some static vars for KiwiLightApp. Vars will be defined for real on call to Create();
VideoCapture  KiwiLightApp::camera;
Runner        KiwiLightApp::runner;
ConfigEditor  KiwiLightApp::configeditor;
UIMode        KiwiLightApp::mode = UIMode::UI_STREAM;
bool          KiwiLightApp::lastFrameGrabSuccessful = false;
Mat           KiwiLightApp::lastFrameGrabImage;
Window        KiwiLightApp::win;
ConfigPanel   KiwiLightApp::confInfo;
NumberBox     KiwiLightApp::cameraIndexBox;
Label         KiwiLightApp::cameraStatusLabel;
Image         KiwiLightApp::outputImage;
Button        KiwiLightApp::toggleUDPButton;

/**
 * Initializes GTK and builds KiwiLight
 */
void KiwiLightApp::Create(int argc, char *argv[]) {
    KiwiLightApp::mode = UIMode::UI_STREAM;
    KiwiLightApp::camera = VideoCapture(0);

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
                    
                    Button openCameraButton = Button("Open", KiwiLightApp::OpenNewCamera);
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
}

/**
 * Starts KiwiLight.
 */
void KiwiLightApp::Start() {
    win.Show();
    KiwiLightApp::LaunchStreamingThread();
    win.Main();
}

/**
 * Returns KiwiLight's camera.
 */
VideoCapture KiwiLightApp::GetCamera() { return camera; }

/**
 * Returns KiwiLight's runner.
 */
Runner KiwiLightApp::GetRunner() { return runner; }

/**
 * Returns KiwiLight's config editor.
 */
ConfigEditor KiwiLightApp::GetEditor() { return configeditor; }

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
void KiwiLightApp::LaunchStreamingThread() {
    g_thread_new("stream monitor", GThreadFunc(KiwiLightApp::UpdateStreamsConstantly), NULL);
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
    KiwiLightApp::runner = Runner(KiwiLightApp::configeditor.GetFileName(), true, KiwiLightApp::camera);
    KiwiLightApp::mode = UIMode::UI_RUNNER;
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
    KiwiLightApp::configeditor.RecheckUDP();
}

/**
 * Causes the editor to set the image resolution from the overview panel.
 */
void KiwiLightApp::EditorSetImageResolutionFromOverview() {
    KiwiLightApp::configeditor.ResetResolutionFromOverview();
}

/**
 * Causes the editor to connect the UDP from the overview panel.
 */
void KiwiLightApp::EditorConnectUDPFromOverview() {
    KiwiLightApp::configeditor.ReconnectUDPFromOverview();
}

/**
 * Causes the editor to apply all camera settings to the video stream.
 */
void KiwiLightApp::EditorApplyCameraSettings() {
    KiwiLightApp::configeditor.ApplyCameraSettings();
}

/**
 * Updates KiwiLight's utilities depending on it's state.
 */
void KiwiLightApp::UpdateApp() {
    if(KiwiLightApp::lastFrameGrabSuccessful) {
        KiwiLightApp::cameraStatusLabel.SetText("");
        KiwiLightApp::outputImage.Update(KiwiLightApp::lastFrameGrabImage);
    } else {
        KiwiLightApp::cameraStatusLabel.SetText("Camera Error!");
    }

    if(KiwiLightApp::mode == UIMode::UI_EDITOR) {
        KiwiLightApp::configeditor.Update();
    }
}

/**
 * Updates the streams nonstop until mode is set to UI_QUITTING
 */
void KiwiLightApp::UpdateStreamsConstantly() {
    while(KiwiLightApp::mode != UIMode::UI_QUTTING) {
        KiwiLightApp::UpdateStreams();
    }
    
    std::cout << "stream terminated!" << std::endl;

    g_thread_exit(0);
}

/**
 * Updates KiwiLight's video streams.
 */
void KiwiLightApp::UpdateStreams() {
    bool streamSuccess = false;
    Mat displayImage;
    switch(KiwiLightApp::mode) {
        case UIMode::UI_STREAM:
            streamSuccess = KiwiLightApp::camera.read(displayImage);
            break;
        case UIMode::UI_RUNNER:
            KiwiLightApp::runner.Iterate();
            streamSuccess = KiwiLightApp::runner.GetLastFrameSuccessful();
            displayImage = KiwiLightApp::runner.GetOutputImage();
            break;
        case UIMode::UI_EDITOR:
            streamSuccess = KiwiLightApp::configeditor.UpdateImageOnly();
            displayImage = KiwiLightApp::configeditor.GetOutputImage();
            break;
    }
    
    if(displayImage.empty()) {
        std::cout << "DISPLAY IMAGE EMPTY!" << std::endl;
    }

    KiwiLightApp::lastFrameGrabSuccessful = streamSuccess;
    KiwiLightApp::lastFrameGrabImage = displayImage;
}

/**
 * Opens a new videostream on the index that the user selects.
 */
void KiwiLightApp::OpenNewCamera() {
    int newIndex = (int) KiwiLightApp::cameraIndexBox.GetValue();
    KiwiLightApp::camera.~VideoCapture();
    KiwiLightApp::camera = VideoCapture(newIndex);
}

/**
 * Enables or disables the runner's UDP.
 */
void KiwiLightApp::ToggleUDP() {
    std::cout << "toggle UDP" << std::endl;
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
 */
void KiwiLightApp::EditConfiguration() {
    std::string pathToOpen = "";

    if(KiwiLightApp::mode == UIMode::UI_STREAM) {
        //find generic.xml (in /home/user/KiwiLightData/confs)
        char *homedir = getenv("HOME");
        if(homedir == NULL) {
            std::cout << "The HOME environment variable could not be found." << std::endl;
            return;
        } else {
            pathToOpen = std::string(homedir) + "/KiwiLightData/confs/generic.xml";
        }
    } else if(KiwiLightApp::mode == UIMode::UI_RUNNER) {
        pathToOpen = KiwiLightApp::runner.GetFileName();
    }

    KiwiLightApp::configeditor = ConfigEditor(pathToOpen, KiwiLightApp::camera);
    KiwiLightApp::mode = UIMode::UI_EDITOR;
}

/**
 * Causes KiwiLight to open a new configuration.
 */
void KiwiLightApp::OpenConfiguration() {
    FileChooser chooser = FileChooser(false, "");
    std::string fileToOpen = chooser.Show();

    KiwiLightApp::confInfo.LoadConfig(XMLDocument(fileToOpen));
    KiwiLightApp::runner = Runner(fileToOpen, true, KiwiLightApp::camera);
    KiwiLightApp::mode = UIMode::UI_RUNNER;
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
    KiwiLightApp::mode = UIMode::UI_QUTTING;
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

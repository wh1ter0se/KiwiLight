#include "KiwiLight.h"

/*---------------------------------------------------------------*/
/*          .d8888b.   .d8888b.   .d8888b.  888888888            */
/*         d88P  Y88b d88P  Y88b d88P  Y88b 888                  */
/*              .d88P 888        888    888 888                  */
/*             8888"  888d888b.  Y88b. d888 8888888b.            */
/*              "Y8b. 888P "Y88b  "Y888P888      "Y88b           */
/*         888    888 888    888        888        888           */
/*         Y88b  d88P Y88b  d88P Y88b  d88P Y88b  d88P           */
/*          "Y8888P"   "Y8888P"   "Y8888P"   "Y8888P"            */
/*                                                               */
/*  This code was written by FRC3695 - Foximus Prime and stored  */
/*   at github.com/wh1ter0se/KiwiLight. KiwiLight is published   */
/*     under a GPL-3.0 license, permitting modification and      */
/*      distribution under the condition that the source is      */
/* disclosed and distribution is accompanied by the same license */
/*---------------------------------------------------------------*/

using namespace cv;
using namespace KiwiLight;

Panel content;

NumberBox cameraIndex;

ConfigPanel configPanel;

ConfigEditor configEditor;

Label cameraStatusLabel;

VideoCapture cam;

Image imgFrame;

Mat outputImage;

Runner runner;

bool displayingImage = false,
     cameraOpen = true,
     imageCaptureSuccess = false,
     pauseStreamer = false,
     udpEnabledForIterate = false;

UIMode uiMode;

GThread *runnerThread,
        *streamerThread;


/**
 * Runs through a checklist and updates UI objects, utilities, etc.
 */
void Update() {
    //to prevent interval overlap, do this if-statement thing
    if(!displayingImage) {
        displayingImage = true;

        if(imageCaptureSuccess) {
            try {
                imgFrame.Update(outputImage);
                cameraStatusLabel.SetText("");
            } catch(cv::Exception ex) {
                // std::cout << "cv exception in update()" << std::endl;
            }
        } else {
            cameraOpen = false;
            cameraStatusLabel.SetText("Error Streaming Camera!!!");
        }
        displayingImage = false;
    }

    if(uiMode == UIMode::UI_EDITOR) {
        configEditor.Update();
    }

    //check for update flags
    if(Flags::GetFlag("CloseCamera")) {
        //pause streamer. It will stay paused until the end of StartCamera handling.
        pauseStreamer = true;
        Flags::LowerFlag("CloseCamera");
        cameraOpen = false;
        displayingImage = true; //block out updating image so nothing uses camera
        
        Flags::RaiseFlag("CameraClosed");
    }

    if(Flags::GetFlag("StartCamera")) {
        Flags::LowerFlag("StartCamera");
        cameraOpen = true;
        configEditor.ResetRunnerResolution();
        pauseStreamer = false;
    }

    if(Flags::GetFlag("CloseEditor")) {
        pauseStreamer = true;
        Flags::LowerFlag("CloseEditor");
        runner = Runner(configEditor.GetFileName(), true, configEditor.GetVideoCapture());
        uiMode = UIMode::UI_RUNNER;
        pauseStreamer = false;
        configEditor.Close();
    }

    if(Flags::GetFlag("SaveAndCloseEditor")) {
        //pause streamer so it doesnt cause problems
        pauseStreamer = true;
        Flags::LowerFlag("SaveAndCloseEditor");
        runner = Runner(configEditor.GetFileName(), true, configEditor.GetVideoCapture());
        uiMode = UIMode::UI_RUNNER;
        configEditor.Save();
        configEditor.Close();
        pauseStreamer = false;
    }
}

void OpenNewCamera() {
    if(uiMode == UIMode::UI_STREAM) {
        pauseStreamer = true;
        int newIndex = (int) cameraIndex.GetValue();

        cam.~VideoCapture();
        cam = VideoCapture(newIndex);

        //unfreeze the stream
        displayingImage = false;
        pauseStreamer = false;
    }
}

/**
 * Saves the config currently in the editor.
 */
void SaveConfig() {
    if(uiMode == UIMode::UI_EDITOR) {
        configEditor.Save();
    } else {
        std::cout << "oops! there be no big config bro" << std::endl;
    }
}

/**
 * Based on the value of UIMode, Updates the output image in a separate thread
 */
void RunStream() {
    while(uiMode != UIMode::UI_QUTTING) {
        if(!pauseStreamer) {
            switch(uiMode) {
                case UIMode::UI_STREAM:
                    imageCaptureSuccess = cam.read(outputImage);
                    break;
                case UIMode::UI_EDITOR:
                    imageCaptureSuccess = true;
                    configEditor.UpdateImageOnly();
                    outputImage = configEditor.GetOutputImage();
                    break;
                case UIMode::UI_RUNNER:
                    imageCaptureSuccess = true;
                    runner.Iterate();
                    outputImage = runner.GetOutputImage();
                    break;
            }
        }
    }

    Flags::RaiseFlag("StreamerQuit");
    g_thread_exit(0);
}


/**
 * Opens a file menu to open a config.
 */
void OpenConfig() {
    pauseStreamer = true;
   
    FileChooser chooser = FileChooser(false, "");
    std::string file = chooser.Show();

    //if file was selected and "cancel" was not pressed
    if(file != "") {
        configPanel.LoadConfig(file);
        runner = Runner(file, true, cam);
        uiMode = UIMode::UI_RUNNER;
    } 
    
    pauseStreamer = false;
}

/**
 * Returns camera stream back to normal streaming, no extra processing.
 */
void StopUsingRunner() {
    if(uiMode == UIMode::UI_RUNNER) {
        pauseStreamer = true;
        configPanel.Clear();
        displayingImage = false;
        cam = runner.GetVideoStream();
        uiMode = UIMode::UI_STREAM;
        pauseStreamer = false;
    }
}

/**
 * Edits the selected config, or does nothing if nothing is selected.
 */
void EditSelected() {
    pauseStreamer = true; //pause streamer so that the editor can complete its initalization

    if(uiMode == UIMode::UI_RUNNER) {
        configEditor = ConfigEditor(runner.GetFileName(), cam);
        configEditor.SetUDPEnabled(runner.GetUDPEnabled());
    } else if(uiMode == UIMode::UI_STREAM) {
        std::string genericFilePath = "";
        char *home = getenv("HOME");
        if(home != NULL) {
            genericFilePath = std::string(home) + "/KiwiLightData/confs/generic.xml";
        } else {
            //oh HECK. Show the user the error message
            std::cout << "the HOME environment variable does not exist, therfore I cannot access generic!" << std::endl;
            ConfirmationDialog error = ConfirmationDialog("ERROR:the HOME environment variable does not exist!");
            error.ShowAndGetResponse();
            return;
        }
        std::cout << "path: " << genericFilePath << std::endl;
        configEditor = ConfigEditor(genericFilePath, cam);
    }
    
    uiMode = UIMode::UI_EDITOR;
    pauseStreamer = false;
}


void ToggleUDP() {
    bool udpEnabled;

    if(uiMode == UIMode::UI_RUNNER) {
        bool udpIsEnabled = runner.GetUDPEnabled();
        runner.SetUDPEnabled(!udpIsEnabled);
        udpEnabled = !udpIsEnabled;
    }

    if(uiMode == UIMode::UI_EDITOR) {
        bool udpIsEnabled = configEditor.GetUDPEnabled();
        configEditor.SetUDPEnabled(!udpIsEnabled);
        udpEnabled = !udpIsEnabled;
    }

    //update UI based on what state the UDP is.
    configPanel.SetUDPEnabled(udpEnabled);
    udpEnabledForIterate = udpEnabled;
}

/**
 * Opens a form for the user to create a new vision config.
 */
void AddConfig() {
    // will edit generic xml as new file
    pauseStreamer = true;
    uiMode = UIMode::UI_STREAM; //set to stream so that editSelected() will init a new config
    EditSelected();
}

/**
 * Shows the about menu and flexes about how Foximus Prime is a cool FRC team
 */
void ShowAbout() {
    AboutWindow abtWindow = AboutWindow(GTK_WINDOW_TOPLEVEL);
    abtWindow.Show();
}

/**
 * directs you to the github repo where tutorials for the app are shown
 */
void HELPME() {
    HelpWindow hlpWindow = HelpWindow(GTK_WINDOW_TOPLEVEL);
    hlpWindow.Show();
}


void Quit() {
    uiMode = UIMode::UI_QUTTING;
    while(!Flags::GetFlag("StreamerQuit")) {
        //haha do nothing
    }

    gtk_main_quit();
}

/**
 * Creates a menu bar and adds it to the content panel.
 */
void CreateMenuBar() {
    //create a new menubar
    MenuBar menubar = MenuBar();
        menubar.SetName("hover_gray");
        MenuItem file = MenuItem("File");
            SubMenuItem addConfig = SubMenuItem("New Configuration", AddConfig);
                file.AddSubmenuItem(addConfig);

            SubMenuItem openConfig = SubMenuItem("Open Configuration", OpenConfig);
                file.AddSubmenuItem(openConfig);

            SubMenuItem saveConfig = SubMenuItem("Save Configuration", SaveConfig);
                file.AddSubmenuItem(saveConfig);

            SubMenuItem stopConfig = SubMenuItem("Close Config", StopUsingRunner);
                file.AddSubmenuItem(stopConfig);


            SubMenuItem quit = SubMenuItem("Quit", Quit);
                file.AddSubmenuItem(quit);

            menubar.AddItem(file);

        // MenuItem config = MenuItem("Config");
        //     SubMenuItem confBoot = SubMenuItem("Configure Start on Boot", ConfStartConfigOnBoot);
        //         config.AddSubmenuItem(confBoot);

        //     menubar.AddItem(config);

        MenuItem help = MenuItem("Help");
            SubMenuItem about = SubMenuItem("About", ShowAbout);
                help.AddSubmenuItem(about);

            SubMenuItem helpme = SubMenuItem("How the heck does this work?", HELPME);
                help.AddSubmenuItem(helpme);

            menubar.AddItem(help);

    content.Pack_start(menubar.GetWidget(), false, false, 0);
}


/**
 * KIWILIGHT MAIN ENTRY POINT!!! 
 * KiwiLight will enter, create a UI, and start the main loop in this method.
 */
int main(int argc, char *argv[]) {

    //check argc. If 1 arg (aka: only the command) is present, make ui, otherwise, the user wants something else.
    if(argc == 1) {
        //lets make ourselves a ui
        gtk_init(&argc, &argv);

        cam = VideoCapture(0);
        uiMode = UIMode::UI_STREAM;

        Window win = Window(GTK_WINDOW_TOPLEVEL);
            win.SetCSS("ui/Style.css");
            win.SetSize(300,200);
            content = Panel(false, 0);
                CreateMenuBar();
                
                Panel header = Panel(true, 0);
                    header.SetName("mainHeader");

                    Image logo = Image("banner_small.png");
                        header.Pack_start(logo.GetWidget(), false, false, 0);
                    
                    Separator logoSep = Separator(false);
                        header.Pack_start(logoSep.GetWidget(), false, false, 5);

                    Panel cameraIndexPanel = Panel(false, 0);

                        Panel indexSelectorPanel = Panel(true, 5);

                            Label cameraLabel = Label("Camera: ");
                                indexSelectorPanel.Pack_start(cameraLabel.GetWidget(), false, false, 0);
                            
                            cameraIndex = NumberBox(0, 100, 0);
                                indexSelectorPanel.Pack_start(cameraIndex.GetWidget(), false, false, 0);

                            cameraIndexPanel.Pack_start(indexSelectorPanel.GetWidget(), false, false, 0);

                        Button openCamButton = Button("Open", OpenNewCamera);
                            cameraIndexPanel.Pack_start(openCamButton.GetWidget(), false, false, 0);

                        header.Pack_start(cameraIndexPanel.GetWidget(), false, false, 0);

                        cameraStatusLabel = Label("");
                            header.Pack_start(cameraStatusLabel.GetWidget(), false, false, 10);

                    content.Pack_start(header.GetWidget(), false, false, 0);

                Panel body = Panel(true, 0);
                    configPanel = ConfigPanel("");
                        body.Pack_start(configPanel.GetWidget(), false, false, 0);

                    Panel imagePanel = Panel(false, 0);
                        // inital image for the stream
                        imgFrame = Image(ImageColorspace::RGB);
                            imagePanel.Pack_start(imgFrame.GetWidget(), true, true, 0);

                        body.Pack_start(imagePanel.GetWidget(), true, true, 0);

                    content.Pack_start(body.GetWidget(), true, true, 0);
            win.SetPane(content);

        //create and initalize the streamer thread
        streamerThread = g_thread_new("streamer", GThreadFunc(RunStream), NULL);

        //set events and show Window
        win.SetOnAppClosed(Quit);
        win.SetInterval(75, Update);
        win.Show();
        win.Main(); //MAIN LOOP

        return 0;
    } else {
        // -r means run config
        if(std::string(argv[1]) == "-r") {
            std::string fileToRun = std::string(argv[2]);
            runner = Runner(fileToRun, false);
            runner.Loop();
            return 0;
        }

        // -h means "help"
        if(std::string(argv[1]) == "-h") {
            std::cout << "KIWILIGHT COMMANDS: " << std::endl;
            std::cout << "Usage: ./KiwiLight [options] [filename]" << std::endl;
            std::cout <<  std::endl;
            std::cout << "Options:" << std::endl;
            std::cout << "-r: Run a configuration. \"filename\" must be specified." << std::endl;
            std::cout << "-h: Display this help menu." << std::endl;
            return 0;
        }

        std::cout << "No valid command specified. Aborting." << std::endl;
    }
}

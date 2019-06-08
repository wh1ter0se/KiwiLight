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
     pauseStreamer = false;

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

        //since config running is not addressed in the streamer, do it here
        if(uiMode == UIMode::UI_CONFIG_RUNNING) {
            outputImage = runner.GetOutputImage();
        }

        if(imageCaptureSuccess) {
            // try {
                imgFrame.Update(outputImage);
                cameraStatusLabel.SetText("");
            // } catch(cv::Exception ex) {
            //     std::cout << "cv exception in update()" << std::endl;
            // }
        } else {
            cameraOpen = false;
            cameraStatusLabel.SetText("Error Streaming Camera!!!");
        }
        displayingImage = false;
    }


    //update editor outside of displaying image because it has the ability to stop the camera
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
        if(uiMode == UIMode::UI_RUNNER) {
            runner.Stop();
        } else {
            cam.~VideoCapture();
        }
        
        Flags::RaiseFlag("CameraClosed");
    }

    if(Flags::GetFlag("StartCamera")) {
        Flags::LowerFlag("StartCamera");
        cameraOpen = true;
        displayingImage = false;
        if(uiMode == UIMode::UI_RUNNER) {
            runner.Start();
        } else if(uiMode == UIMode::UI_RUNNER) {
            cam = VideoCapture(cameraIndex.GetValue());
        }
        pauseStreamer = false;
    }

    if(Flags::GetFlag("SaveAndCloseEditor")) {
        //pause streamer so it doesnt cause problems
        pauseStreamer = true;
        Flags::LowerFlag("SaveAndCloseEditor");
        configEditor.Save();
        configEditor.Close();

        runner = Runner(configEditor.GetFileName(), true);
        uiMode = UIMode::UI_RUNNER;
        pauseStreamer = false;
    }
}

void OpenNewCamera() {
    if(uiMode == UIMode::UI_STREAM) {
        pauseStreamer = true;
        int newIndex = (int) cameraIndex.GetValue();

        cam.release();
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
    while(uiMode != UIMode::UI_CONFIG_RUNNING) {
        if(!pauseStreamer) {
            switch(uiMode) {
                case UIMode::UI_STREAM:
                    imageCaptureSuccess = cam.read(outputImage);
                    break;
                case UIMode::UI_EDITOR:
                    imageCaptureSuccess = true;
                    outputImage = configEditor.GetOutputImage();
                    break;
                case UIMode::UI_RUNNER:
                    imageCaptureSuccess = true;
                    outputImage = runner.GetOutputImage();
                    break;
                case UIMode::UI_CONFIG_RUNNING:
                    g_thread_exit(0);
                    break;
            }
        }
    }

    g_thread_exit(0);
}

/**
 * Runs a runner loop in a separate thread
 */
void StartRunnerLoop() {
    runner.Loop();
    g_thread_exit(0);
}

/**
 * Runs the selected config, or does nothing if nothing is selected.
 */
void RunSelected() {
    std::string fileName = "";
    if(uiMode == UIMode::UI_RUNNER) {
        fileName = runner.GetFileName();
    } else if(uiMode == UIMode::UI_EDITOR) {
        fileName = configEditor.GetFileName();
        configEditor.Close();
    } else if(uiMode != UIMode::UI_CONFIG_RUNNING) {
        ConfirmationDialog warning = ConfirmationDialog("Please open a configuration to run it.");
        warning.ShowAndGetResponse();
        return;
    }

    if(uiMode == UIMode::UI_CONFIG_RUNNING) {
        runner.StopLoopOnly();
        g_thread_unref(runnerThread);
        uiMode = UIMode::UI_RUNNER;
        pauseStreamer = false;
    } else {
        //pause streamer and keep it paused until running ends
        pauseStreamer = true;
        uiMode = UIMode::UI_CONFIG_RUNNING;
        runner.UnlockLoop(); //unlock the runner in case it is locked
        runnerThread = g_thread_new("runnerThread", GThreadFunc(StartRunnerLoop), NULL);
    }
}

/**
 * Sets up the config to start when the computer boots up
 */
void ConfStartConfigOnBoot() {
    std::cout << "conf start on boot" << std::endl;
}

/**
 * Compiles the config and leaves it there.
 */
void CompileConfig() {
    std::string configFileName = "";
    if(uiMode == UIMode::UI_RUNNER) {
        configFileName = runner.GetFileName();
    } else if(uiMode == UIMode::UI_EDITOR) {
        configFileName = configEditor.GetFileName();
        configEditor.Close();
    } else if(uiMode != UIMode::UI_CONFIG_RUNNING) {
        ConfirmationDialog warning = ConfirmationDialog("Please open a configuration to compile it.");
        warning.ShowAndGetResponse();
        return;
    }

    std::string fileContents = "#include \"KiwiLight.h\"\n" + 
                   std::string("\n") + 
                   std::string("using namespace KiwiLight;\n") + 
                   std::string("\n") + 
                   std::string("int main() {\n") + 
                   std::string("Runner runner = Runner(\"" + configFileName + "\");\n") + 
                   std::string("runner.Loop();\n") + 
                   std::string("return 0;\n") + 
                   std::string("}\n");

    std::string fileName = "temp.cpp";

    std::ofstream file = std::ofstream(fileName.c_str(), std::ofstream::out);
    file.write(fileContents.c_str(), fileContents.length());

    file.close();
}

/**
 * Opens a file menu to open a config.
 */
void OpenConfig() {
    std::cout << "oc1" << std::endl;
    pauseStreamer = true;
    if(uiMode == UIMode::UI_RUNNER) {
        runner.Stop();
    }
    std::cout << "oc2" << std::endl;
    cam.~VideoCapture();
    std::cout << "oc3" << std::endl;
    FileChooser chooser = FileChooser(false, "");
    std::string file = chooser.Show();

    //if file was selected and "cancel" was not pressed
    if(file != "") {
        configPanel.LoadConfig(file);
        runner = Runner(file, true);
        uiMode = UIMode::UI_RUNNER;
    } else { //otherwise...
        if(uiMode == UIMode::UI_RUNNER) {
            runner.Start();
        } else if(uiMode == UIMode::UI_STREAM) {
            cam = VideoCapture((int) cameraIndex.GetValue());
        }
    }
    pauseStreamer = false;
}

/**
 * Returns camera stream back to normal streaming, no extra processing.
 */
void StopUsingRunner() {
    if(uiMode == UIMode::UI_RUNNER) {
        pauseStreamer = true;
        runner.Stop();
        configPanel.Clear();
        cam = VideoCapture((int) cameraIndex.GetValue());
        displayingImage = false;
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
        runner.Stop();
        configEditor = ConfigEditor(runner.GetFileName());
    } else if(uiMode == UIMode::UI_STREAM) {
        cam.~VideoCapture();
        configEditor = ConfigEditor("confs/generic.xml");
    }
    
    uiMode = UIMode::UI_EDITOR;
    pauseStreamer = false;
}

/**
 * Opens a form for the user to create a new vision config.
 */
void AddConfig() {
    // will edit generic xml as new file
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


            SubMenuItem quit = SubMenuItem("Quit", gtk_main_quit);
                file.AddSubmenuItem(quit);

            menubar.AddItem(file);

        MenuItem config = MenuItem("Config");
            SubMenuItem runConfig = SubMenuItem("Run Config", RunSelected);
                config.AddSubmenuItem(runConfig);

            SubMenuItem compConfig = SubMenuItem("Compile Config", CompileConfig);
                config.AddSubmenuItem(compConfig);

            SubMenuItem confBoot = SubMenuItem("Configure Start on Boot", ConfStartConfigOnBoot);
                config.AddSubmenuItem(confBoot);

            menubar.AddItem(config);

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

    //check argc. If 1 arg is present, make ui, otherwise, the user wants something else.
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
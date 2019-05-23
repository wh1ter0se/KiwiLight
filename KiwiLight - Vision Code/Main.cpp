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

ImageFrame imgFrame;

Runner runner;

bool displayingImage = false,
     cameraOpen = true;

UIMode uiMode;


/**
 * Runs through a checklist and updates UI objects, utilities, etc.
 */
void Update() {
    cv::Mat img;
    bool success = true; //did the videocapture work correctly?

    //to prevent interval overlap, do this if-statement thing
    if(!displayingImage) {
        displayingImage = true;

        if(uiMode == UIMode::UI_RUNNER) {
            success = true;
            runner.Iterate();
            img = runner.GetOutputImage();
        } else if (uiMode == UIMode::UI_STREAM) {
            success = cam.read(img);
        } else if(uiMode == UIMode::UI_EDITOR) {
            success = true;
            img = configEditor.GetOutputImage();
        }

        if(success) {
            // try {
                //convert image to 16 bit for bgr-rgb conversion. Not doing this step may result in segfault
                img.convertTo(img, CV_16U);
                cvtColor(img, img, COLOR_BGR2RGB);
                img.convertTo(img, CV_8U);

                //display image
                Image forFrame = Image(img);
                imgFrame.Update(forFrame);
                cameraStatusLabel.SetText("");
            // } catch(cv::Exception ex) {
                // std::cout << "WARNING: IMAGE DISPLAY ERROR DETECTED!" << std::endl;
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
    }
}

void OpenNewCamera() {
    if(uiMode == UIMode::UI_STREAM) {
        int newIndex = cameraIndex.GetValue();

        cam.release();
        cam.~VideoCapture();
        cam = VideoCapture(newIndex);

        //unfreeze the stream
        displayingImage = false;
    }
}

/**
 * Saves the config currently in the editor.
 */
void SaveConfig() {
    if(uiMode = UIMode::UI_EDITOR) {
        configEditor.Save();
    } else {
        std::cout << "oops! there be no big config bro" << std::endl;
    }
}

/**
 * Runs the selected config, or does nothing if nothing is selected.
 */
void RunSelected() {
    std::cout << "run selected from MAIN" << std::endl;
    std::cout.flush();
}

/**
 * Sets up the config to start when the computer boots up
 */
void ConfStartConfigOnBoot() {
    std::cout << "conf start on boot" << std::endl;
    std::cout.flush();
}

/**
 * Compiles the config and leaves it there.
 */
void CompileConfig() {
    std::cout << "compile config" << std::endl;
    std::cout.flush();
}

/**
 * Opens a file menu to open a config.
 */
void OpenConfig() {
    if(uiMode == UIMode::UI_RUNNER) {
        runner.Stop();
    }
    cam.~VideoCapture();
    FileChooser chooser = FileChooser(true);
    std::string file = chooser.Show();
    if(file != "") {
        configPanel.LoadConfig(file);
        runner = Runner(file, true);
        uiMode = UIMode::UI_RUNNER;
    }
}

/**
 * Returns camera stream back to normal streaming, no extra processing.
 */
void StopUsingRunner() {
    if(uiMode == UIMode::UI_RUNNER) {
        uiMode = UIMode::UI_STREAM;
        runner.Stop();
        configPanel.Clear();
        cam = VideoCapture((int) cameraIndex.GetValue());
        displayingImage = false;
    }
}

/**
 * Edits the selected config, or does nothing if nothing is selected.
 */
void EditSelected() {
    if(uiMode == UIMode::UI_RUNNER) {
        runner.Stop();
        configEditor = ConfigEditor(runner.GetFileName());
    } else if(uiMode == UIMode::UI_STREAM) {
        cam.~VideoCapture();
        configEditor = ConfigEditor("confs/generic.xml");
    }

    uiMode = UIMode::UI_EDITOR;
}

/**
 * Opens a form for the user to create a new vision config.
 */
void AddConfig() {
    //will edit generic xml as new file
    EditSelected();
}

/**
 * Shows the about menu and flexes about how Foximus Prime is a cool FRC team
 */
void ShowAbout() {
    std::cout << "show about menu" << std::endl;
    std::cout.flush();
}

/**
 * directs you to the github repo where tutorials for the app are shown
 */
void HELPME() {
    std::cout << "HELP" << std::endl;
    std::cout.flush();
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

                Image banner = Image("banner_small.png");
                ImageFrame logo = ImageFrame(banner);
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
                    //inital image for the stream
                    cv::Mat img;
                    cam.read(img);
                    img.convertTo(img, CV_32F);
                    cvtColor(img, img, COLOR_BGR2RGB);
                    img.convertTo(img, CV_8U);
                    Image initalImage = Image(img);
                    imgFrame = ImageFrame(initalImage);
                        imagePanel.Pack_start(imgFrame.GetWidget(), true, false, 0);

                    body.Pack_start(imagePanel.GetWidget(), true, false, 0);

                content.Pack_start(body.GetWidget(), true, false, 0);
        win.SetPane(content);

    //set events and show Window
    win.SetInterval(75, Update);
    win.Show();
    win.Main(); //MAIN LOOP

    return 0;
}
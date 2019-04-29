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


using namespace KiwiLight;

Panel content;

NumberBox cameraIndex;

// Camera cam(0);
VideoCapture cam(0);

ImageFrame imgFrame;

/**
 * Runs through a checklist and updates UI objects, utilities, etc.
 */
void Update() {
    cv::Mat img;
    cam.read(img);

    //convert the image to RGB (convert to 32f MUST be done)
    //if 32f is not done, segfault is possible when displaying in GTK
    img.convertTo(img, CV_32F);
    cvtColor(img, img, COLOR_BGR2RGB);
    img.convertTo(img, CV_8U);
    Image forFrame = Image(img);
    imgFrame.Update(forFrame);
}

/**
 * Creates a menu bar and adds it to the content panel.
 */
void CreateMenuBar() {
    //create a new menubar
    MenuBar menubar = MenuBar();
        MenuItem file = MenuItem("File");
            SubMenuItem addConfig = SubMenuItem("New Configuration", Kiwi::AddConfig);
                file.AddSubmenuItem(addConfig);

            SubMenuItem openConfig = SubMenuItem("Open Configuration", Kiwi::OpenConfig);
                file.AddSubmenuItem(openConfig);

            SubMenuItem saveConfig = SubMenuItem("Save Configuration", Kiwi::SaveConfig);
                file.AddSubmenuItem(saveConfig);

            SubMenuItem quit = SubMenuItem("Quit", gtk_main_quit);
                file.AddSubmenuItem(quit);

            menubar.AddItem(file);

        MenuItem config = MenuItem("Config");
            SubMenuItem runConfig = SubMenuItem("Run Config", Kiwi::RunSelected);
                config.AddSubmenuItem(runConfig);

            SubMenuItem compConfig = SubMenuItem("Compile Config", Kiwi::CompileConfig);
                config.AddSubmenuItem(compConfig);

            SubMenuItem confBoot = SubMenuItem("Configure Start on Boot", Kiwi::ConfStartConfigOnBoot);
                config.AddSubmenuItem(confBoot);

            menubar.AddItem(config);

        MenuItem help = MenuItem("Help");
            SubMenuItem about = SubMenuItem("About", Kiwi::ShowAbout);
                help.AddSubmenuItem(about);

            SubMenuItem helpme = SubMenuItem("How the heck does this work?", Kiwi::HELPME);
                help.AddSubmenuItem(helpme);

            menubar.AddItem(help);

    content.Pack_start(menubar.GetWidget(), false, false, 0);
}

void OpenNewCamera() {
    
}

/**
 * KIWILIGHT MAIN ENTRY POINT!!! 
 * KiwiLight will enter, create a UI, and start the main loop in this method.
 */
int main(int argc, char *argv[]) {
    std::cout << "m1" << std::endl;
    //lets make ourselves a ui
    gtk_init(&argc, &argv);

    Window win = Window();
        win.SetSize(300,200);
        content = Panel(false, 5);
            CreateMenuBar();
            
            Panel header = Panel(true, 0);

                Image banner = Image("banner_small.png");
                ImageFrame logo = ImageFrame(banner);
                    header.Pack_start(logo.GetWidget(), true, false, 0);
                
                Separator logoSep = Separator(false);
                    header.Pack_start(logoSep.GetWidget(), true, false, 5);
                
                Label cameraLabel = Label("Camera: ");
                    header.Pack_start(cameraLabel.GetWidget(), true, false, 0);
                
                cameraIndex = NumberBox(0, 100, 0);
                    header.Pack_start(cameraIndex.GetWidget(), true, false, 0);

                Button openCamButton = Button("Open", OpenNewCamera);
                    header.Pack_start(openCamButton.GetWidget(), true, false, 0);

                content.Pack_start(header.GetWidget(), true, false, 0);

            Panel body = Panel(true, 0);
                ConfigPanel configPanel = ConfigPanel();
                    body.Pack_start(configPanel.GetWidget(), true, true, 0);

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
    std::cout << "m2" << std::endl;
    win.Main();

    return 0;
}
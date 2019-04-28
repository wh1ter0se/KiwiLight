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

// Window win;

Panel content;

// MenuBar menubar;

VideoCapture camera;

ImageFrame imgFrame;

/**
 * Runs through a checklist and updates UI objects, utilities, etc.
 */
void Update() {
    cv::Mat newImg;
    bool success = camera.read(newImg);
    if(success) {
        //NOTE: Image MUST be converted to 32f before converting to rgb.
        //      Not doing this step may result in a segmentation fault when displaying image.
        newImg.convertTo(newImg, CV_32F);
        cvtColor(newImg, newImg, COLOR_BGR2RGB);
        newImg.convertTo(newImg, CV_8U);

        Image imgForFrame = Image(newImg);
        imgFrame.Update(imgForFrame);
    }
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
    //lets make ourselves a ui
    gtk_init(&argc, &argv);

    camera = VideoCapture(0);

    Window win = Window();
        win.SetSize(300,200);
        content = Panel(false, 5);
            CreateMenuBar();
            ConfigPanel conf = ConfigPanel();
                content.Pack_start(conf.GetWidget(), true, true, 0);

                Mat cvImg;
                camera.read(cvImg);

                cvImg.convertTo(cvImg, CV_32F);
                cvtColor(cvImg, cvImg, COLOR_BGR2RGB);
                cvImg.convertTo(cvImg, CV_8U);

                Image img = Image(cvImg);
                imgFrame = ImageFrame(img);
                    content.Pack_start(imgFrame.GetWidget(), true, false, 0);

                //make the panel containing the buttons
                Panel buttonPanel = Panel(true, 0);
                    Button runButton = Button("Run", Kiwi::RunSelected);
                        buttonPanel.Pack_start(runButton.GetWidget(), true, true, 0);

                    Button editButton = Button("Edit", Kiwi::EditSelected);
                        buttonPanel.Pack_start(editButton.GetWidget(), true, true, 0);

                    content.Pack_start(buttonPanel.GetWidget(), true, false, 0);
                    
        win.SetPane(content);

    //set events and show Window
    win.SetInterval(75, Update);
    win.Show();

    win.Main();

    return 0;
}
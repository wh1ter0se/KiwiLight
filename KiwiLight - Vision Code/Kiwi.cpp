#include "KiwiLight.h"

using namespace std;
using namespace cv;
using namespace KiwiLight;

/**
 * Source file for the Kiwi class, which controls all user interactions with the program.
 * Written By: Brach Knutson
 */

Camera Kiwi::cam = Camera(0);

/**
 * Creates and runs a new Kiwi
 */
Kiwi::Kiwi( void (*UpdateMethod)() ) {

    gtk_init(NULL, NULL);
    //lets make a new UI broskissss
    this->win = Window();
        this->win.SetSize(600, 400);
        this->content = Panel(false, 0);
            CreateMenuBar(); //create the menu bar at the top of the window

        //now add different panels to the UI
        this->data = Panel(true, 5);
            this->config = Frame("Config"); //the box thing that all vision configuration info goes in
                this->data.Pack_start(this->config, true, true, 5);
            
            this->cameraFrame = Frame("Camera");
                this->camera = Panel(false, 15); //the box where you can edit camera exposure and stuff
                        this->camSettings = Settings();
                        this->camera.Pack_start(this->camSettings, true, false, 0);

                    cameraFrame.Pack(this->camera);
                this->data.Pack_start(this->cameraFrame, true, true, 5);

            this->content.Pack_start(this->data, true, true, 0);
        
        win.SetPane(content);
    win.SetInterval(25, UpdateMethod);
    win.Show();
    win.Main();
}

/**
 * Forces the settings widget to update.
 */
void Kiwi::UpdateSettings() {
    this->camSettings.Update();
}

/**
 * Opens a form for the user to create a new vision config.
 */
void Kiwi::AddConfig() {
    cout << "new config" << endl;
    cout.flush();
}

/**
 * Saves the config currently in the editor.
 */
void Kiwi::SaveConfig() {
    cout << "save config" << endl;
    cout.flush();
}

/**
 * Runs the selected config, or does nothing if nothing is selected.
 */
void Kiwi::RunSelected() {
    cout << "run selected" << endl;
    cout.flush();
}

/**
 * Sets up the config to start when the computer boots up
 */
void Kiwi::ConfStartConfigOnBoot() {
    cout << "conf start on boot" << endl;
    cout.flush();
}

/**
 * Compiles the config and leaves it there.
 */
void Kiwi::CompileConfig() {
    cout << "compile config" << endl;
    cout.flush();
}

/**
 * Opens a file menu to open a config.
 */
void Kiwi::OpenConfig() {
    cout << "open config" << endl;
    cout.flush();
}

/**
 * Edits the selected config, or does nothing if nothing is selected.
 */
void Kiwi::EditSelected() {
    cout << "edit selected" << endl;
    cout.flush();
}

/**
 * Forces the Kiwi Camera to update, also takes care of some tasks for the UI
 */
void Kiwi::UpdateCamera() {
    Kiwi::cam.Update();

}

/**
 * Shows the about menu and flexes about how Foximus Prime is a cool FRC team
 */
void Kiwi::ShowAbout() {
    cout << "show about menu" << endl;
    cout.flush();
}

/**
 * directs you to the github repo where tutorials for the app are shown
 */
void Kiwi::HELPME() {
    cout << "HELP" << endl;
    cout.flush();
}


void Kiwi::CreateMenuBar() {
    //create a new menubar
        this->menubar = MenuBar();
            MenuItem file = MenuItem("File");
                SubMenuItem addConfig = SubMenuItem("New Configuration", Kiwi::AddConfig);
                    file.AddSubmenuItem(addConfig);

                SubMenuItem openConfig = SubMenuItem("Open Config", Kiwi::OpenConfig);
                    file.AddSubmenuItem(openConfig);

                SubMenuItem saveConfig = SubMenuItem("SaveConfig", Kiwi::SaveConfig);
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

        content.Pack_start(this->menubar, false, false, 0);
}
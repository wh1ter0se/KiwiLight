#include "KiwiLight.h"

/**
 * Main entry point for KiwiLight program.
 * Written By: Brach Knutson
 */


using namespace KiwiLight;

Window win;

Panel content,
      dataPanel,
      cameraPanel,
      cameraLabelPanel;

Frame configFrame,
      cameraFrame;

Label cameraLabel,
      cameraStatus;

TextBox CameraIndex;

Button ApplyCameraIndex;

Settings settings;

MenuBar menubar;

Camera cam;

/**
 * Runs through a checklist and updates UI objects, utilities, etc.
 */
void Update() {
    cam.Update();
    settings.Update();

    cam.SetWidth(settings.GetWidth());
    cam.SetHeight(settings.GetHeight());

    if(!cam.isOpen()) {
        cameraStatus.SetText("Error Streaming Camera!!!");
    } else {
        cameraStatus.SetText("");
    }
}

/**
 * Creates a menu bar and adds it to the content panel.
 */
void CreateMenuBar() {
    //create a new menubar
    menubar = MenuBar();
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

    content.Pack_start(menubar.GetWidget(), false, false, 0);
}

void OpenNewCamera() {
    std::cout << "open camera" << std::endl;
    std::cout.flush();

    std::string indexString = CameraIndex.GetText();
    
    try {
        int newIndex = std::stoi(indexString);
        cam.SetIndex(newIndex);
    } catch(...) {
        CameraIndex.SetText(std::to_string(cam.GetIndex()));
    }
    
}

/**
 * KIWILIGHT MAIN ENTRY POINT!!! 
 * KiwiLight will enter, create a UI, and start the main loop in this method.
 */
int main() {
    //lets make ourselves a ui
    gtk_init(NULL, NULL);

    cam = Camera(0);

    win = Window();
        content = Panel(false, 5);
            CreateMenuBar();
            dataPanel = Panel(true, 5);
                configFrame = Frame("Config");
                    dataPanel.Pack_start(configFrame.GetWidget(), true, false, 0);
                
                cameraFrame = Frame("Camera");
                    cameraPanel = Panel(false, 0);
                        cameraLabelPanel = Panel(true, 0);
                            cameraLabel = Label("Camera Device ");
                                cameraLabel.SetFont("Monospace");
                                cameraLabel.SetFontSize(16.0);
                                cameraLabelPanel.Pack_start(cameraLabel.GetWidget(), true, false, 0);

                            CameraIndex = TextBox("0");
                                cameraLabelPanel.Pack_start(CameraIndex.GetWidget(), true, false, 0);

                            ApplyCameraIndex = Button("Open", OpenNewCamera);
                                cameraLabelPanel.Pack_start(ApplyCameraIndex.GetWidget(), true, false, 0);
                            
                            cameraPanel.Pack_start(cameraLabelPanel.GetWidget(), true, false, 0);

                        cameraStatus = Label("");
                            cameraStatus.SetFont("Monospace");
                            cameraStatus.SetFontSize(11.0);
                            cameraPanel.Pack_start(cameraStatus.GetWidget(), true, false, 0);

                        settings = Settings(0);
                            cameraPanel.Pack_start(settings.GetWidget(), true, false, 0);
                        
                        cameraFrame.Pack(cameraPanel.GetWidget());
                    dataPanel.Pack_start(cameraFrame.GetWidget(), true, false, 0);

                content.Pack_start(dataPanel.GetWidget(), true, false, 0);

        win.SetPane(content);
    
    //set events and show Window
    win.SetInterval(25, Update);
    win.Show();
    win.Main();

    return 0;
}
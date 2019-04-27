#include "KiwiLight.h"

/**
 * Source file for the ConfigPanel class
 * Written By: Brach Knutson
 */

using namespace KiwiLight;

/**
 * Creates a new config panel. No configuration will be loaded.
 */
ConfigPanel::ConfigPanel() {
    this->panel = Panel(false, 5);
        this->header = Label("Configuration");
            this->header.SetName("header");
            this->panel.Pack_start(header.GetWidget(), true, false, 0);
        
        this->description = Label("Go to \"File->Open Configuration\" to load a config, or \"file->New Configuration\" to create a new one.");
            this->description.SetLineWrap(true);
            this->panel.Pack_start(description.GetWidget(), true, false, 0);

    this->configPanel = this->panel.GetWidget();
}

/**
 * Loads the config described the the file at fileName.
 */
void ConfigPanel::LoadConfig(std::string fileName) {
    std::cout << "load file: " << fileName << std::endl;
    std::cout.flush();
}
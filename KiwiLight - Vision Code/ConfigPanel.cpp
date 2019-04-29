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
        this->header = Label("Configuration: (none loaded)");
            this->header.SetName("header");
            this->panel.Pack_start(header.GetWidget(), true, true, 0);

        this->informationPanel = Panel(false, 5);
        
            this->fileLabel = Label("File: (none)");
                fileLabel.SetJustify(Label::JUSTIFY_LEFT);
                this->informationPanel.Pack_start(this->fileLabel.GetWidget(), false, false, 0);

            this->PreProcessorLabel = Label("Preprocessor: (none)");
                this->informationPanel.Pack_start(this->PreProcessorLabel.GetWidget(), false, false, 0);

            this->PostProcessorLabel = Label("Postprocessor: (none)");
                this->informationPanel.Pack_start(this->PostProcessorLabel.GetWidget(), false, false, 0);

            this->TargetLabel = Label("Targets: (none)");
                this->informationPanel.Pack_start(this->TargetLabel.GetWidget(), false, false, 0);

            this->UDPAddressLabel = Label("UDP Address: (none)");
                this->informationPanel.Pack_start(this->UDPAddressLabel.GetWidget(), false, false, 0);

            this->UDPPortLabel = Label("UDP Port: (none)");
                this->informationPanel.Pack_start(this->UDPPortLabel.GetWidget(), false, false, 0);

            this->panel.Pack_start(this->informationPanel.GetWidget(), false, false, 0);

        Separator sep = Separator(true);
            this->panel.Pack_start(sep.GetWidget(), true, false, 5);
        
        this->buttonPanel = Panel(true, 0);
            this->editConfig = Button("Edit", Kiwi::EditSelected);
                this->buttonPanel.Pack_start(editConfig.GetWidget(), true, true, 0);

            this->runConfig = Button("Run", Kiwi::RunSelected);
                this->buttonPanel.Pack_start(runConfig.GetWidget(), true, true, 0);

            this->panel.Pack_start(this->buttonPanel.GetWidget(), true, true, 0);

    this->configPanel = this->panel.GetWidget();
}

/**
 * Loads the config described the the file at fileName.
 */
void ConfigPanel::LoadConfig(std::string fileName) {
    std::cout << "load file: " << fileName << std::endl;
    std::cout.flush();
}


void ConfigPanel::SetName(std::string name) {
    gtk_widget_set_name(this->configPanel, name.c_str());
}
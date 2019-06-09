#include "KiwiLight.h"

/**
 * Source file for the ConfigPanel class
 * Written By: Brach Knutson
 */

using namespace KiwiLight;

// extern void RunSelected(); //from Main.cpp
extern void EditSelected(); //from Main.cpp

/**
 * Creates a new config panel. No configuration will be loaded.
 */
ConfigPanel::ConfigPanel(std::string configFilePath) {
    this->configFile = configFilePath;
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
            this->editConfig = Button("Edit", EditSelected);
                this->buttonPanel.Pack_start(editConfig.GetWidget(), true, true, 0);

            this->panel.Pack_start(this->buttonPanel.GetWidget(), true, true, 0);

    if(configFilePath != "") {
        this->LoadConfig(configFilePath);
    }

    this->configPanel = this->panel.GetWidget();
}

/**
 * Loads the config described the the file at fileName.
 */
void ConfigPanel::LoadConfig(std::string fileName) {
    //create an xmldocument to parse and then find all information needed to fill out the label
    XMLDocument file = XMLDocument(fileName);

    XMLTag config = file.GetTagsByName("configuration")[0];
        std::string name = config.GetAttributesByName("name")[0].Value();
        std::string preProcessorType = config.GetTagsByName("preprocessor")[0].GetAttributesByName("type")[0].Value();
        std::string postProcessorType = "full";
        std::string numTargets = std::to_string(config.GetTagsByName("postprocessor")[0].GetTagsByName("target").size());

        XMLTag UDPTag = config.GetTagsByName("postprocessor")[0].GetTagsByName("UDP")[0];
            std::string udpAddress = UDPTag.GetTagsByName("address")[0].Content();
            std::string udpPort    = UDPTag.GetTagsByName("port")[0].Content();

    //set the label texts for the informational panel with the found information
    this->header.SetText("Configuration: " + name);
    this->fileLabel.SetText("File: " + fileName);
    this->PreProcessorLabel.SetText("Preprocessor: " + preProcessorType);
    this->PostProcessorLabel.SetText("Postprocessor: " + postProcessorType);
    this->TargetLabel.SetText("Targets: " + numTargets);
    this->UDPAddressLabel.SetText("UDP Address: " + udpAddress);
    this->UDPPortLabel.SetText("UDP Port: " + udpPort);
}


void ConfigPanel::Clear() {
    this->header.SetText("Configuration: (none loaded)");
    this->fileLabel.SetText("File: (none)");
    this->PreProcessorLabel.SetText("Preprocessor: (none)");
    this->PostProcessorLabel.SetText("Postprocessor: (none)");
    this->TargetLabel.SetText("Targets: (none)");
    this->UDPAddressLabel.SetText("UDP Address: (none)");
    this->UDPPortLabel.SetText("UDP Port: (none)");
}


void ConfigPanel::SetName(std::string name) {
    gtk_widget_set_name(this->configPanel, name.c_str());
}
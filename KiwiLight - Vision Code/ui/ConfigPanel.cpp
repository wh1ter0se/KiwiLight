#include "UI.h"

/**
 * Source file for the ConfigPanel class
 * Written By: Brach Knutson
 */

using namespace KiwiLight;

extern void EditSelected(); //from Main.cpp
extern void ToggleUDP(); //from Main.cpp


ConfigPanel::ConfigPanel(XMLDocument file, bool withButtons, bool withDynamicName) {
    this->panel = Panel(false, 5);

        this->configFile = "(none)";
        std::string configName = "(none)";
        std::string preprocessorType = "(none)";
        std::string numContours = "(none)";
        std::string udpAddr = "(none)";
        std::string udpPort = "(none)";

        if(file.HasContents()) {
            this->configFile = file.FileName();

            XMLTag confTag = file.GetTagsByName("configuration")[0];
            configName = confTag.GetAttributesByName("name")[0].Value();

            XMLTag preprocessorTag = confTag.GetTagsByName("preprocessor")[0];
            preprocessorType = preprocessorTag.GetAttributesByName("type")[0].Value();

            XMLTag postprocessorTag = confTag.GetTagsByName("postprocessor")[0];
            XMLTag targetTag = postprocessorTag.GetTagsByName("target")[0];
            numContours = std::to_string(targetTag.GetTagsByName("contour").size());

            XMLTag udpTag = postprocessorTag.GetTagsByName("UDP")[0];
            udpAddr = udpTag.GetTagsByName("address")[0].Content();
            udpPort = udpTag.GetTagsByName("port")[0].Content();
        }

        this->configNameString = configName;

        if(withDynamicName) {
            //dynamic name mode selected; the name of the config will be able to be changed in the panel.
            this->dynamicName = true;
            
            Panel headerPanel = Panel(true, 0);
                this->header = Label("Configuration: ");
                    this->header.SetName("header");
                    headerPanel.Pack_start(this->header.GetWidget(), false, false, 0);

                this->configName = TextBox(configName);
                    headerPanel.Pack_start(this->configName.GetWidget(), false, false, 0);

                this->panel.Pack_start(headerPanel.GetWidget(), false, false, 0);
        } else {
            this->dynamicName = false;
            this->header = Label("Configuration: " + configName);
                this->header.SetName("header");
                this->panel.Pack_start(this->header.GetWidget(), false, false, 0);
        }

        Panel filePanel = Panel(true, 0);
            Label filePanelHeader = Label("File: ");
                filePanelHeader.SetName("gray");
                filePanel.Pack_start(filePanelHeader.GetWidget(), false, false, 0);
            
            this->fileLabel = Label(this->configFile);
                filePanel.Pack_start(this->fileLabel.GetWidget(), false, false, 0);

            this->panel.Pack_start(filePanel.GetWidget(), false, false, 0);

        Panel preprocessorPanel = Panel(true, 0);
            Label preprocessorPanelHeader = Label("Preprocessor: ");
                preprocessorPanelHeader.SetName("gray");
                preprocessorPanel.Pack_start(preprocessorPanelHeader.GetWidget(), false, false, 0);

            this->PreProcessorLabel = Label(preprocessorType);
                preprocessorPanel.Pack_start(this->PreProcessorLabel.GetWidget(), false, false, 0);

            this->panel.Pack_start(preprocessorPanel.GetWidget(), false, false, 0);

        Panel targetPanel = Panel(true, 0);
            Label targetPanelHeader = Label("Contours: ");
                targetPanelHeader.SetName("gray");
                targetPanel.Pack_start(targetPanelHeader.GetWidget(), false, false, 0);

            this->TargetLabel = Label(numContours);
                targetPanel.Pack_start(this->TargetLabel.GetWidget(), false, false, 0);

            this->panel.Pack_start(targetPanel.GetWidget(), false, false, 0);

        Panel udpAddrPanel = Panel(true, 0);
            Label udpAddrPanelHeader = Label("UDP Address: ");
                udpAddrPanelHeader.SetName("gray");
                udpAddrPanel.Pack_start(udpAddrPanelHeader.GetWidget(), false, false, 0);

            this->UDPAddressLabel = Label(udpAddr);
                udpAddrPanel.Pack_start(this->UDPAddressLabel.GetWidget(), false, false, 0);

            this->panel.Pack_start(udpAddrPanel.GetWidget(), false, false, 0);

        Panel udpPortPanel = Panel(true, 0);
            Label udpPortPanelHeader = Label("UDP Port: ");
                udpPortPanelHeader.SetName("gray");
                udpPortPanel.Pack_start(udpPortPanelHeader.GetWidget(), false, false, 0);

            this->UDPPortLabel = Label(udpPort);
                udpPortPanel.Pack_start(this->UDPPortLabel.GetWidget(), false, false, 0);
        
            this->panel.Pack_start(udpPortPanel.GetWidget(), false, false, 0);

        if(withButtons) {
            Separator sep = Separator(true);
                this->panel.Pack_start(sep.GetWidget(), false, false, 5);

            this->buttonPanel = Panel(true, 0);
                this->editConfig = Button("Edit", EditSelected);
                    this->buttonPanel.Pack_start(editConfig.GetWidget(), true, true, 0);

                this->toggleUDP = Button("Enable UDP", ToggleUDP);
                    this->buttonPanel.Pack_start(toggleUDP.GetWidget(), true, true, 0);

                this->panel.Pack_start(this->buttonPanel.GetWidget(), false, false, 0);
            
        }

    this->configPanel = this->panel.GetWidget();
}

/**
 * Sets the UI based on whether or not the UDP is enabled.
 */
void ConfigPanel::SetUDPEnabled(bool enabled) {
    if(enabled) {
        this->toggleUDP.SetText("Disable UDP");
    } else {
        this->toggleUDP.SetText("Enable UDP");
    }
}


void ConfigPanel::LoadConfig(std::string fileName) {
    this->LoadConfig(XMLDocument(fileName));
}

/**
 * Loads the config described the the file at fileName.
 */
void ConfigPanel::LoadConfig(XMLDocument file) {
    //create an xmldocument to parse and then find all information needed to fill out the label

    XMLTag config = file.GetTagsByName("configuration")[0];
        std::string name = config.GetAttributesByName("name")[0].Value();
        std::string preProcessorType = config.GetTagsByName("preprocessor")[0].GetAttributesByName("type")[0].Value();
        std::string postProcessorType = "full";
        std::string numContours = std::to_string(config.GetTagsByName("postprocessor")[0].GetTagsByName("target")[0].GetTagsByName("contour").size());

        XMLTag UDPTag = config.GetTagsByName("postprocessor")[0].GetTagsByName("UDP")[0];
            std::string udpAddress = UDPTag.GetTagsByName("address")[0].Content();
            std::string udpPort    = UDPTag.GetTagsByName("port")[0].Content();

    //set the label texts for the informational panel with the found information
    this->header.SetText("Configuration: " + name);
    this->fileLabel.SetText("File: " + file.FileName());
    this->PreProcessorLabel.SetText("Preprocessor: " + preProcessorType);
    this->TargetLabel.SetText("Contours: " + numContours);
    this->UDPAddressLabel.SetText("UDP Address: " + udpAddress);
    this->UDPPortLabel.SetText("UDP Port: " + udpPort);

    this->configNameString = name;
}


void ConfigPanel::Clear() {
    this->header.SetText("Configuration: (none loaded)");
    this->fileLabel.SetText("File: (none)");
    this->PreProcessorLabel.SetText("Preprocessor: (none)");
    this->TargetLabel.SetText("Targets: (none)");
    this->UDPAddressLabel.SetText("UDP Address: (none)");
    this->UDPPortLabel.SetText("UDP Port: (none)");
}


void ConfigPanel::SetConfigurationName(std::string newName) {
    if(this->dynamicName) {
        this->configName.SetText(newName);
    }
}


std::string ConfigPanel::GetConfigurationName() {
    if(this->dynamicName) {
        return this->configName.GetText();
    }

    return this->configNameString;
}


void ConfigPanel::SetName(std::string name) {
    gtk_widget_set_name(this->configPanel, name.c_str());
}

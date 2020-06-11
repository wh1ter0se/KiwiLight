#include "UI.h"

/**
 * Source file for the ConfigPanel class
 * Written By: Brach Knutson
 */

using namespace KiwiLight;

/**
 * Creates a new ConfigPanel displaying information from "file"
 */
ConfigPanel::ConfigPanel(XMLDocument file) {
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
       
        this->header = Label("Configuration: " + configName);
            this->header.SetName("header");
            this->panel.Pack_start(this->header.GetWidget(), false, false, 0);

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

    this->widget = this->panel.GetWidget();
}

/**
 * Loads config information from fileName.
 */
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

    //set the file name
    this->configFile = file.FileName();

    //set the label texts for the informational panel with the found information
    this->header.SetText("Configuration: " + name);
    this->fileLabel.SetText(this->configFile);
    this->PreProcessorLabel.SetText(preProcessorType);
    this->TargetLabel.SetText(numContours);
    this->UDPAddressLabel.SetText(udpAddress);
    this->UDPPortLabel.SetText(udpPort);

    this->configNameString = name;
}

/**
 * Sets the panel information to reflect that no config is loaded.
 */
void ConfigPanel::Clear() {
    this->configFile = "(none)";
    this->header.SetText("Configuration: (none)");
    this->fileLabel.SetText(configFile);
    this->PreProcessorLabel.SetText("(none)");
    this->TargetLabel.SetText("(none)");
    this->UDPAddressLabel.SetText("(none)");
    this->UDPPortLabel.SetText("(none)");
}


std::string ConfigPanel::GetConfigFile() {
    if(this->configFile == "(none)") {
        return "";
    }

    return this->configFile;
}
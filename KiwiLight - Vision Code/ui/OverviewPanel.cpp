#include "../KiwiLight.h"

/**
 * Source file for the OverviewPanel class.
 * Written By: Brach Knutson
 */

using namespace cv;
using namespace KiwiLight;


OverviewPanel::OverviewPanel(XMLDocument doc) {
    XMLTag configTag = doc.GetTagsByName("configuration")[0];
    Panel editor = Panel(false, 0);
        Panel namePanel = Panel(true, 0);
            Label confNameHeader = Label("Configuration: ");
                confNameHeader.SetName("header");
                namePanel.Pack_start(confNameHeader.GetWidget(), false, false, 0);
            
            std::string realConfigName = configTag.GetAttributesByName("name")[0].Value();
            this->configName = TextBox(realConfigName);
                namePanel.Pack_start(this->configName.GetWidget(), false, false, 0);
            
            editor.Pack_start(namePanel.GetWidget(), false, false, 0);

        Label imageResolutionHeader = Label("Image Resolution");
            imageResolutionHeader.SetName("subHeader");
            editor.Pack_start(imageResolutionHeader.GetWidget(), true, true, 0);

        std::vector<XMLTag> camerasettings = 
            doc.GetTagsByName("camera")[0]
            .GetTagsByName("settings")[0]
            .GetTagsByName("setting");

        Panel imageResolutionPanel = Panel(true, 0);
            Panel imageResolutionContents = Panel(false, 0);
                Panel resXPanel = Panel(true, 0);
                    Label resXHeader = Label("Width: ");
                        resXPanel.Pack_start(resXHeader.GetWidget(), true, true, 0);

                    int realImgResX = std::stoi(Util::SearchCameraSettingsByID(camerasettings, CAP_PROP_FRAME_WIDTH).Content());
                    this->imgResX = NumberBox(100, 2000, 1, realImgResX);
                        resXPanel.Pack_start(this->imgResX.GetWidget(), true, true, 0);

                    imageResolutionContents.Pack_start(resXPanel.GetWidget(), true, true, 0);

                Panel resYPanel = Panel(true, 0);
                    Label resYHeader = Label("Height: ");
                        resYPanel.Pack_start(resYHeader.GetWidget(), true, true, 0);

                    int realImgResY = std::stoi(Util::SearchCameraSettingsByID(camerasettings, CAP_PROP_FRAME_HEIGHT).Content());
                    this->imgResY = NumberBox(100, 2000, 1, realImgResY);
                        resYPanel.Pack_start(this->imgResY.GetWidget(), true, true, 0);

                    imageResolutionContents.Pack_start(resYPanel.GetWidget(), true, true, 0);

                imageResolutionPanel.Pack_start(imageResolutionContents.GetWidget(), true, true, 0);

            Button setResolution = Button("Set", KiwiLightApp::EditorSetImageResolutionFromOverview);
                imageResolutionPanel.Pack_start(setResolution.GetWidget(), true, true, 0);

            editor.Pack_start(imageResolutionPanel.GetWidget(), true, true, 0);

        Label udpHeader = Label("UDP");
            udpHeader.SetName("subHeader");
            editor.Pack_start(udpHeader.GetWidget(), true, true, 0);

        XMLTag udpTag = doc.GetTagsByName("configuration")[0].GetTagsByName("postprocessor")[0].GetTagsByName("UDP")[0];
        Panel udpPanel = Panel(true, 0);
            Panel udpEditorContents = Panel(false, 0);
                Panel udpAddrPanel = Panel(true, 0);
                    Label udpAddrPanelHeader = Label("IPv4 Address: ");
                        udpAddrPanel.Pack_start(udpAddrPanelHeader.GetWidget(), true, true, 0);
                    
                    std::string realUDP = udpTag.GetTagsByName("address")[0].Content();
                    this->udpAddr = TextBox(realUDP);
                        udpAddrPanel.Pack_start(this->udpAddr.GetWidget(), true, true, 0);

                    udpEditorContents.Pack_start(udpAddrPanel.GetWidget(), true, true, 0);

                Panel udpPortPanel = Panel(true, 0);
                    Label udpPortPanelHeader = Label("Port: ");
                        udpPortPanel.Pack_start(udpPortPanelHeader.GetWidget(), true, true, 0);

                    int realUDPPort = std::stoi(udpTag.GetTagsByName("port")[0].Content());
                    this->udpPort = NumberBox(1, 9999, 1, realUDPPort);
                        udpPortPanel.Pack_start(this->udpPort.GetWidget(), true, true, 0);

                    udpEditorContents.Pack_start(udpPortPanel.GetWidget(), true, true, 0);

                udpPanel.Pack_start(udpEditorContents.GetWidget(), true, true, 0);

            Button reconnectUDP = Button("Reconnect", KiwiLightApp::EditorConnectUDPFromOverview);
                udpPanel.Pack_start(reconnectUDP.GetWidget(), true, true, 0);

            editor.Pack_start(udpPanel.GetWidget(), true, true, 0);

    this->overviewpanel = editor.GetWidget();
}

/**
 * Updates the overview panel.
 */
void OverviewPanel::Update() {

}

void OverviewPanel::SetConfigName(std::string name) {
    this->configName.SetText(name);
}

std::string OverviewPanel::GetConfigName() {
    return this->configName.GetText();
}

void OverviewPanel::SetUDPAddr(std::string addr) {
    this->udpAddr.SetText(addr);
}

std::string OverviewPanel::GetUDPAddr() {
    return this->udpAddr.GetText();
}

void OverviewPanel::SetUDPPort(int port) {
    this->udpPort.SetValue(port);
}

int OverviewPanel::GetUDPPort() {
    return (int) this->udpPort.GetValue();
}

void OverviewPanel::SetImageResolution(Size res) { 
    this->imgResX.SetValue(res.width);
    this->imgResY.SetValue(res.height);
}

Size OverviewPanel::GetImageResolution() { 
    int resX = (int) this->imgResX.GetValue();
    int resY = (int) this->imgResY.GetValue();
    return Size(resX, resY);
}


void OverviewPanel::SetName(std::string name) {
    gtk_widget_set_name(this->overviewpanel, name.c_str());
}
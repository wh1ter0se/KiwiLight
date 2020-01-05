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

        Label targetInformationHeader = Label("Target Information:");
            targetInformationHeader.SetName("subHeader");
            editor.Pack_start(targetInformationHeader.GetWidget(), true, true, 0);

        //the real time target information panel
        Panel targetInformationPanel = Panel(false, 0);
            this->targetSpotted = Label("Target Spotted: NO");
            this->targetSpotted.SetName("gray");
                targetInformationPanel.Pack_start(this->targetSpotted.GetWidget(), false, false, 0);

            this->targetImageLocation = Label("Target Location: N/A");
                targetInformationPanel.Pack_start(this->targetImageLocation.GetWidget(), false, false, 0);

            this->targetDist = Label("Target Distance: N/A");
                targetInformationPanel.Pack_start(this->targetDist.GetWidget(), false, false, 0);

            this->targetHAngle = Label("Target Horizontal Angle: N/A");
                targetInformationPanel.Pack_start(this->targetHAngle.GetWidget(), false, false, 0);

            this->targetVAngle = Label("Target Vertical Angle: N/A");
                targetInformationPanel.Pack_start(this->targetVAngle.GetWidget(), false, false, 0);

            editor.Pack_start(targetInformationPanel.GetWidget(), true, true, 0);

        Label cameraHeader = Label("Camera");
            cameraHeader.SetName("subHeader");
            editor.Pack_start(cameraHeader.GetWidget(), true, true, 0);

        Panel cameraPanel = Panel(true, 0);
            Label cameraIndexHeader = Label("Camera Index:");
                cameraPanel.Pack_start(cameraIndexHeader.GetWidget(), true, true, 0);

            int realCameraIndex = std::stoi(doc.GetTagsByName("camera")[0].GetAttributesByName("index")[0].Value());
            this->cameraIndex = NumberBox(0, 100, 1, realCameraIndex);
                cameraPanel.Pack_start(this->cameraIndex.GetWidget(), true, true, 0);

            Button applyIndexButton = Button("Open", KiwiLightApp::EditorOpenNewCameraFromOverview);
                cameraPanel.Pack_start(applyIndexButton.GetWidget(), true, true, 0);
            editor.Pack_start(cameraPanel.GetWidget(), true, true, 0);

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

            this->enableUDP = Button("Enable", KiwiLightApp::ToggleUDP);
                udpPanel.Pack_start(this->enableUDP.GetWidget(), true, true, 0);

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


void OverviewPanel::SetTargetInformationLabels(bool targetSpotted, int targetImgX, int targetImgY, double targetDist, double targetHAngle, double targetVAngle) {
    this->targetSpotted.SetText(std::string("Target Spotted: ") + (targetSpotted ? std::string("YES") : std::string("NO")));
    
    std::string
        targetLocationString = "N/A",
        targetDistString = "N/A",
        targetHAngleString = "N/A",
        targetVAngleString = "N/A";

    if(targetSpotted) {
        targetLocationString = "( " + std::to_string(targetImgX) + ", " + std::to_string(targetImgY) + ")";
        targetDistString = std::to_string(targetDist);
        targetHAngleString = std::to_string(targetHAngle);
        targetVAngleString = std::to_string(targetVAngle);
    }

    this->targetImageLocation.SetText("Target Location " + targetLocationString);
    this->targetDist.SetText("Target Distance: " + targetDistString);
    this->targetHAngle.SetText("Target Horizontal Angle: " + targetHAngleString);
    this->targetVAngle.SetText("Target Vertical Angle: " + targetVAngleString);
}


void OverviewPanel::SetCameraIndex(int index) {
    this->cameraIndex.SetValue((double) index);
}


int OverviewPanel::GetCameraIndex() {
    return this->cameraIndex.GetValue();
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

void OverviewPanel::SetUDPEnabledLabels(bool UDPEnabled) {
    this->enableUDP.SetText((UDPEnabled ? "Disable" : "Enable"));
}


void OverviewPanel::SetTargetInformationLabelsFromString(std::string iterOutput) {
    if(iterOutput.length() < 3) { //improperly formatted string
        return;
    }

    std::string trimmedOutput = iterOutput.substr(1, iterOutput.length() - 2); //sub off the ':' and ';' at beginning and end
    std::vector<std::string> splitOutput = StringUtils::SplitString(trimmedOutput, ',');

    //there must be 5 nums in string, no more, no less
    if(splitOutput.size() == 5) {
        int targetX = std::stoi(splitOutput[0]);
        int targetY = std::stoi(splitOutput[1]);
        double targetDist = std::stod(splitOutput[2]);
        double targetAngleHorizontal = std::stod(splitOutput[3]);
        double targetAngleVertical = std::stod(splitOutput[4]);
        bool targetSpotted = (targetX > -1);

        SetTargetInformationLabels(targetSpotted, targetX, targetY, targetDist, targetAngleHorizontal, targetAngleVertical);
    } else {
        std::cout << "WARNING: Could not update overview panel information labels. Input string was wrongly formatted." << std::endl;
    }
}


void OverviewPanel::SetName(std::string name) {
    gtk_widget_set_name(this->overviewpanel, name.c_str());
}
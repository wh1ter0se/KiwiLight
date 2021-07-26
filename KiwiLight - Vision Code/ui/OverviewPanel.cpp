#include "../KiwiLight.h"

/**
 * Source file for the OverviewPanel class.
 * Written By: Brach Knutson
 */

using namespace cv;
using namespace KiwiLight;

/**
 * Creates a new OverviewPanel dispaying information from doc.
 */
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

            this->targetSize = Label("Target Size: N/A");
                targetInformationPanel.Pack_start(this->targetSize.GetWidget(), false, false, 0);

            this->targetDist = Label("Target Distance: N/A");
                targetInformationPanel.Pack_start(this->targetDist.GetWidget(), false, false, 0);

            this->targetHAngle = Label("Target Horizontal Angle: N/A");
                targetInformationPanel.Pack_start(this->targetHAngle.GetWidget(), false, false, 0);

            this->targetVAngle = Label("Target Vertical Angle: N/A");
                targetInformationPanel.Pack_start(this->targetVAngle.GetWidget(), false, false, 0);

            editor.Pack_start(targetInformationPanel.GetWidget(), false, false, 0);

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

            this->enableUDP = Button("Disable", KiwiLightApp::ToggleUDP);
                udpPanel.Pack_start(this->enableUDP.GetWidget(), true, true, 0);

            editor.Pack_start(udpPanel.GetWidget(), true, true, 0);

    this->widget = editor.GetWidget();
}

/**
 * Sets the name of the configuration being displayed.
 */
void OverviewPanel::SetConfigName(std::string name) {
    this->configName.SetText(name);
}

/**
 * Returns the name of the configuration being displayed.
 */
std::string OverviewPanel::GetConfigName() {
    return this->configName.GetText();
}

/**
 * Sets the text of the information labels.
 * @param targetSpotted Whether or not the target is currently spotted.
 * @param targetImgX The X coordinate of the target in image space.
 * @param targetImgY The Y coordinate of the target in image space.
 * @param targetImgW The width of the target in image space.
 * @param targetImgH The height of the target in image space.
 * @param targetDist The distance from the camera to the target.
 * @param targetHAngle The horizontal angle that the camera must turn to be aligned with the target.
 * @param targetVAngle The vertical angle that the camera must turn to be aligned with the target.
 */
void OverviewPanel::SetTargetInformationLabels(
    bool targetSpotted, 
    int targetImgX, 
    int targetImgY, 
    int targetImgW, 
    int targetImgH,
    double targetDist,
    double targetHAngle, 
    double targetVAngle
) {
    this->targetSpotted.SetText(std::string("Target Spotted: ") + (targetSpotted ? std::string("YES") : std::string("NO")));
    
    std::string
        targetLocationString = "N/A",
        targetSizeString = "N/A",
        targetDistString = "N/A",
        targetHAngleString = "N/A",
        targetVAngleString = "N/A";

    if(targetSpotted) {
        targetLocationString = "(" + std::to_string(targetImgX) + ", " + std::to_string(targetImgY) + ")";
        targetSizeString = "(" + std::to_string(targetImgW) + ", " + std::to_string(targetImgH) + ")";
        targetDistString = std::to_string(targetDist);
        targetHAngleString = std::to_string(targetHAngle);
        targetVAngleString = std::to_string(targetVAngle);
    }

    this->targetImageLocation.SetText("Target Location " + targetLocationString);
    this->targetSize.SetText("Target Size: " + targetSizeString);
    this->targetDist.SetText("Target Distance: " + targetDistString);
    this->targetHAngle.SetText("Target Horizontal Angle: " + targetHAngleString);
    this->targetVAngle.SetText("Target Vertical Angle: " + targetVAngleString);
}

/**
 * Sets the index of the camera to use.
 */
void OverviewPanel::SetCameraIndex(int index) {
    this->cameraIndex.SetValue((double) index);
}

/**
 * Returns the index of the camera being used.
 */
int OverviewPanel::GetCameraIndex() {
    return this->cameraIndex.GetValue();
}

/**
 * Sets the text of the "UDP Address" field.
 */
void OverviewPanel::SetUDPAddr(std::string addr) {
    this->udpAddr.SetText(addr);
}

/**
 * Returns the text of the "UDP Address" field.
 */
std::string OverviewPanel::GetUDPAddr() {
    return this->udpAddr.GetText();
}

/**
 * Sets the value of the "UDP Port" field.
 */
void OverviewPanel::SetUDPPort(int port) {
    this->udpPort.SetValue(port);
}

/**
 * Returns the value of the "UDP Port" field.
 */
int OverviewPanel::GetUDPPort() {
    return (int) this->udpPort.GetValue();
}

/**
 * Sets the "Enable/Disable" text of the UDP enable button.
 */
void OverviewPanel::SetUDPEnabledLabels(bool UDPEnabled) {
    this->enableUDP.SetText((UDPEnabled ? "Disable" : "Enable"));
}

/**
 * Calls SetTargetInformationLabels() with the information from the passed string
 * @param iterOutput A Runner output string (returned by Runner::Iterate()).
 */
void OverviewPanel::SetTargetInformationLabelsFromString(std::string iterOutput) {
    if(iterOutput.length() < 3) { //improperly formatted string
        return;
    }

    std::string trimmedOutput = iterOutput.substr(1, iterOutput.length() - 2); //sub off the ':' and ';' at beginning and end
    std::vector<std::string> splitOutput = Util::SplitString(trimmedOutput, ',');

    //there must be 5 nums in string, no more, no less
    if(splitOutput.size() == 8) {
        int targetX = std::stoi(splitOutput[1]);
        int targetY = std::stoi(splitOutput[2]);
        int targetWidth = std::stoi(splitOutput[3]);
        int targetHeight = std::stoi(splitOutput[4]);
        double targetDist = std::stod(splitOutput[5]);
        double targetAngleHorizontal = std::stod(splitOutput[6]);
        double targetAngleVertical = std::stod(splitOutput[7]);
        bool targetSpotted = (targetX > -1);

        SetTargetInformationLabels(targetSpotted, targetX, targetY, targetWidth, targetHeight, targetDist, targetAngleHorizontal, targetAngleVertical);
    } else {
        std::cout << "WARNING: Could not update overview panel information labels. Input string was wrongly formatted: " << iterOutput << std::endl;
    }
}

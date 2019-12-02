#include "../KiwiLight.h"

/**
 * Source file for the RunnerEditor class.
 * Written By: Brach Knutson
 */

using namespace KiwiLight;

static void ReconnectUDP() {
    KiwiLightApp::EditorReconnectUDP();
}

RunnerEditor::RunnerEditor(Runner runner) {
    Panel editor = Panel(false, 0);
        // OFFSET_X,
        // OFFSET_Y,
        // IMAGE_WIDTH,
        // IMAGE_HEIGHT,
        // TRUE_WIDTH,
        // PERCEIVED_WIDTH,
        // CALIBRATED_DISTANCE,
        // ERROR_CORRECTION

        Panel udpPanel = Panel(true, 0);
            Panel udpInputPanel = Panel(false, 0);
                Panel udpAddressPanel = Panel(true, 0);
                    Label udpAddressPanelHeader = Label("IPv4 Address: ");
                        udpAddressPanel.Pack_start(udpAddressPanelHeader.GetWidget(), true, true, 0);

                    std::string realUDPAddress = runner.GetUDP().GetAddress();
                    this->udpAddress = TextBox(realUDPAddress);
                        udpAddressPanel.Pack_start(this->udpAddress.GetWidget(), true, true, 0);

                    udpInputPanel.Pack_start(udpAddressPanel.GetWidget(), true, true, 0);

                Panel udpPortPanel = Panel(true, 0);
                    Label udpPortPanelHeader = Label("Port: ");
                        udpPortPanel.Pack_start(udpPortPanelHeader.GetWidget(), true, true, 0);

                    int realUDPPort = runner.GetUDP().GetPort();
                    this->udpPort = NumberBox(0.0, 9999.0, 1.0, (double) realUDPPort);
                        udpPortPanel.Pack_start(this->udpPort.GetWidget(), true, true, 0);
                    udpInputPanel.Pack_start(udpPortPanel.GetWidget(), true, true, 0);
                udpPanel.Pack_start(udpInputPanel.GetWidget(), true, true, 0);
            Button reconnectUDP = Button("Reconnect", ReconnectUDP);
                udpPanel.Pack_start(reconnectUDP.GetWidget(), true, true, 0);

            this->enableUDP = Button("Enable", KiwiLightApp::ToggleUDP);
                udpPanel.Pack_start(this->enableUDP.GetWidget(), true, true, 0);

            editor.Pack_start(udpPanel.GetWidget(), true, true, 0);
                    

        Panel offsetPanel = Panel(false, 0);
            Label offsetPanelHeader = Label("Camera offset (inches)");
                offsetPanelHeader.SetName("subHeader");
                offsetPanel.Pack_start(offsetPanelHeader.GetWidget(), true, true, 0);

            Panel offsetPanelContents = Panel(true, 0);
                double realOffsetX = runner.GetRunnerProperty(RunnerProperty::OFFSET_X);
                this->offsetX = LabeledSlider("Horizontal", -48.0, 48.0, -1.0, realOffsetX);
                    offsetPanelContents.Pack_start(this->offsetX.GetWidget(), true, true, 0);

                double realOffsetY = runner.GetRunnerProperty(RunnerProperty::OFFSET_Y);
                this->offsetY = LabeledSlider("Vertical", -48.0, 48.0, 0.1, realOffsetY);
                    offsetPanelContents.Pack_start(this->offsetY.GetWidget(), true, true, 0);

                offsetPanel.Pack_start(offsetPanelContents.GetWidget(), true, true, 0);
            editor.Pack_start(offsetPanel.GetWidget(), true, true, 0);

        Panel resizePanel = Panel(false, 0);
            Label resizePanelHeader = Label("Image Resize (pixels)");
                resizePanelHeader.SetName("subHeader");
                resizePanel.Pack_start(resizePanelHeader.GetWidget(), true, true, 0);

            Panel resizePanelContents = Panel(true, 0);
                double realResizeX = runner.GetRunnerProperty(RunnerProperty::IMAGE_WIDTH);
                this->imageWidth = LabeledSlider("Horizontal", 50.0, 1080.0, 1.0, realResizeX);
                    resizePanelContents.Pack_start(this->imageWidth.GetWidget(), true, true, 0);

                double realResizeY = runner.GetRunnerProperty(RunnerProperty::IMAGE_HEIGHT);
                this->imageHeight = LabeledSlider("Vertical", 50.0, 720.0, 1.0, realResizeY);
                    resizePanelContents.Pack_start(this->imageHeight.GetWidget(), true, true, 0);

                resizePanel.Pack_start(resizePanelContents.GetWidget(), true, true, 0);
            editor.Pack_start(resizePanel.GetWidget(), true, true, 0);

        Panel distancePanel = Panel(false, 0);
            Label distancePanelHeader = Label("Distance Constants");
                distancePanelHeader.SetName("subHeader");
                distancePanel.Pack_start(distancePanelHeader.GetWidget(), true, true, 0);

            this->distanceLabel = Label("Calculated Distance: (no target!)");
                this->distanceLabel.SetName("gray");
                distancePanel.Pack_start(this->distanceLabel.GetWidget(), true, true, 0);
            
            Panel imageConstantsPanel = Panel(true, 0);
                double realTargetWidth = runner.GetRunnerProperty(RunnerProperty::TRUE_WIDTH);
                this->targetTrueWidth = LabeledSlider("Target Width (inches)", 0.0, 120.0, 0.1, realTargetWidth);
                    imageConstantsPanel.Pack_start(this->targetTrueWidth.GetWidget(), true, true, 0);
                
                double realFocalWidth = runner.GetRunnerProperty(RunnerProperty::PERCEIVED_WIDTH);
                this->targetPercievedWidth = LabeledSlider("Focal Width", 0.0, 1000.0, 0.1, realFocalWidth);
                    imageConstantsPanel.Pack_start(this->targetPercievedWidth.GetWidget(), true, true, 0);

                distancePanel.Pack_start(imageConstantsPanel.GetWidget(), true, true, 0);

            Panel errorCorrectionPanel = Panel(true, 0);
                double realCalibratedDistance = runner.GetRunnerProperty(RunnerProperty::CALIBRATED_DISTANCE);
                this->targetCalibratedDistance = LabeledSlider("Calibrated Distance (inches)", 6.0, 240.0, 0.1, realCalibratedDistance);
                    errorCorrectionPanel.Pack_start(this->targetCalibratedDistance.GetWidget(), true, true, 0);

                double realErrorCorrection = runner.GetRunnerProperty(RunnerProperty::ERROR_CORRECTION);
                this->targetErrorCorrection = LabeledSlider("Error Correction", -3.0, 3.0, 0.01, realErrorCorrection);
                    errorCorrectionPanel.Pack_start(this->targetErrorCorrection.GetWidget(), true, true, 0);

                distancePanel.Pack_start(errorCorrectionPanel.GetWidget(), true, true, 0);
            editor.Pack_start(distancePanel.GetWidget(), true, true, 0);

    this->runnereditor = editor.GetWidget();
}

/**
 * Updates the editor and performs important stuff
 */
void RunnerEditor::Update(int targetDistance) {
    std::string distanceValueString = (targetDistance < 0 ? "(no target!)" : std::to_string(targetDistance));
    this->distanceLabel.SetText("Target Distance: " + distanceValueString);
}

/**
 * Returns the value of the property associated with prop.
 */
double RunnerEditor::GetProperty(RunnerProperty prop) {
    switch(prop) {
        case RunnerProperty::OFFSET_X:
            return this->offsetX.GetValue();
        case RunnerProperty::OFFSET_Y:
            return this->offsetY.GetValue();
        case RunnerProperty::IMAGE_WIDTH:
            return this->imageWidth.GetValue();
        case RunnerProperty::IMAGE_HEIGHT:
            return this->imageHeight.GetValue();
        case RunnerProperty::TRUE_WIDTH:
            return this->targetTrueWidth.GetValue();
        case RunnerProperty::PERCEIVED_WIDTH:
            return this->targetPercievedWidth.GetValue();
        case RunnerProperty::CALIBRATED_DISTANCE:
            return this->targetCalibratedDistance.GetValue();
        case RunnerProperty::ERROR_CORRECTION:
            return this->targetErrorCorrection.GetValue();
    }
}

/**
 * Sets the value of prop to value.
 */
void RunnerEditor::SetProperty(RunnerProperty prop, double value) {
    switch(prop) {
        case RunnerProperty::OFFSET_X:
            this->offsetX.SetValue(value);
            break;
        case RunnerProperty::OFFSET_Y:
            this->offsetY.SetValue(value);
            break;
        case RunnerProperty::IMAGE_WIDTH:
            this->imageWidth.SetValue(value);
            break;
        case RunnerProperty::IMAGE_HEIGHT:
            this->imageHeight.SetValue(value);
            break;
        case RunnerProperty::TRUE_WIDTH:
            this->targetTrueWidth.SetValue(value);
            break;
        case RunnerProperty::PERCEIVED_WIDTH:
            this->targetPercievedWidth.SetValue(value);
            break;
        case RunnerProperty::CALIBRATED_DISTANCE:
            this->targetCalibratedDistance.SetValue(value);
            break;
        case RunnerProperty::ERROR_CORRECTION:
            this->targetErrorCorrection.SetValue(value);
            break;
    }
}


std::string RunnerEditor::GetUDPAddr() {
    return this->udpAddress.GetText();
}


int RunnerEditor::GetUDPPort() {
    return (int) this->udpPort.GetValue();
}


void RunnerEditor::SetUDPAddr(std::string newAddr) {
    this->udpAddress.SetText(newAddr);
}


void RunnerEditor::SetUDPPort(int newPort) {
    this->udpPort.SetValue((double) newPort);
}


void RunnerEditor::SetUDPEnabledLabels(bool UDPEnabled) {
    this->enableUDP.SetText((UDPEnabled ? "Disable" : "Enable"));
}

void RunnerEditor::SetName(std::string name) {
    gtk_widget_set_name(this->runnereditor, name.c_str());
}

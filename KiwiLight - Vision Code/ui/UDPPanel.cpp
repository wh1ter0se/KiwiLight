#include "../KiwiLight.h"

/**
 * Source file for the UDPPanel class.
 * Written By: Brach Knutson
 */

using namespace KiwiLight;

/**
 * Creates a new UDPPanel, defaulting the "enabled" state to the value of "enabled"
 */
UDPPanel::UDPPanel(bool enabled) {
    Panel panel = Panel(true, 0);
        Label senderHeader = Label("UDP Sender:");
            senderHeader.SetName("subHeader");
            panel.Pack_start(senderHeader.GetWidget(), false, false, 5);

        this->readouts = Panel(false, 0);
            panel.Pack_start(readouts.GetWidget(), false, true, 0);

        this->connected = Label("NO");
        createHorizontalReadout("Connected: ", connected);

        this->address = Label("(none)");
        createHorizontalReadout("Address: ", address);

        this->port = Label("(none)");
        createHorizontalReadout("Port: ", port);
        
        this->toggleButton = Button("", KiwiLightApp::ToggleUDP);
            SetEnabled(enabled);
            panel.Pack_start(toggleButton.GetWidget(), false, false, 5);
    this->widget = panel.GetWidget();
}

/**
 * Sets the address field of the UDPPanel to "address"
 */
void UDPPanel::SetAddress(std::string address) {
    this->address.SetText(address);
}

/**
 * Sets the port field of the UDPPanel to "port"
 */
void UDPPanel::SetPort(int port) {
    this->port.SetText(std::to_string(port));
}

/**
 * Sets the text of the enable button to the opposite of "enabled."
 * For example, if "enabled" is true, button will be set to "disable," and vice versa.
 */
void UDPPanel::SetEnabled(bool enabled) {
    this->toggleButton.SetText(enabled ? "Disable" : "Enable");
}

/**
 * Sets the text of the connected field to the value of "connected."
 */
void UDPPanel::SetConnected(bool connected) {
    this->connected.SetText((connected ? "YES" : "NO"));
}

/**
 * Reads the information directly from the KiwiLight UDP sender and applies to the panel.
 */
void UDPPanel::ReadAndSetInfo() {
    SetAddress(KiwiLightApp::GetUDP().GetAddress());
    SetPort(KiwiLightApp::GetUDP().GetPort());
    SetEnabled(KiwiLightApp::GetUDPEnabled());
    SetConnected(KiwiLightApp::GetUDP().Connected());
}


void UDPPanel::createHorizontalReadout(std::string header, Label readout) {
    Panel readoutPanel = Panel(true, 0);
        Label headerLabel = Label(header);
            headerLabel.SetName("gray");
            readoutPanel.Pack_start(headerLabel.GetWidget(), false, true, 0);

            readoutPanel.Pack_start(readout.GetWidget(), false, true, 0);
        readouts.Pack_start(readoutPanel.GetWidget(), true, true, 0);
}

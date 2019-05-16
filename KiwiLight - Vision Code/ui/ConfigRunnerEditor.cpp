#include "UI.h"

/**
 * Source file for the ConfigRunnerEditor class.
 * Written By: Brach Knutson
 */

using namespace cv;
using namespace KiwiLight;


ConfigRunnerEditor::ConfigRunnerEditor(std::string fileName) {

    //define the xml file we will be parsing, and get the postprocessor tag to pull information from it
    XMLDocument document = XMLDocument(fileName);
    XMLTag postprocessor = document.GetTagsByName("configuration")[0].GetTagsByName("postprocessor")[0];

    this->panel = Panel(false, 3);

        XMLTag udpTag = postprocessor.GetTagsByName("UDP")[0];

        Label UDPHeader = Label("UDP Destination:");
            UDPHeader.SetName("subHeader");
            this->panel.Pack_start(UDPHeader.GetWidget(), false, false, 0);

        Panel UDPAddrPanel = Panel(true, 0);
            Label UDPAddrPanelHeader = Label("UDP Address(IPv4): ");
                UDPAddrPanel.Pack_start(UDPAddrPanelHeader.GetWidget(), false, false, 0);

            this->UDPAddr = TextBox("127.0.0.1");
                std::string realAddr = udpTag.GetTagsByName("address")[0].Content();
                UDPAddr.SetText(realAddr);
                UDPAddrPanel.Pack_start(this->UDPAddr.GetWidget(), true, true, 0);

            this->panel.Pack_start(UDPAddrPanel.GetWidget(), true, true, 0);

        Panel UDPPortPanel = Panel(true, 0);
            Label UDPPortPanelHeader = Label("UDP Port: ");
                UDPPortPanel.Pack_start(UDPPortPanelHeader.GetWidget(), false, false, 0);

            this->UDPPort = TextBox("3695");
                std::string realPort = udpTag.GetTagsByName("port")[0].Content();
                UDPPort.SetText(realPort);
                UDPPortPanel.Pack_start(this->UDPPort.GetWidget(), true, true, 0);

            this->panel.Pack_start(UDPPortPanel.GetWidget(), true, true, 0);

        Separator sep = Separator(true);
            this->panel.Pack_start(sep.GetWidget(), false, false, 0);

        Label distanceHeader = Label("Target Distance Calculation");
            distanceHeader.SetName("subHeader");
            this->panel.Pack_start(distanceHeader.GetWidget(), false, false, 0);
        
        this->distToTarget = Label("Distance to Target(inches): 0");
            this->distToTarget.SetName("gray");
            this->panel.Pack_start(this->distToTarget.GetWidget(), false, false, 0);

        //find the target with id "0"
        XMLTag firstTarget;
        std::vector<XMLTag> allTargets = postprocessor.GetTagsByName("target");
        for(int i=0; i<allTargets.size(); i++) {
            if(allTargets[i].GetAttributesByName("id")[0].Value() == "0") {
                firstTarget = allTargets[i];
                break;
            }
        }

        Panel widthPanel = Panel(true, 0);
                this->TargetWidth = LabeledSlider("True Width(inches)", 0.5, 120.0, 0.1, 5.0);
                    double realWidth = std::stod(firstTarget.GetTagsByName("knownWidth")[0].Content());
                    this->TargetWidth.SetValue(realWidth);
                    widthPanel.Pack_start(this->TargetWidth.GetWidget(), true, true, 0);

                this->TargetFocalWidth = LabeledSlider("Perceived Width", 0, 2000, 1, 0);
                    double realFocalWidth = std::stod(firstTarget.GetTagsByName("focalWidth")[0].Content());
                    this->TargetFocalWidth.SetValue(realFocalWidth);
                    widthPanel.Pack_start(this->TargetFocalWidth.GetWidget(), true, true, 0);

                this->panel.Pack_start(widthPanel.GetWidget(), false, false, 0);

        Panel errorCorrectPanel = Panel(true, 0);
            this->TargetCalibratedDistance = LabeledSlider("Calibrated Distance(inches)", 12.0, 240.0, 0.5, 24.0);
                double realCalibratedDistance = std::stod(firstTarget.GetTagsByName("calibratedDistance")[0].Content());
                this->TargetCalibratedDistance.SetValue(realCalibratedDistance);
                errorCorrectPanel.Pack_start(this->TargetCalibratedDistance.GetWidget(), true, true, 0);

            this->TargetDistErrCorrect = LabeledSlider("Distance Error Correction", -1.0, 1.0, 0.01, 0.0);
                double realErrCorrect = std::stod(firstTarget.GetTagsByName("distErrorCorrect")[0].Content());
                this->TargetDistErrCorrect.SetValue(realErrCorrect);
                errorCorrectPanel.Pack_start(this->TargetDistErrCorrect.GetWidget(), true, true, 0);

            this->panel.Pack_start(errorCorrectPanel.GetWidget(), true, true, 0);

    this->configrunnereditor = panel.GetWidget();
}


void ConfigRunnerEditor::Update() {

}


ConfigurationSettingsList ConfigRunnerEditor::GetSettings() {

}


void ConfigRunnerEditor::SetName(std::string name) {
    gtk_widget_set_name(this->configrunnereditor, name.c_str());
}
#include "UI.h"

/**
 * Source file for the ConfigRunnerEditor class.
 * Written By: Brach Knutson
 */

using namespace cv;
using namespace KiwiLight;


static void tuneDistancePressed() {
    std::cout << "tune distance" << std::endl;
}


static void SaveAndExit() {
    Flags::RaiseFlag("SaveAndCloseEditor");
}


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

        Label imageResizeHeader = Label("Image Resize");
            imageResizeHeader.SetName("subHeader");
            this->panel.Pack_start(imageResizeHeader.GetWidget(), true, true, 0);

        Panel imageResizePanel = Panel(true, 0);
            this->imageResizeX = LabeledSlider("Image Width", 25, 800, 1, 200);
                imageResizePanel.Pack_start(this->imageResizeX.GetWidget(), true, true, 0);

            this->imageResizeY = LabeledSlider("Image Height", 25, 800, 1, 145);
                imageResizePanel.Pack_start(this->imageResizeY.GetWidget(), true ,true, 0);

            this->panel.Pack_start(imageResizePanel.GetWidget(), true ,true, 0);

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

        this->tuneDistance = Button("Tune Distance", tuneDistancePressed);
            this->panel.Pack_start(this->tuneDistance.GetWidget(), false ,false, 0);

        Label imageHeader = Label("Image Output");
                this->panel.Pack_start(imageHeader.GetWidget(), false, false, 0);

        //panel where the output images will show up
        Panel imagePanel = Panel(true, 0);

            this->imageTooBig = Label("");
                this->imageTooBig.SetLineWrap(true);
                imagePanel.Pack_start(this->imageTooBig.GetWidget(), false, false, 0);

            Image blank = Image(); // blank image for outputImages
            this->outputImages = ImageFrame(blank);
                imagePanel.Pack_start(this->outputImages.GetWidget(), false, false, 0);

            this->panel.Pack_start(imagePanel.GetWidget(), false, false, 0);  

        this->saveAndExit = Button("Save and Exit", SaveAndExit);
            this->panel.Pack_start(this->saveAndExit.GetWidget(), false, false, 0);          

    this->configrunnereditor = panel.GetWidget();
}

/**
 * Updates the sliders and output images. Runner is needed for output images
 * @param originalImage an image that will be displayed to represent the original image
 * @param processedImage an output image from the runner to be displayed
 * @param targetWidth the width of the seen target in pixels, or anything less than 0 if none found.
 */
void ConfigRunnerEditor::Update(cv::Mat originalImage, cv::Mat processedImage, double targetDistance) {
    //concat the original image and processed image together and display them

    if(this->imageResizeX.GetValue() < 250 && this->imageResizeY.GetValue() < 225) {
        //when displaying as separate images, sometimes processed will render as original, so we do it this way
        try {
            cv::Mat concatImages;
            cv::hconcat(processedImage, originalImage, concatImages);

            Image newImage = Image(concatImages);
            this->outputImages.Update(newImage);
            this->imageTooBig.SetText("");
        } catch(cv::Exception ex) {}
    } else {
        Image blank = Image();
        this->outputImages.Update(blank);
        this->imageTooBig.SetText("Image too big to display comfortably");
    }

    //update distance information
    this->distToTarget.SetText("Distance to Target(inches): " + std::to_string(targetDistance));
}


std::string ConfigRunnerEditor::GetUDPAddress() {
    return this->UDPAddr.GetText();
}

/**
 * Return the address of the UDP port as entered by the user, or -1 if the input is invalid.
 */
int ConfigRunnerEditor::GetUDPPort() {
    try {
        int port = std::stoi(this->UDPPort.GetText());
        return port;
    } catch(std::invalid_argument ex) {
        return -1;
    }
}

/**
 * Returns the value of the property described by prop.
 */
double ConfigRunnerEditor::GetProperty(RunnerProperty prop) {
    double finalValue = -1.0;

    switch(prop) {
        case RunnerProperty::IMAGE_WIDTH:
            finalValue = this->imageResizeX.GetValue();
            break;
        case RunnerProperty::IMAGE_HEIGHT:
            finalValue = this->imageResizeY.GetValue();
            break;
        case RunnerProperty::TRUE_WIDTH:
            finalValue = this->TargetWidth.GetValue();
            break;
        case RunnerProperty::PERCEIVED_WIDTH:
            finalValue = this->TargetFocalWidth.GetValue();
            break;
        case RunnerProperty::CALIBRATED_DISTANCE:
            finalValue = this->TargetCalibratedDistance.GetValue();
            break;
        case RunnerProperty::ERROR_CORRECTION:
            finalValue = this->TargetDistErrCorrect.GetValue();
            break;
    }

    return finalValue;
}


ConfigurationSettingsList ConfigRunnerEditor::GetSettings() {

}


void ConfigRunnerEditor::SetName(std::string name) {
    gtk_widget_set_name(this->configrunnereditor, name.c_str());
}
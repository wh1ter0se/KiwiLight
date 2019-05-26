#include "UI.h"

/**
 * Source file for the ConfigRunnerEditor class.
 * Written By: Brach Knutson
 */

using namespace cv;
using namespace KiwiLight;


static void tuneDistancePressed() {
    std::cout << "tune distance" << std::endl;
    std::cout.flush();
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

        //panel where the output images will show up
        Panel imagePanel = Panel(true, 0);

            Panel processedImagePanel = Panel(false, 3);
                Label processedImageHeader = Label("Processed Image");
                    processedImagePanel.Pack_start(processedImageHeader.GetWidget(), false, false, 0);

                //default blank image for this one too
                Image processedImageImage = Image();
                
                this->processedImage = ImageFrame(processedImageImage);
                    processedImagePanel.Pack_start(this->processedImage.GetWidget(), false, false, 0);

                imagePanel.Pack_start(processedImagePanel.GetWidget(), false, false, 0);

            this->panel.Pack_start(imagePanel.GetWidget(), false, false, 0);


            //panel with image for original image
            Panel originalImagePanel = Panel(false, 3);
                Label originalImageHeader = Label("Original Image");
                    originalImagePanel.Pack_start(originalImageHeader.GetWidget(), false, false, 0);

                //make default blank image for the original image frame
                Image originalImageImage = Image();
 
                this->originalImage = ImageFrame(originalImageImage);
                    originalImagePanel.Pack_start(this->originalImage.GetWidget(), false, false, 0);

                imagePanel.Pack_start(originalImagePanel.GetWidget(), false, false, 0);

            //panel with image for processed image
            

    this->configrunnereditor = panel.GetWidget();
}

/**
 * Updates the sliders and output images. Runner is needed for output images
 */
void ConfigRunnerEditor::Update(cv::Mat originalImage, cv::Mat processedImage) {


    //update processed (also in RGB)
    try {
        Mat processedRGB;
        processedImage.convertTo(processedRGB, CV_16U);
        cvtColor(processedRGB, processedRGB, COLOR_BGR2RGB);
        processedRGB.convertTo(processedRGB, CV_8U);
        Image processedImageImage = Image(processedRGB);
        this->processedImage.Update(processedImageImage);
    } catch(cv::Exception ex) {}

    //update original (in RGB colorspace of course)
    try {
        Mat originalRGB;
        originalImage.convertTo(originalRGB, CV_16U);
        cvtColor(originalImage, originalRGB, COLOR_BGR2RGB);
        originalRGB.convertTo(originalRGB, CV_8U);
        Image originalImageImage = Image(originalRGB);
        this->originalImage.Update(originalImageImage);
    } catch(cv::Exception ex) {}

    
}

/**
 * Returns the value of the property described by prop.
 * Returns -1.0 if no such property exists
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
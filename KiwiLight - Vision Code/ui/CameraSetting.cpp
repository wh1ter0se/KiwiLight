#include "UI.h"

/**
 * Source file for the CameraSetting class.
 * Written By: Brach Knutson
 */


using namespace KiwiLight;

/**
 * Creates a new CameraSetting widget, using the given std::string from V4L
 */
CameraSetting::CameraSetting(std::string name, int min, int max, int value) {
    this->min = min;
    this->max = max;
    this->value = value;

    //find true name and type of the setting
    int index = name.find_first_not_of(' ');
    int endIndex = index;
    while(endIndex < name.length()) {
        if(name.at(endIndex) == ' ') {
            break;
        }
        endIndex++;
    }

    //do substd::string manually because the one std provides wasn't working correctly
    for(int i=index; i<endIndex; i++) {
        this->name += name.at(i);
    }

    //find the true type. It is assumed that the input std::string has one set of () because v4l
    int firstParen = name.find('(');
    int lastParen  = name.find(')');
    for(int k=firstParen; k <= lastParen; k++) {
        this->type += name.at(k);
    }

    //create the widget
    std::string boundString = "Min: " + std::to_string(min) + 
                         "  Max: " + std::to_string(max);

    std::string nameString = this->name + " (" + boundString + ")";

    Panel main = Panel(false, 0);
        Panel nameAndInput = Panel(true, 5);
            this->nameLabel = Label(nameString);
                nameAndInput.Pack_start(nameLabel.GetWidget(), true, false, 0);

            AlignmentContainer align = AlignmentContainer(1, 0);
                this->input = TextBox(std::to_string(this->value));
                    this->input.SetAlignment(1);
                    align.Pack(this->input.GetWidget());

                nameAndInput.Pack_start(align.GetWidget(), true ,true, 0);

            main.Pack_start(nameAndInput.GetWidget(), true, false, 0);

    this->camerasetting = main.GetWidget();
}

/**
 * Gets and returns the value of the widget.
 */
int CameraSetting::GetValue() {
    std::string val = this->input.GetText();

    //convert to int and return
    try {
        return std::stoi(val);
    } catch(...) {
        std::cout << "A A A A A A A A A" << std::endl;
        std::cout.flush();
        return this->value;
    }
}

/**
 * Sets the value of the setting to newValue.
 */
void CameraSetting::SetValue(int newValue) {
    this->value = newValue;
    this->input.SetText(std::to_string(newValue));
}


#include "UI.h"

/**
 * Source file for the CameraSetting class.
 * Written By: Brach Knutson
 */

using namespace std;
using namespace KiwiLight;

/**
 * Creates a new CameraSetting widget, using the given string from V4L
 */
CameraSetting::CameraSetting(string name, int min, int max, int value) {
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

    //do substring manually because the one std provides wasn't working correctly
    for(int i=index; i<endIndex; i++) {
        this->name += name.at(i);
    }

    //find the true type. It is assumed that the input string has one set of () because v4l
    int firstParen = name.find('(');
    int lastParen  = name.find(')');
    for(int k=firstParen; k <= lastParen; k++) {
        this->type += name.at(k);
    }

    //create the widget
    string boundString = "Min: " + std::to_string(min) + 
                         "  Max: " + std::to_string(max);

    string nameString = this->name + " (" + boundString + ")";

    Panel main = Panel(false, 0);
        Panel nameAndInput = Panel(true, 5);
            Label nameLabel = Label(nameString);
                nameAndInput.Pack_start(nameLabel, true, false, 0);

            AlignmentContainer align = AlignmentContainer(1, 0);
                this->input = TextBox(std::to_string(this->value));
                    this->input.SetAlignment(1);
                    align.Pack(this->input);

                nameAndInput.Pack_start(align, true ,true, 0);

            main.Pack_start(nameAndInput, true, false, 0);

    this->camerasetting = main.GetWidget();
}

/**
 * Gets and returns the value of the widget.
 */
int CameraSetting::GetValue() {

}
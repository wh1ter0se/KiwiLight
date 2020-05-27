#include "UI.h"

/**
 * Source file for the AboutWindow class.
 * Written By: Brach Knutson
 */

using namespace KiwiLight;

/**
 * Creates a new AboutWindow with the given window type.
 */
AboutWindow::AboutWindow(GtkWindowType type) {

    //put together an about window
    this->window = Window(type, false);
    this->window.SetSize(550,200);

    Panel main = Panel(true, 0);
        Panel banners = Panel(false, 0);
            Image kiwiLightBanner = Image("banner_small.png");
                banners.Pack_start(kiwiLightBanner.GetWidget(), false, false, 0);

            Image foximusBanner = Image("foximus_small.png");
                banners.Pack_start(foximusBanner.GetWidget(), false, false, 0);

            main.Pack_start(banners.GetWidget(), false, false, 0);

        Panel descriptions = Panel(false, 0);
            Label header = Label("About KiwiLight");
                header.SetName("header");
                descriptions.Pack_start(header.GetWidget(), false, false, 0);

            Label description = Label(
                std::string("KiwiLight was written by Brach Knutson and Colton Kreischer ") +
                std::string("of FRC 3695  Foximus Prime for use by any FRC team ") +
                std::string("for any vision purpose.\n\nKiwiLight is stored at ") +
                std::string("github.com/wh1ter0se/KiwiLight. \n\nKiwiLight is published ") +
                std::string("under a GPL-3.0 license, permitting modification ") +
                std::string("and distribution under the condition that the source is ") +
                std::string("disclosed and distribution is accompanied by the same license.")
                );
                description.SetLineWrap(true);
                descriptions.Pack_start(description.GetWidget(), false, false, 0);

            main.Pack_start(descriptions.GetWidget(), false, false, 0);

        this->window.SetPane(main);

    this->widget = window.GetWidget();
}

/**
 * Shows the about window.
 */
void AboutWindow::Show() {
    this->window.Show();
}
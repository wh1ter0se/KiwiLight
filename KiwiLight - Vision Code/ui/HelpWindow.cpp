#include "UI.h"

/**
 * Source file for the HelpWindow class.
 * Written By: Brach Knutson
 */


using namespace KiwiLight;

/**
 * Creates a new HelpWindow with the given window type.
 */
HelpWindow::HelpWindow(GtkWindowType type) {
    this->window = Window(type, false);
    this->window.SetSize(550, 200);
        Panel main = Panel(true, 0);
            Panel banners = Panel(false, 0);

                Image kiwiLogo = Image("banner_small.png");
                    banners.Pack_start(kiwiLogo.GetWidget(), false, false, 0);

                Image foximusLogo = Image("foximus_small.png");
                    banners.Pack_start(foximusLogo.GetWidget(), false, false, 0);

                main.Pack_start(banners.GetWidget(), false, false, 0);

            Panel labels = Panel(false, 0);

                Label header = Label("KiwiLight Help");
                    header.SetName("header");
                    labels.Pack_start(header.GetWidget(), false, false, 0);

                Label content = Label(
                    std::string("For KiwiLight help, check out the tutorial at https://github.com/BTK203/KiwiLight/blob/master/Manual.pdf\n\n") +
                    std::string("You can also email brach2003@gmail.com with any questions, comments, concerns, etc.")
                );
                    content.SetLineWrap(true);
                    labels.Pack_start(content.GetWidget(), false, false, 0);

                main.Pack_start(labels.GetWidget(), false, false, 0);

            this->window.SetPane(main);

    this->widget = this->window.GetWidget();
}

/**
 * Shows the help window.
 */
void HelpWindow::Show() {
    this->window.Show();
}

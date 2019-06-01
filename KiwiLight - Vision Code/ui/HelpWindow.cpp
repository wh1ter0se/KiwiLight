#include "UI.h"

/**
 * Source file for the HelpWindow class.
 * Written By: Brach Knutson
 */


using namespace KiwiLight;


HelpWindow::HelpWindow(GtkWindowType type) {
    this->window = Window(type, false);
    this->window.SetSize(550, 200);
        Panel main = Panel(true, 0);
            Panel banners = Panel(false, 0);

                Image kiwiLogo = Image("banner_small.png");
                ImageFrame kiwiImage = ImageFrame(kiwiLogo);
                    banners.Pack_start(kiwiImage.GetWidget(), false, false, 0);

                Image foximusLogo = Image("foximus_small.png");
                ImageFrame foximusImage = ImageFrame(foximusLogo);
                    banners.Pack_start(foximusImage.GetWidget(), false, false, 0);

                main.Pack_start(banners.GetWidget(), false, false, 0);

            Panel labels = Panel(false, 0);

                Label header = Label("KiwiLight Help");
                    header.SetName("header");
                    labels.Pack_start(header.GetWidget(), false, false, 0);

                Label content = Label(
                    std::string("For KiwiLight help, check out the tutorial at <insert link here>\n\n") +
                    std::string("You can also email brach2003@gmail.com with any questions, comments, concerns, etc.")
                );
                    content.SetLineWrap(true);
                    labels.Pack_start(content.GetWidget(), false, false, 0);

                main.Pack_start(labels.GetWidget(), false, false, 0);

            this->window.SetPane(main);

    this->helpwindow = this->window.GetWidget();
}


void HelpWindow::Show() {
    this->window.Show();
}


void HelpWindow::SetName(std::string name) {
    gtk_widget_set_name(this->helpwindow, name.c_str());
}
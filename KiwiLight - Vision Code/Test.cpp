#include "KiwiLight.h"

/**
 * TEST FILE
 */

using namespace cv;
using namespace KiwiLight;

void EditSelected() {}
void ToggleUDP() {}

void tab2ButtonPressed() {
    std::cout << "tab 2 button" << std::endl;
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);
    Window win = Window(GTK_WINDOW_TOPLEVEL);
        Panel content = Panel(false, 0);
            Panel originalTab = Panel(false, 0);
                Label header = Label("Tab 1");
                    header.SetName("header");
                    originalTab.Pack_start(header.GetWidget(), false, false, 0);

            Panel tab2 = Panel(false, 0);
                Label tab2Header = Label("Tab 2");
                    tab2Header.SetName("header");
                    tab2.Pack_start(tab2Header.GetWidget(), false, false, 0);
                
                Button tab2Button = Button("Button", tab2ButtonPressed);
                    tab2.Pack_start(tab2Button.GetWidget(), false, false, 0);

            Panel tab3 = Panel(false, 0);
                Label tab3Header = Label("tab 3");
                    tab3Header.SetName("header");
                    tab3.Pack_start(tab3Header.GetWidget(), false, false, 0);

                Image img = Image("foximus_small.png");
                    tab3.Pack_start(img.GetWidget(), false, false, 0);

            Panel tab4 = Panel(false, 0);
                Label tab4Header = Label("Tab 4");
                    tab4Header.SetName("header");
                    tab4.Pack_start(tab4Header.GetWidget(), false, false, 0);

                Image tab4Image = Image("banner_small.png");
                    tab4.Pack_start(tab4Image.GetWidget(), false, false, 0);

            TabView tabs = TabView("tab 1", originalTab.GetWidget());
                tabs.AddTab("Tab 2", tab2.GetWidget());
                tabs.AddTab("Tab 3", tab3.GetWidget());
                tabs.AddTab("Tab 4", tab4.GetWidget());
                content.Pack_start(tabs.GetWidget(), false, false, 0);

            win.SetPane(content);

    win.SetCSS("ui/Style.css");
    win.Show();
    win.Main();


    return 0;
}
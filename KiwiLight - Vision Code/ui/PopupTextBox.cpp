#include "UI.h"

/**
 * Source file for the PopupTextBox class.
 * Written By: Brach Knutson
 */

using namespace KiwiLight;


PopupTextBox::PopupTextBox(std::string name, std::string prompt, std::string initValue) {
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    this->popuptextbox = gtk_dialog_new_with_buttons(name.c_str(), GTK_WINDOW(window), GTK_DIALOG_MODAL, "OK", GTK_RESPONSE_ACCEPT, NULL);
    GtkWidget *dialogContent = gtk_dialog_get_content_area(GTK_DIALOG(this->popuptextbox));
        Panel pnl = Panel(false, 0);
            Label promptLabel = Label(prompt);
                pnl.Pack_start(promptLabel.GetWidget(), false, false, 0);

            this->textbox = TextBox(initValue);
                pnl.Pack_start(this->textbox.GetWidget(), false, false, 0);

            gtk_container_add(GTK_CONTAINER(dialogContent), pnl.GetWidget());

}


std::string PopupTextBox::Show() {
    gtk_widget_show_all(this->popuptextbox);
    gtk_dialog_run(GTK_DIALOG(this->popuptextbox));
    std::string response = this->textbox.GetText();
    gtk_widget_destroy(this->popuptextbox);
    return response;
}


void PopupTextBox::SetName(std::string name) {
    gtk_widget_set_name(this->popuptextbox, name.c_str());
}
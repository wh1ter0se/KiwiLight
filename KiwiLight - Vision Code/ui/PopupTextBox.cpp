#include "UI.h"

/**
 * Source file for the PopupTextBox class.
 * Written By: Brach Knutson
 */

using namespace KiwiLight;

/**
 * Creates a new PopupTextBox.
 * @param name The name of the dialog.
 * @param prompt The text to prompt the user with.
 * @param initValue The inital value of the text box.
 * DEPRECATED: The PopupTextBox class is no longer used and will be removed in the next update.
 */
PopupTextBox::PopupTextBox(std::string name, std::string prompt, std::string initValue) {
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    this->widget = gtk_dialog_new_with_buttons(name.c_str(), GTK_WINDOW(window), GTK_DIALOG_MODAL, "OK", GTK_RESPONSE_ACCEPT, NULL);
    GtkWidget *dialogContent = gtk_dialog_get_content_area(GTK_DIALOG(this->widget));
        Panel pnl = Panel(false, 0);
            Label promptLabel = Label(prompt);
                pnl.Pack_start(promptLabel.GetWidget(), false, false, 0);

            this->textbox = TextBox(initValue);
                pnl.Pack_start(this->textbox.GetWidget(), false, false, 0);

            gtk_container_add(GTK_CONTAINER(dialogContent), pnl.GetWidget());

}

/**
 * Shows the dialog and returns the user's response.
 * DEPRECATED: The PopupTextBox class will be removed in the next update.
 */
std::string PopupTextBox::Show() {
    gtk_widget_show_all(this->widget);
    gtk_dialog_run(GTK_DIALOG(this->widget));
    std::string response = this->textbox.GetText();
    gtk_widget_destroy(this->widget);
    return response;
}

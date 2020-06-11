#include "UI.h"

/**
 * Source file for the TabView class.
 * Written By: Brach Knutson
 */

using namespace KiwiLight;

/**
 * Creates a new TabView with a pre-loaded tab.
 * @param tab1Name The name of the first tab.
 * @param tab1Content The tab 1 widget.
 */
TabView::TabView(std::string tab1Name, GtkWidget *tab1Content) {
    this->widget = gtk_notebook_new();
    Label tabLabel = Label(tab1Name);
    gtk_notebook_append_page(GTK_NOTEBOOK(this->widget), tab1Content, tabLabel.GetWidget());
}

/**
 * Adds a tab to the TabView.
 * @param tabName The name of the new tab.
 * @param tabContent The widget of the new tab.
 */
void TabView::AddTab(std::string tabName, GtkWidget *tabContent) {
    Label tabLabel = Label(tabName);
    gtk_notebook_append_page(GTK_NOTEBOOK(this->widget), tabContent, tabLabel.GetWidget());
}

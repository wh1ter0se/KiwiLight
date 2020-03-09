#include "UI.h"

/**
 * Source file for the TabView class.
 * Written By: Brach Knutson
 */

using namespace KiwiLight;


TabView::TabView(std::string tab1Name, GtkWidget *tab1Content) {
    this->tabview = gtk_notebook_new();
    Label tabLabel = Label(tab1Name);
    gtk_notebook_append_page(GTK_NOTEBOOK(this->tabview), tab1Content, tabLabel.GetWidget());
}


void TabView::AddTab(std::string tabName, GtkWidget *tabContent) {
    Label tabLabel = Label(tabName);
    gtk_notebook_append_page(GTK_NOTEBOOK(this->tabview), tabContent, tabLabel.GetWidget());
}


void TabView::SetName(std::string name) {
    gtk_widget_set_name(this->tabview, name.c_str());
}
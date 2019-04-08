#include "UI.h"

/**
 * Source file for the AlignmentContainer class.
 * Written By: Brach Knutson
 */

using namespace std;
using namespace KiwiLight;

/**
 * Creates a new alignment container with the x and y values for alignment.
 * for the values, x=0 is all the way left, x=1 is all the way right, and same for y.
 */
AlignmentContainer::AlignmentContainer(int x, int y) {
    this->aligncontainer = gtk_alignment_new(gfloat(x), gfloat(y), 1.5f, 1.5f);
}


void AlignmentContainer::Pack(Widget &widget) {
    gtk_container_add(GTK_CONTAINER(this->aligncontainer), widget.GetWidget());
}
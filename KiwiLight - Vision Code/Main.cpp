#include "KiwiLight.h"

/**
 * Main entry point for KiwiLight program.
 * Written By: Brach Knutson
 */

using namespace std;
using namespace KiwiLight;

Kiwi kiwi;

/**
 * Updates the camera stream and allows the kiwi to update without static methods.
 */
static void Update() {
    kiwi.UpdateCamera();
    kiwi.UpdateSettings();
}


int main() {
    kiwi = Kiwi(Update);

    return 0;
}
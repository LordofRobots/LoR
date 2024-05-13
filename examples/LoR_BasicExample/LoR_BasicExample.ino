#include "LoR.h"

void setup() {
    Serial.begin(115200);
    while (!Serial) {
        ;  // Wait for serial port to connect. Needed for native USB port only
    }
    LoR.begin();  // Initialize the LoR library
}

void loop() {
    // Main loop is empty to allow for other tasks
}

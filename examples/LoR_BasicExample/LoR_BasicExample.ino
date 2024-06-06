/**
 * LoR_BasicExample.ino
 * 
 * Description:
 * This basic example demonstrates the initial setup and minimal configuration needed to use the LoR (Lord of Robots) library 
 * with an ESP32 controller. It is designed as a starting point for more complex projects, providing the foundation for 
 * initializing the library and setting up serial communication. The main loop is intentionally left empty to keep the system
 * idle or ready for further customization.
 * 
 * Key Features:
 * - Simple Initialization: Showcases the basic steps to get started with the LoR library.
 * - Serial Setup: Begins serial communication at 115200 baud for debugging and monitoring purposes.
 * - Scalable Framework: Serves as a template for further development and integration of additional functionalities.
 * 
 * Components:
 * - LoR.begin(): Initializes the LoR library, setting up the core functionalities.
 * 
 * Usage:
 * This program is a template for beginners or for projects that require a straightforward setup of the LoR library. It is 
 * structured to allow easy expansion and integration into more complex systems.
 * 
 * Setup:
 * - Ensure all necessary libraries are included and the hardware connections are correctly configured.
 * - For detailed library usage and additional functionality, refer to the LoR library documentation.
 * - Install ESP32 boards through board manager. Must include this link in file/preferences https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
 * - Install BluePad32 boards through board manager. Must include this link in file/preferences https://raw.githubusercontent.com/ricardoquesada/esp32-arduino-lib-builder/master/bluepad32_files/package_esp32_bluepad32_index.json
 * - The code is designed to be uploaded to an ESP32 controller.
 * - Target board: Choose the BluePad32 version of the "ESP32 dev module"
 *
 * Author:
 * Dave Barratt
 * 
 * Date:
 * JUNE 6 2024
 */

#include "LoR.h"

void setup() {
    Serial.begin(115200);
    LoR.begin();  // Initialize the LoR library
}

void loop() {
    // Main loop is empty to allow for other tasks
}

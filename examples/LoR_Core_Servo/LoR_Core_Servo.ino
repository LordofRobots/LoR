/**
 * LoR_Core_Servo.ino
 * 
 * Description:
 * This example demonstrates simple servo control using the LoR_Core library. It initializes the LoR library and provides
 * basic commands to manage servo positions through predefined sequences. The servos are programmed to cycle through 
 * positions at 0, 90, and 180 degrees to illustrate basic motion control capabilities within robotic applications.
 * This introductory code is beneficial for understanding servo operation and control using the LoR_Core library.
 * 
 * Key Features:
 * - Servo Initialization: Configures servo motors for operation using LoR's servo management functions.
 * - Position Sequencing: Demonstrates control precision by cycling servos through three predefined positions.
 * - Delay Management: Implements delays to hold each position momentarily, allowing clear observation of the movements.
 * 
 * Components:
 * - LoR.Servo(): Initializes the servo motor settings.
 * - LoR.Servo_SetPosition(): Adjusts the servo to specific angles as per the sequence requirements.
 * 
 * Usage:
 * This code is ideal for educational purposes and initial setup tests, showcasing basic servo control with the LoR_Core library.
 * It can be further developed or adapted for complex tasks in robotic systems requiring precise positional control.
 * 
 * Setup:
 * - Include all necessary libraries and verify hardware connections are correctly made.
 * - Refer to the LoR library documentation for detailed usage instructions and additional features.
 * - Install ESP32 boards via the board manager using this link in Arduino IDE file/preferences:
 *   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
 * - Install BluePad32 boards via the board manager using this link:
 *   https://raw.githubusercontent.com/ricardoquesada/esp32-arduino-lib-builder/master/bluepad32_files/package_esp32_bluepad32_index.json
 * - Code is designed for an ESP32 controller; select the BluePad32 version of the "ESP32 dev module" as the target board.
 *
 * Authors:
 * Dave Barratt
 * Khushi Tailor
 * 
 * Date:
 * JUNE 6, 2024
 */

#include "LoR.h"  // Include the LoR library to access servo control functionalities

void setup() {
  Serial.begin(115200);   // Initialize the Serial Port for debugging outputs
  LoR.begin();            // Initialize the LoR library to prepare for servo control
  LoR.INIT_Servo();       // Initialize the servo settings
}

void loop() {
  // Set all servos to 0 degrees
  LoR.Servo_SetPosition(Servo_CH[0], 0);
  LoR.Servo_SetPosition(Servo_CH[1], 0);
  LoR.Servo_SetPosition(Servo_CH[2], 0);
  LoR.Servo_SetPosition(Servo_CH[3], 0);
  delay(1000);  // Hold position for 1 second

  // Move all servos to 90 degrees
  LoR.Servo_SetPosition(Servo_CH[0], 90);
  LoR.Servo_SetPosition(Servo_CH[1], 90);
  LoR.Servo_SetPosition(Servo_CH[2], 90);
  LoR.Servo_SetPosition(Servo_CH[3], 90);
  delay(1000);  // Hold position for 1 second

  // Move all servos to 180 degrees
  LoR.Servo_SetPosition(Servo_CH[0], 180);
  LoR.Servo_SetPosition(Servo_CH[1], 180);
  LoR.Servo_SetPosition(Servo_CH[2], 180);
  LoR.Servo_SetPosition(Servo_CH[3], 180);
  delay(1000);  // Hold position for 1 second

  // Return all servos to 90 degrees
  LoR.Servo_SetPosition(Servo_CH[0], 90);
  LoR.Servo_SetPosition(Servo_CH[1], 90);
  LoR.Servo_SetPosition(Servo_CH[2], 90);
  LoR.Servo_SetPosition(Servo_CH[3], 90);
  delay(1000);  // Hold position for 1 second
}

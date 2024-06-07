/**
 * LoR_Core_Motor.ino
 * 
 * Description:
 * This example illustrates basic motor control functionalities using the LoR_Core module, demonstrating both 
 * forward and reverse motor operations through the Mx ports. The code provides fundamental insights into motor 
 * control mechanics, essential for robotic system integrations using the LoR_Core module. Motor power levels 
 * range from -512 to 512, where 512 represents maximum power and -512 represents maximum reverse power.
 * 
 * Key Features:
 * - Bidirectional Motor Control: Manages motor operation in both forward and reverse directions, allowing comprehensive control over motor movement.
 * - Power Level Management: Showcases the ability to adjust motor power from -512 to 512, offering full control over speed and torque.
 * - Sequential Motor Stopping and Starting: Demonstrates orderly starting and stopping of motors, important for task sequencing in robotic operations.
 * 
 * Components:
 * - LoR.Set_Motor_Output(): Controls the speed and direction of each motor connected to the Mx ports of the LoR_Core module.
 * 
 * Usage:
 * This script is designed as a diagnostic tool to verify motor connections and functionality on the LoR_Core platform. It lays the groundwork
 * for more complex motor control algorithms and serves educational purposes in demonstrating basic principles of motor control.
 * 
 * Setup:
 * - Ensure all necessary libraries are included and the hardware connections are correctly configured.
 * - For comprehensive library usage and extended functionalities, consult the LoR library documentation.
 * - Install ESP32 boards through the board manager. Include this link in Arduino IDE file/preferences:
 *   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
 * - Install BluePad32 boards via the board manager using this link in Arduino IDE file/preferences:
 *   https://raw.githubusercontent.com/ricardoquesada/esp32-arduino-lib-builder/master/bluepad32_files/package_esp32_bluepad32_index.json
 * - The code is intended for uploading to an ESP32 controller.
 * - Select "ESP32 dev module" (BluePad32 version) as the target board.
 *
 * Authors:
 * Dave Barratt
 * Khushi Tailor
 *
 * Date:
 * JUNE 6, 2024
 */

#include "LoR.h"  // Include the LoR library for accessing motor control functionalities

///////////////////////////////////////////////////////////////////////
//                          Main SETUP                               //
///////////////////////////////////////////////////////////////////////
void setup() {
  Serial.begin(115200); // Initialize the Serial Port for debugging outputs
  LoR.begin();          // Initialize the LoR library to prepare for motor control
}


///////////////////////////////////////////////////////////////////////
//                          Main Loop                                //
///////////////////////////////////////////////////////////////////////
void loop() {
  // Initially stop all motors to ensure a known state before changing speeds
  LoR.Set_Motor_Output(STOP, M1);
  LoR.Set_Motor_Output(STOP, M2);
  LoR.Set_Motor_Output(STOP, M3);
  LoR.Set_Motor_Output(STOP, M4);
  LoR.Set_Motor_Output(STOP, M5);
  LoR.Set_Motor_Output(STOP, M6);

  delay(1000);  // Wait for 1 second

  // Set all motors to a moderate forward speed
  LoR.Set_Motor_Output(300, M1);
  LoR.Set_Motor_Output(300, M2);
  LoR.Set_Motor_Output(300, M3);
  LoR.Set_Motor_Output(300, M4);
  LoR.Set_Motor_Output(300, M5);
  LoR.Set_Motor_Output(300, M6);

  delay(1000);  // Wait for 1 second

  // Stop all motors again
  LoR.Set_Motor_Output(STOP, M1);
  LoR.Set_Motor_Output(STOP, M2);
  LoR.Set_Motor_Output(STOP, M3);
  LoR.Set_Motor_Output(STOP, M4);
  LoR.Set_Motor_Output(STOP, M5);
  LoR.Set_Motor_Output(STOP, M6);

  delay(1000);  // Wait for 1 second

  // Set all motors to maximum reverse speed
  LoR.Set_Motor_Output(-512, M1);
  LoR.Set_Motor_Output(-512, M2);
  LoR.Set_Motor_Output(-512, M3);
  LoR.Set_Motor_Output(-512, M4);
  LoR.Set_Motor_Output(-512, M5);
  LoR.Set_Motor_Output(-512, M6);

  delay(1000);  // Wait for 1 second
}

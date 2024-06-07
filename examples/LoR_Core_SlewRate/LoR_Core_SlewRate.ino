/**
 * LoR_Core_SlewRate.ino
 * 
 * Description:
 * This project demonstrates a simple implementation of slew rate control for motor operations using the LoR library.
 * It focuses on smoothing transitions between motor speeds to enhance robotic control dynamics. The Slew Rate function 
 * is used to incrementally adjust the motor power from -512 to 512, which is essential for maintaining mechanical 
 * integrity and operational smoothness in practical applications.
 * 
 * Key Features:
 * - Slew Rate Control: Gradually adjusts motor speed to avoid sudden jerks, ensuring smoother transitions.
 * - Variable Speed Adjustment: Demonstrates full control range by cycling the target motor speed from -512 to 512.
 * - Continuous Update Cycle: Employs timing functions to periodically update motor speeds in a consistent manner.
 * 
 * Components:
 * - LoR.SlewRateFunction(): Computes incremental changes needed for smooth approach to target speed.
 * - LoR.Set_Motor_Output(): Applies the adjusted speed to the motor based on the output from the Slew Rate function.
 * 
 * Usage:
 * This example is intended as an educational tool to illustrate effective motor control strategies in robotic systems,
 * particularly beneficial in scenarios requiring precise and gradual speed adjustments.
 * 
 * Setup:
 * - Include all necessary libraries and ensure hardware connections are properly configured.
 * - For expanded library functions and more detailed usage, refer to the LoR library documentation.
 * - Install ESP32 boards through the board manager using the provided link in Arduino IDE file/preferences: 
 *   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
 * - Install BluePad32 boards using the following link in Arduino IDE file/preferences:
 *   https://raw.githubusercontent.com/ricardoquesada/esp32-arduino-lib-builder/master/bluepad32_files/package_esp32_bluepad32_index.json
 * - Code intended for an ESP32 controller, select "ESP32 dev module" as the target board (BluePad32 version).
 *
 * Authors:
 * Dave Barratt
 * Khushi Tailor
 *
 * Date:
 * JUNE 6 2024
 */

#include "LoR.h"  // Include the LoR library for motor control functionalities

int Target_Value = 0;   // Desired target speed of the motor
int Current_Value = 0;  // Current speed of the motor

unsigned long TimeLimit = 0;  // Time control for speed update cycle


///////////////////////////////////////////////////////////////////////
//                          Main SETUP                               //
///////////////////////////////////////////////////////////////////////
void setup() {
  Serial.begin(115200);  // Initialize the Serial Port for debugging outputs
  LoR.begin();           // Initialize the LoR library to prepare motor control functions
}


///////////////////////////////////////////////////////////////////////
//                          Main Loop                                //
///////////////////////////////////////////////////////////////////////
void loop() {
  if (millis() > TimeLimit) {                     // Check if it's time to update the motor speed
    if (Target_Value > 512) Target_Value = -512;  // Reset target speed if max is exceeded
    else Target_Value += 400;                     // Increment target speed gradually

    TimeLimit = millis() + 500;  // Set next update time
  }

  LoR.Set_Motor_Output(Target_Value, M1);   // Apply new speed without Slewrate compensation to motor 1
  
  Current_Value = LoR.SlewRateFunction(Target_Value, Current_Value, 10);  // Calculate new speed to apply
  LoR.Set_Motor_Output(Current_Value, M2);  // Apply new speed with Slewrate compensation to motor 2
}

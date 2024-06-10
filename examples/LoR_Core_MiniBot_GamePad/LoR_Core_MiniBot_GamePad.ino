/**
 * LoR_Core_MiniBot_GamePad.ino
 * 
 * Description:
 * This Arduino sketch is designed for controlling a robotic platform using joystick input from a gamepad, 
 * integrating with the LoR (Lord of Robots) library for efficient control of wheel-driven robots. The sketch
 * supports dynamic movement control, motor speed adjustments using a slew rate control, and visual feedback through 
 * LED signaling using an Adafruit NeoPixel LED strip. The implementation is ideal for educational or prototyping 
 * purposes in robotic applications.
 * 
 * Key Features:
 * - Drive Control: Enables directional movement using joystick inputs.
 * - Motor Speed Control: Implements a slew rate function to ensure smooth acceleration and deceleration.
 * - LED Feedback System: Utilizes an LED strip to indicate system status and operational feedback.
 * - Gamepad Support: Integrates Bluepad32 library to allow control via various gamepad controllers.
 * 
 * Components:
 * - Motion_Control: Handles joystick inputs to determine motor speed and direction.
 * - Motor_Control: Updates motor outputs to execute the motion commands.
 * - NeoPixel_SetColour: Manages LED colors to signify different system statuses.
 * - Controller Connection Management: Manages gamepad connection and disconnection events.
 * 
 * Usage:
 * Deploy this project to control robotic platforms for educational and prototyping purposes. 
 * Adjust motor and pin configurations according to your specific hardware setup.
 * 
 * Setup:
 * - Include all necessary libraries and verify that hardware connections are correctly configured.
 * - Refer to the LoR library documentation for detailed usage and additional functionalities.
 * - Install ESP32 boards through the board manager with this URL in Arduino IDE file/preferences:
 *   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
 * - Install BluePad32 boards with this URL:
 *   https://raw.githubusercontent.com/ricardoquesada/esp32-arduino-lib-builder/master/bluepad32_files/package_esp32_bluepad32_index.json
 * - Code is designed for ESP32 controllers; ensure the BluePad32 version of the "ESP32 dev module" is selected as the target board.
 *
 * Authors:
 * Dave Barratt
 * Khushi Tailor
 * 
 * Date:
 * JUNE 2, 2024
 */

#include <LoR.h>  // Include the Lord of Robots (LoR) library for robot control functionalities

//////////////////////////////////////////////////////////////////////////
/////             Serial Configuration and Functions                /////
//////////////////////////////////////////////////////////////////////////

// Initializes the Serial communication to debug and monitor robot status
void INIT_Serial() {
  Serial.begin(115200);                          // Set baud rate to 115200 for fast data transmission
  delay(200);                                    // Short delay to ensure stable communication setup
  Serial.println("MiniBot: System Startup...");  // Print startup message to Serial Monitor
}

//////////////////////////////////////////////////////////////////////////
/////                      Motion Functions                         /////
//////////////////////////////////////////////////////////////////////////

// Global variables to store current motor speed settings
int Motor_LEFT_SetValue, Motor_RIGHT_SetValue = 0;

// Processes joystick input to compute motor speeds for motion control
void Motion_Control(int ForwardBackward_Axis, int TurnLeftRight_Axis) {
  int LEFT_TargetValue = 0, RIGHT_TargetValue = 0;
  const int DEAD_BAND = 50;     // Threshold to ignore minimal joystick input
  const float TURN_RATE = 1.0;  // Scaling factor for turning motion

  // Filter out small movements within the dead band
  if (abs(ForwardBackward_Axis) < DEAD_BAND) ForwardBackward_Axis = 0;
  else {
    LEFT_TargetValue = ForwardBackward_Axis;
    RIGHT_TargetValue = ForwardBackward_Axis;
  }

  // Adjust target values based on turning axis input
  if (abs(TurnLeftRight_Axis) < DEAD_BAND) TurnLeftRight_Axis = 0;
  else {
    LEFT_TargetValue -= TURN_RATE * TurnLeftRight_Axis;
    RIGHT_TargetValue += TURN_RATE * TurnLeftRight_Axis;
  }

  // Constrain the motor values to the maximum allowed range
  LEFT_TargetValue = constrain(LEFT_TargetValue, -512, 512);
  RIGHT_TargetValue = constrain(RIGHT_TargetValue, -512, 512);

  // Apply the slew rate function to smoothly transition to new speed targets
  Motor_LEFT_SetValue = LoR.SlewRateFunction(LEFT_TargetValue, Motor_LEFT_SetValue, 300);
  Motor_RIGHT_SetValue = LoR.SlewRateFunction(RIGHT_TargetValue, Motor_RIGHT_SetValue, 300);
}

// Sends the calculated values to motor controllers to drive the robot
void Drive_Control(int LEFT, int RIGHT) {
  LoR.Set_Motor_Output(LEFT, M1);
  LoR.Set_Motor_Output(LEFT, M2);
  LoR.Set_Motor_Output(LEFT, M3);
  LoR.Set_Motor_Output(-RIGHT, M4);
  LoR.Set_Motor_Output(-RIGHT, M5);
  LoR.Set_Motor_Output(-RIGHT, M6);
}

//////////////////////////////////////////////////////////////////////////
//                     RGB LED Configuration and Functions             //
//////////////////////////////////////////////////////////////////////////

// RGB LED strip setup and configuration using Adafruit NeoPixel library
Adafruit_NeoPixel strip(LED_COUNT, LED_DataPin, NEO_GRB + NEO_KHZ800);
const uint32_t RED = strip.Color(255, 0, 0);
const uint32_t GREEN = strip.Color(0, 255, 0);
const uint32_t BLUE = strip.Color(0, 0, 255);
const uint32_t WHITE = strip.Color(255, 255, 255);
const uint32_t PURPLE = strip.Color(255, 0, 255);
const uint32_t CYAN = strip.Color(0, 255, 255);
const uint32_t YELLOW = strip.Color(255, 255, 0);

// Initializes the RGB LED strip and sets default color
void INIT_rgbLED() {
  strip.begin();            // Initialize the NeoPixel strip
  strip.show();             // Clear all pixels on strip
  strip.setBrightness(50);  // Set brightness level to 20% (max 255)
  LED_SetColour(BLUE);      // Set initial color to blue
}

// Sets the entire strip to a specified color
void LED_SetColour(uint32_t color) {
  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, color);  // Set color for each pixel
    strip.show();                   // Update strip to display new color
  }
}

//////////////////////////////////////////////////////////////////////////
//                      Gamepad Output Functions                      //
//////////////////////////////////////////////////////////////////////////

// Dumps gamepad state to the Serial Monitor for debugging
void dumpGamepad(ControllerPtr ctl) {
  Serial.printf(
    "idx=%d, dpad: 0x%02x, buttons: 0x%04x, axis L: %4d, %4d, axis R: %4d, %4d, brake: %4d, throttle: %4d, "
    "misc: 0x%02x, gyro x:%6d y:%6d z:%6d, accel x:%6d y:%6d z:%6d BATT:%6d\n",
    ctl->index(),        // Controller Index
    ctl->dpad(),         // D-pad
    ctl->buttons(),      // bitmask of pressed buttons
    ctl->axisX(),        // (-511 - 512) left X Axis
    ctl->axisY(),        // (-511 - 512) left Y axis
    ctl->axisRX(),       // (-511 - 512) right X axis
    ctl->axisRY(),       // (-511 - 512) right Y axis
    ctl->brake(),        // (0 - 1023): brake button
    ctl->throttle(),     // (0 - 1023): throttle (AKA gas) button
    ctl->miscButtons(),  // bitmask of pressed "misc" buttons
    ctl->gyroX(),        // Gyro X
    ctl->gyroY(),        // Gyro Y
    ctl->gyroZ(),        // Gyro Z
    ctl->accelX(),       // Accelerometer X
    ctl->accelY(),       // Accelerometer Y
    ctl->accelZ(),       // Accelerometer Z
    ctl->battery()       // Battery level
  );
}


//////////////////////////////////////////////////////////////////////////
//                      Gamepad Processing Functions                   //
//////////////////////////////////////////////////////////////////////////

// Processes connected gamepad inputs and sets appropriate LED color
void Process_GamePad() {
  // Check if the connected device is a supported gamepad
  if (LoR.myController->isGamepad()) {
    dumpGamepad(LoR.myController);  // Output gamepad status to Serial for debugging
    LED_SetColour(GREEN);           // Set LED strip to green to indicate active control
  } else {
    Serial.println("Unsupported controller");  // Notify via Serial if an unsupported controller is connected
    LED_SetColour(YELLOW);                     // Set LED strip to yellow to indicate an issue
  }
}

//////////////////////////////////////////////////////////////////////////
//                          Main Setup Function                         //
//////////////////////////////////////////////////////////////////////////

// Main setup function - initializes all configurations and libraries
void setup() {
  INIT_Serial();  // Initialize the serial communication for debugging output
  INIT_rgbLED();  // Initialize and configure the RGB LED strip
  LoR.begin();    // Initialize the LoR library for robotic control
  Serial.println("MiniBot: CORE System Ready! ");  // Print system ready message to the serial monitor
}

//////////////////////////////////////////////////////////////////////////
//                          Main Loop Function                          //
//////////////////////////////////////////////////////////////////////////
// Main loop function - executes the core logic repeatedly
void loop() {
  BP32.update();  // Update the state of the connected BluePad32 device
  if (LoR.myController && LoR.myController->isConnected()) {
    Process_GamePad();                                                      // Process gamepad inputs if a controller is connected
    Motion_Control(LoR.myController->axisY(), LoR.myController->axisRX());  // Control robot motion based on joystick inputs
    Drive_Control(Motor_LEFT_SetValue, Motor_RIGHT_SetValue);               // Update motor outputs
    LED_SetColour(GREEN);                                                   // Indicate operational status with green LEDs
  } else {                                                                  // If no controller is connected
    Drive_Control(STOP, STOP);                                              // Stop all motor activity
    LED_SetColour(RED);                                                     // Indicate standby mode with red LEDs
  }
}

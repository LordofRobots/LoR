/**
 * LoR_Core_BluePad32.ino
 * 
 * Description:
 * This project file integrates the LoR (Lord of Robots) library to control a robotic platform
 * equipped with wheels, utilizing joystick input for dynamic movement control and LED feedback. 
 * It features motion control via a wheel drive mechanism, which allows for movement 
 * . This implementation utilizes the LoR library to handle complex
 * input and output operations, including motor control and serial communication.
 * 
 * Key Features:
 * - rive Control: Enables movement using joystick inputs.
 * - Motor Speed Control: Uses a slew rate control function to manage acceleration and deceleration smoothly.
 * - LED Feedback System: Controls an Adafruit NeoPixel LED strip to provide visual feedback based on the system status.
 * - Gamepad Support: Integrates with Bluepad32 to support various gamepad controllers for intuitive control.
 * 
 * Components:
 * - Motion_Control: Processes joystick inputs to compute the target motor speeds.
 * - Set_Motor_Output: Manages individual motor speeds based on computed targets.
 * - Motor_Control: Coordinates the updating of all motors' outputs.
 * - NeoPixel_SetColour: Manages LED colors to reflect different operational statuses.
 * - Controller Connection Management: Handles the connection and disconnection events for gamepad controllers.
 * 
 * Usage:
 * To deploy this project, ensure that the LoR library is downloaded and installed in your Arduino environment.
 * The project is configured for ESP32-based controllers, leveraging multitasking capabilities to handle
 * multiple operations concurrently. Adjust the motor and pin configurations as necessary to match your hardware setup.
 * 
 * Setup:
 * - Ensure all necessary libraries are included and the hardware connections are correctly configured.
 * - For detailed library usage and additional functionality, refer to the LoR library documentation.
 * - Install ESP32 boards through board manager. Must include this link in file/preferences https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
 * - Install BluePad32 boards through board manager. Must include this link in file/preferences https://raw.githubusercontent.com/ricardoquesada/esp32-arduino-lib-builder/master/bluepad32_files/package_esp32_bluepad32_index.json
 * - The code is designed to be uploaded to an ESP32 controller.
 * - Target board: Choose the bluepad version of the "ESP32 dev module"
 *
 * Author:
 * Dave Barratt
 * Khushi Tailor
 * 
 * Date:
 * JUNE 2 2024
 */

#include <LoR.h> // Install via Library manager, by Lord of Robots

//////////////////////////////////////////////////////////////////////////
/////             Serial  config and functions                        /////
//////////////////////////////////////////////////////////////////////////

//Serial SETUP ------------------------------------------------------------
void INIT_Serial(){
  Serial.begin(115200);
  delay(200);
  Serial.println("MiniBot: System Startup...");
}

//////////////////////////////////////////////////////////////////////////
/////                      Motion functions                          /////
//////////////////////////////////////////////////////////////////////////
//MOTION CONTROL ALGORITHM ---------------------------------------------
// Process joystick input and calculate motor speeds
const float TURN_RATE = 1;
int Motor_LEFT_SetValue, Motor_RIGHT_SetValue = 0;
void Motion_Control(int ForwardBackward_Axis, int TurnLeftRight_Axis) {
  int LEFT_TargetValue = 0;
  int RIGHT_TargetValue = 0;
  const int DEAD_BAND = 50;

  if (abs(ForwardBackward_Axis) < DEAD_BAND) ForwardBackward_Axis = 0;
  else {
    LEFT_TargetValue = ForwardBackward_Axis;
    RIGHT_TargetValue = ForwardBackward_Axis;
  }
  //calculate rotation values
  if (abs(TurnLeftRight_Axis) < DEAD_BAND) TurnLeftRight_Axis = 0;
  else {
    LEFT_TargetValue -= (TURN_RATE * TurnLeftRight_Axis);
    RIGHT_TargetValue += (TURN_RATE * TurnLeftRight_Axis);
  }

  //constrain to joystick range
  LEFT_TargetValue = constrain(LEFT_TargetValue, -512, 512);
  RIGHT_TargetValue = constrain(RIGHT_TargetValue, -512, 512);

  //set motor speed through slew rate function
  Motor_LEFT_SetValue = LoR.SlewRateFunction(LEFT_TargetValue, Motor_LEFT_SetValue, 300);
  Motor_RIGHT_SetValue = LoR.SlewRateFunction(RIGHT_TargetValue, Motor_RIGHT_SetValue, 300);
}

// MINIBOT 3 MOTOR DRIVE ----------------------------------------------
//send values to all motors
void Drive_Control(int LEFT, int RIGHT) {
  LoR.Set_Motor_Output(LEFT, M1);
  LoR.Set_Motor_Output(LEFT, M2);
  LoR.Set_Motor_Output(LEFT, M3);
  LoR.Set_Motor_Output(-RIGHT, M4);
  LoR.Set_Motor_Output(-RIGHT, M5);
  LoR.Set_Motor_Output(-RIGHT, M6);
}




//////////////////////////////////////////////////////////////////////////
//                     RGB LED config and functions                     //
//////////////////////////////////////////////////////////////////////////

// SETUP & CONFIG --------------------------------------------------------
// Set a specific color for the entire NeoPixel strip
// NeoPixel Configurations

Adafruit_NeoPixel strip(LED_COUNT, LED_DataPin, NEO_GRB + NEO_KHZ800);
const uint32_t RED = strip.Color(255, 0, 0, 0);
const uint32_t GREEN = strip.Color(0, 255, 0, 0);
const uint32_t BLUE = strip.Color(0, 0, 255, 0);
const uint32_t WHITE = strip.Color(0, 0, 0, 255);
const uint32_t PURPLE = strip.Color(255, 0, 255, 0);
const uint32_t CYAN = strip.Color(0, 255, 255, 0);
const uint32_t YELLOW = strip.Color(255, 255, 0, 0);

// LED Configuration
void INIT_rgbLED() {
  strip.begin();            // INITIALIZE NeoPixel strip object
  strip.show();             // Turn OFF all pixels ASAP
  strip.setBrightness(50);  // Set BRIGHTNESS to about 1/5 (max = 255)
  LED_SetColour(BLUE);
}

void LED_SetColour(uint32_t color) {
  for (int i = 0; i < strip.numPixels(); i++) {  // For each pixel in strip...
    strip.setPixelColor(i, color);               //  Set pixel's color (in RAM)
    strip.show();                                // Update strip with new contents
  }
}



// GAMEPAD SERIAL OUTPUT -----------------------------------------------------------------------------
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

// GAMEPAD BATTERY -----------------------------------------------
void CheckController_Battery() {
  int Batt_Level = map(LoR.myController->battery(), 0, 255, 1, 8);

  LoR.myController->setPlayerLEDs(Batt_Level);
  if (Batt_Level < 4) {
    LoR.myController->setColorLED(200, 255, 0);
  } else if (Batt_Level < 2) {
    LoR.myController->setColorLED(255, 0, 0);
    //myController->playDualRumble(100, 100, 100, 100);
  } else {
    LoR.myController->setColorLED(0, 255, 0);
  }
}

// GAME PAD COMPATIBILITY -----------------------------------------------
void Process_GamePad() {
  if (LoR.myController->isGamepad()) {
    dumpGamepad(LoR.myController);
    LED_SetColour(GREEN);
    //CheckController_Battery();
  } else {
    Serial.println("Unsupported controller");
    LED_SetColour(YELLOW);
  }
}

///////////////////////////////////////////////////////////////////////
//                          Main SETUP                               //
///////////////////////////////////////////////////////////////////////

// Set up pins, LED PWM functionalities and begin PS4 controller, Serial and Serial2 communication
void setup() {
  INIT_Serial();
  INIT_rgbLED();
  LoR.begin();  // Initialize the LoR library
  // INIT_BluePad32();

  LED_SetColour(RED);
  Serial.println("MiniBot: CORE System Ready! ");
}


///////////////////////////////////////////////////////////////////////
//                          Main Loop                               //
///////////////////////////////////////////////////////////////////////

void loop() {

  BP32.update();
  if (LoR.myController && LoR.myController->isConnected()) {
    Process_GamePad();
    Motion_Control(LoR.myController->axisY(), LoR.myController->axisRX());  // Joystick control
    Drive_Control(Motor_LEFT_SetValue, Motor_RIGHT_SetValue);
    LED_SetColour(GREEN);

  }
  else {  //Stop/Standby
    Drive_Control(STOP, STOP);
    LED_SetColour(RED);
  }
}
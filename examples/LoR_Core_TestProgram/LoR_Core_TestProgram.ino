/**
 * LoR_Core_TestProgram.ino
 *
 * Description:
 * This test program for the LoR_Core module includes functionality tests for servos, motors, and visual feedback via NeoPixel LEDs. It employs a command-controlled testing structure, enabling "start", "loop", and "stop" commands to manage test cycles. The tests assess the operation of servo positions, motor speeds, and LED color changes, which demonstrate the system's capabilities during different test states.
 *
 * Key Features:
 * - Command-Controlled Testing: Use "start", "loop", and "stop" commands to manage single or continuous test cycles.
 * - Servo Testing: Adjusts servo positions through INIT_Servo during test scenarios.
 * - Motor Speed Testing: Controls motor speeds and verifies operation across all motor channels.
 * - Visual Feedback: NeoPixel LEDs display a variety of colors to indicate test phases and status.
 *
 * Components:
 * - INIT_rgbLED: Initializes the NeoPixel strip and sets default color.
 * - LED_SetColour: Dynamically changes LED colors based on test progress.
 * - LED_Rainbow: Cycles through a rainbow color pattern for visual testing.
 *
 * Usage:
 * Ensure the LoR library is installed within your Arduino environment. Configure the project for an ESP32 controller, ideal for handling multiple operations simultaneously required for rigorous testing.
 *
 * Setup:
 * - Include necessary libraries and ensure correct hardware setup.
 * - Refer to the LoR library documentation for detailed usage.
 * - Install ESP32 and BluePad32 boards via board manager with URLs provided in file/preferences.
 * - Code intended for ESP32 controller; specifically, use the BluePad32 version of the "ESP32 dev module."
 *
 * Author:
 * Dave Barratt
 *
 * Date:
 * JUNE 25, 2024
 */

#include <LoR.h>

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
  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, color);
    strip.show();
  }
}

long firstPixelHue = 0;
void LED_Rainbow() {
  strip.rainbow(firstPixelHue);
  strip.show();
  firstPixelHue += 256;
  if (firstPixelHue >= 5 * 65536) firstPixelHue = 0;
}

bool startTest = false;
bool loopTest = false;

void setup() {
  Serial.begin(115200);
  INIT_rgbLED();

  LoR.begin();

  LoR.INIT_Servo();  // Initialize the servo settings

  delay(1500);
  Serial.println("CORE System Ready for testing! - Test Commands: start, loop, stop");
  LED_SetColour(PURPLE);
}

long TimeLimit = 0;
int StepNumber = 0;

void loop() {
  if (!StepNumber && digitalRead(SwitchPin)) LED_Rainbow();
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();  // Remove any leading/trailing whitespace

    if (input.equalsIgnoreCase("start")) {
      startTest = true;
      loopTest = false;
      StepNumber = 1;
      Serial.println("Starting single test cycle...");
    } else if (input.equalsIgnoreCase("loop")) {
      loopTest = true;
      StepNumber = 1;
      Serial.println("Starting continuous loop test...");
    } else if (input.equalsIgnoreCase("stop")) {
      startTest = false;
      loopTest = false;
      StepNumber = 0;
      Serial.println("Stopping all tests...");
    }
  }

  int SPEED = STOP;
  int Position = 90;
  if (startTest || loopTest) {

    switch (StepNumber) {
      case 1:
        Position = 180;
        LED_SetColour(RED);
        if (millis() > TimeLimit) {
          TimeLimit = millis() + 1000;
          StepNumber++;
        }
        break;
      case 2:
        Position = 0;
        LED_SetColour(YELLOW);
        if (millis() > TimeLimit) {
          TimeLimit = millis() + 1000;
          StepNumber++;
        }
        break;
      case 3:
        Position = 90;
        LED_SetColour(GREEN);
        if (millis() > TimeLimit) {
          TimeLimit = millis() + 1000;
          StepNumber++;
        }
        break;
      case 4:
        SPEED = 200;
        LED_SetColour(CYAN);
        if (millis() > TimeLimit) {
          TimeLimit = millis() + 1000;
          StepNumber++;
        }
        break;
      case 5:
        SPEED = -200;
        LED_SetColour(BLUE);
        if (millis() > TimeLimit) {
          TimeLimit = millis() + 1000;
          StepNumber++;
        }
        break;
      default:
        LED_SetColour(PURPLE);  // Default LED color to purple
        SPEED = STOP;// Stop all motor movement
        
        // Reset to initial state after a loop or at the end of all steps
        if (millis() > TimeLimit) {
          TimeLimit = millis() + 1000;
          if (loopTest) {
            StepNumber = 1;  // Continue looping
          } else {
            startTest = false;
            StepNumber = 0; // Stop testing
          }
        }
        break;
    }

  } else {
    SPEED = STOP; // Ensure motors are stopped if no tests are running
    Position = 90;  // Reset servo position to neutral
  }

  // Apply test configurations to servos and motors
  LoR.Servo_SetPosition(Servo_CH[0], Position);
  LoR.Servo_SetPosition(Servo_CH[1], Position);
  LoR.Servo_SetPosition(Servo_CH[2], Position);
  LoR.Servo_SetPosition(Servo_CH[3], Position);
  LoR.Set_Motor_Output(SPEED, M1);
  LoR.Set_Motor_Output(SPEED, M2);
  LoR.Set_Motor_Output(SPEED, M3);
  LoR.Set_Motor_Output(SPEED, M4);
  LoR.Set_Motor_Output(SPEED, M5);
  LoR.Set_Motor_Output(SPEED, M6);
}

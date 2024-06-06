/** 
 * LoR_Core_TestProgram.ino
 * 
 * Description:
 * This program is a comprehensive test suite for the LoR_Core module, designed to verify all functionalities,
 * including switch operations, motor outputs through six Mx channels, and servo control via the Aux_IO port
 * and a servo expansion board. It employs the LoR library for extensive testing of each component, ensuring
 * operational reliability. The inclusion of a visual feedback system via NeoPixel LEDs aids in monitoring
 * test statuses in real time, making it an essential tool for production verification.
 * 
 * Key Features:
 * - Complete Module Testing: Ensures full operational checks on switches, motor outputs, and servo controls.
 * - Servo Management: Uses Aux_IO and a servo expansion board for precise control testing.
 * - Motor Verification: Tests functionality across all six motor channels.
 * - Visual Feedback: NeoPixel LEDs provide immediate status updates during tests.
 * - Command-Controlled Testing: Implements "start", "loop", and "stop" commands for comprehensive test cycles.
 * 
 * Components:
 * - Setup_SERVO: Initializes servo settings for testing.
 * - Motor_STOP: Ensures all motors are stopped at the end of testing or in case of a halt command.
 * - DriveControl: Controls motor directions and speeds during testing scenarios.
 * - LED_SetColour: Changes LED colors according to different test states for clear indication.
 * 
 * Usage:
 * Download and install the LoR library within your Arduino environment. Ensure the project is properly configured for an
 * ESP32 controller, which is suited for managing multiple concurrent operations essential for comprehensive testing.
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
 
 #include <LoR.h>

const int servo1_Channel = 12;
const int servo2_Channel = 13;
const int servo3_Channel = 14;
const int servo4_Channel = 15;

// Define PWM parameters
const double SERVO_PWM_FREQUENCY = 50;   // Frequency of PWM signal (50Hz)
const double SERVO_PWM_RESOLUTION = 12;  // Resolution of PWM signal (12-bit, giving 4095 levels)
const double minPulseWidth = 500;
const double maxPulseWidth = 2500;
int SERVO_PWM_MIN = int(minPulseWidth / ((1000000 / SERVO_PWM_FREQUENCY) / 4095));
int SERVO_PWM_MAX = int(maxPulseWidth / ((1000000 / SERVO_PWM_FREQUENCY) / 4095));

void Setup_SERVO() {
  pinMode(servo1Pin, OUTPUT);
  pinMode(servo2Pin, OUTPUT);
  pinMode(servo3Pin, OUTPUT);
  pinMode(servo4Pin, OUTPUT);
  digitalWrite(servo1Pin, 0);
  digitalWrite(servo2Pin, 0);
  digitalWrite(servo3Pin, 0);
  digitalWrite(servo4Pin, 0);
  ledcSetup(servo1_Channel, SERVO_PWM_FREQUENCY, SERVO_PWM_RESOLUTION);
  ledcSetup(servo2_Channel, SERVO_PWM_FREQUENCY, SERVO_PWM_RESOLUTION);
  ledcSetup(servo3_Channel, SERVO_PWM_FREQUENCY, SERVO_PWM_RESOLUTION);
  ledcSetup(servo4_Channel, SERVO_PWM_FREQUENCY, SERVO_PWM_RESOLUTION);
  ledcAttachPin(servo1Pin, servo1_Channel);
  ledcAttachPin(servo2Pin, servo2_Channel);
  ledcAttachPin(servo3Pin, servo3_Channel);
  ledcAttachPin(servo4Pin, servo4_Channel);
}

void SERVO_Move(int SERVO_CH, int Position_Value) {
  int DutyCycle = map(Position_Value, 0, 180, SERVO_PWM_MIN, SERVO_PWM_MAX);
  ledcWrite(SERVO_CH, DutyCycle);
}

// Control all motors
int Motor_LEFT_SetValue = 0, Motor_RIGHT_SetValue = 0;
void DriveControl(int LEFT, int RIGHT) {
  Motor_LEFT_SetValue = LoR.SlewRateFunction(LEFT, Motor_LEFT_SetValue, 300);
  Motor_RIGHT_SetValue = LoR.SlewRateFunction(RIGHT, Motor_RIGHT_SetValue, 300);
  LoR.Set_Motor_Output(Motor_LEFT_SetValue, M1);
  LoR.Set_Motor_Output(Motor_LEFT_SetValue, M2);
  LoR.Set_Motor_Output(Motor_LEFT_SetValue, M3);
  LoR.Set_Motor_Output(-Motor_RIGHT_SetValue, M4);
  LoR.Set_Motor_Output(-Motor_RIGHT_SetValue, M5);
  LoR.Set_Motor_Output(-Motor_RIGHT_SetValue, M6);
}

// Stop motors from spinning
void Motor_STOP() {
  DriveControl(STOP, STOP);
}


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

  Setup_SERVO();

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
      Motor_STOP();
      Serial.println("Stopping all tests...");
    }
  }

  if (startTest || loopTest) {
    switch (StepNumber) {
      case 1:
        SERVO_Move(servo1_Channel, 180);
        SERVO_Move(servo2_Channel, 180);
        SERVO_Move(servo3_Channel, 180);
        SERVO_Move(servo4_Channel, 180);
        LED_SetColour(RED);
        if (millis() > TimeLimit) {
          TimeLimit = millis() + 1000;
          StepNumber++;
        }
        break;
      case 2:
        SERVO_Move(servo1_Channel, 0);
        SERVO_Move(servo2_Channel, 0);
        SERVO_Move(servo3_Channel, 0);
        SERVO_Move(servo4_Channel, 0);
        LED_SetColour(YELLOW);
        if (millis() > TimeLimit) {
          TimeLimit = millis() + 1000;
          StepNumber++;
        }
        break;
      case 3:
        SERVO_Move(servo1_Channel, 90);
        SERVO_Move(servo2_Channel, 90);
        SERVO_Move(servo3_Channel, 90);
        SERVO_Move(servo4_Channel, 90);
        LED_SetColour(GREEN);
        if (millis() > TimeLimit) {
          TimeLimit = millis() + 1000;
          StepNumber++;
        }
        break;
      case 4:
        DriveControl(300, 300);
        LED_SetColour(CYAN);
        if (millis() > TimeLimit) {
          TimeLimit = millis() + 1000;
          StepNumber++;
        }
        break;
      case 5:
        DriveControl(-300, -300);
        LED_SetColour(BLUE);
        if (millis() > TimeLimit) {
          TimeLimit = millis() + 1000;
          StepNumber++;
        }
        break;
      default:
        LED_SetColour(PURPLE);
        Motor_STOP();
        if (millis() > TimeLimit) {
          TimeLimit = millis() + 1000;
          if (loopTest) {
            StepNumber = 1;
          } else {
            startTest = false;
            StepNumber = 0;
          }
        }
        break;
    }
  }
}

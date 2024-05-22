#include <Adafruit_NeoPixel.h>

// IO Interface Definitions
#define LED_DataPin 12
#define LED_COUNT 36
#define SwitchPin 34
#define MotorEnablePin 13
#define servoPin1 16
#define servoPin2 17
#define servoPin3 21
#define servoPin4 22

// Motor Pin Definitions
#define motorPin_M1_A 26
#define motorPin_M1_B 18
#define motorPin_M2_A 14
#define motorPin_M2_B 5
#define motorPin_M3_A 15
#define motorPin_M3_B 33
#define motorPin_M4_A 23
#define motorPin_M4_B 19
#define motorPin_M5_A 25
#define motorPin_M5_B 27
#define motorPin_M6_A 4
#define motorPin_M6_B 32

const int motorPins_A[] = { motorPin_M1_A, motorPin_M2_A, motorPin_M3_A, motorPin_M4_A, motorPin_M5_A, motorPin_M6_A };
const int motorPins_B[] = { motorPin_M1_B, motorPin_M2_B, motorPin_M3_B, motorPin_M4_B, motorPin_M5_B, motorPin_M6_B };

// Motor PWM Configuration Definitions
const int Motor_M1_A = 0;
const int Motor_M1_B = 1;
const int Motor_M2_A = 2;
const int Motor_M2_B = 3;
const int Motor_M3_A = 4;
const int Motor_M3_B = 5;
const int Motor_M4_A = 6;
const int Motor_M4_B = 7;
const int Motor_M5_A = 8;
const int Motor_M5_B = 9;
const int Motor_M6_A = 10;
const int Motor_M6_B = 11;
const int servo1_Channel = 12;
const int servo2_Channel = 13;
const int servo3_Channel = 14;
const int servo4_Channel = 15;

const int MOTOR_PWM_Channel_A[] = { Motor_M1_A, Motor_M2_A, Motor_M3_A, Motor_M4_A, Motor_M5_A, Motor_M6_A };
const int MOTOR_PWM_Channel_B[] = { Motor_M1_B, Motor_M2_B, Motor_M3_B, Motor_M4_B, Motor_M5_B, Motor_M6_B };
const int PWM_FREQUENCY = 20000;
const int PWM_RESOLUTION = 8;

// Define PWM parameters
const double SERVO_PWM_FREQUENCY = 50;   // Frequency of PWM signal (50Hz)
const double SERVO_PWM_RESOLUTION = 12;  // Resolution of PWM signal (12-bit, giving 4095 levels)
const double minPulseWidth = 500;
const double maxPulseWidth = 2500;
int SERVO_PWM_MIN = int(minPulseWidth / ((1000000 / SERVO_PWM_FREQUENCY) / 4095));
int SERVO_PWM_MAX = int(maxPulseWidth / ((1000000 / SERVO_PWM_FREQUENCY) / 4095));

void Setup_SERVO() {
  pinMode(servoPin1, OUTPUT);
  pinMode(servoPin2, OUTPUT);
  pinMode(servoPin3, OUTPUT);
  pinMode(servoPin4, OUTPUT);
  digitalWrite(servoPin1, 0);
  digitalWrite(servoPin2, 0);
  digitalWrite(servoPin3, 0);
  digitalWrite(servoPin4, 0);
  ledcSetup(servo1_Channel, SERVO_PWM_FREQUENCY, SERVO_PWM_RESOLUTION);
  ledcSetup(servo2_Channel, SERVO_PWM_FREQUENCY, SERVO_PWM_RESOLUTION);
  ledcSetup(servo3_Channel, SERVO_PWM_FREQUENCY, SERVO_PWM_RESOLUTION);
  ledcSetup(servo4_Channel, SERVO_PWM_FREQUENCY, SERVO_PWM_RESOLUTION);
  ledcAttachPin(servoPin1, servo1_Channel);
  ledcAttachPin(servoPin2, servo2_Channel);
  ledcAttachPin(servoPin3, servo3_Channel);
  ledcAttachPin(servoPin4, servo4_Channel);
}

void SERVO_Move(int SERVO_CH, int Position_Value) {
  int DutyCycle = map(Position_Value, 0, 180, SERVO_PWM_MIN, SERVO_PWM_MAX);
  ledcWrite(SERVO_CH, DutyCycle);
}

// Function to handle slew rate for motor speed ramping
int SLEW_RATE_MS = 300;  // smaller number = more slew time or slower acceleration
int SlewRateFunction(int Input_Target, int Input_Current) {
  int speedDiff = Input_Target - Input_Current;
  if (speedDiff > 0) Input_Current += min(speedDiff, SLEW_RATE_MS);
  else if (speedDiff < 0) Input_Current -= min(-speedDiff, SLEW_RATE_MS);
  Input_Current = constrain(Input_Current, -512, 512);
  return Input_Current;
}

// Function to control motor output based on input values
const int MAX_SPEED = 255;
const int MIN_SPEED = -255;
const int MIN_STARTING_SPEED = 140;
const int STOP = 0;
bool INVERT = false;
void Set_Motor_Output(int Output, int Motor_ChA, int Motor_ChB) {
  if (INVERT) Output = -Output;
  Output = constrain(Output, -512, 512);
  int Mapped_Value = map(abs(Output), 0, 512, MIN_STARTING_SPEED, MAX_SPEED);
  int A, B = 0;
  if (Output < -20) {
    A = 0;
    B = Mapped_Value;
  } else if (Output > 20) {
    A = Mapped_Value;
    B = 0;
  } else {
    A = STOP;
    B = STOP;
  }
  ledcWrite(Motor_ChA, A);
  ledcWrite(Motor_ChB, B);
}

// Control all motors
int Motor_LEFT_SetValue = 0, Motor_RIGHT_SetValue = 0;
void DriveControl(int LEFT, int RIGHT) {
  SLEW_RATE_MS = 300;
  Motor_LEFT_SetValue = SlewRateFunction(LEFT, Motor_LEFT_SetValue);
  Motor_RIGHT_SetValue = SlewRateFunction(RIGHT, Motor_RIGHT_SetValue);
  Set_Motor_Output(Motor_LEFT_SetValue, Motor_M1_A, Motor_M1_B);
  Set_Motor_Output(Motor_LEFT_SetValue, Motor_M2_A, Motor_M2_B);
  Set_Motor_Output(Motor_LEFT_SetValue, Motor_M3_A, Motor_M3_B);
  Set_Motor_Output(-Motor_RIGHT_SetValue, Motor_M4_A, Motor_M4_B);
  Set_Motor_Output(-Motor_RIGHT_SetValue, Motor_M5_A, Motor_M5_B);
  Set_Motor_Output(-Motor_RIGHT_SetValue, Motor_M6_A, Motor_M6_B);
}

// Stop motors from spinning
void Motor_STOP() {
  DriveControl(STOP, STOP);
}

// Tones created in the motors. Cycle through each motor.
void Start_Tone() {
  for (int i = 0; i < 6; i++) {
    long ToneTime = millis() + 200;
    bool state = 0;
    while (millis() < ToneTime) {
      digitalWrite(motorPins_A[i], state);
      digitalWrite(motorPins_B[i], !state);
      state = !state;
      long WaitTime = micros() + (100 * (i + 1));
      while (micros() < WaitTime) {}
    }
    digitalWrite(motorPins_A[i], 0);
    digitalWrite(motorPins_B[i], 0);
    delay(50);
  }
  for (int i = 0; i < 6; i++) {
    digitalWrite(motorPins_A[i], 0);
    digitalWrite(motorPins_B[i], 0);
  }
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

void NeoPixel_SetColour(uint32_t color) {
  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, color);
    strip.show();
  }
}

long firstPixelHue = 0;
void NeoPixel_Rainbow() {
  strip.rainbow(firstPixelHue);
  strip.show();
  firstPixelHue += 256;
  if (firstPixelHue >= 5 * 65536) firstPixelHue = 0;
}

bool startTest = false;
bool loopTest = false;

void setup() {
  Serial.begin(115200);

  pinMode(LED_DataPin, OUTPUT);
  pinMode(SwitchPin, INPUT_PULLUP);
  pinMode(MotorEnablePin, OUTPUT);

  for (int i = 0; i < 6; i++) {
    pinMode(motorPins_A[i], OUTPUT);
    pinMode(motorPins_B[i], OUTPUT);
    digitalWrite(motorPins_A[i], 0);
    digitalWrite(motorPins_B[i], 0);
  }

  digitalWrite(LED_DataPin, 0);
  delay(1000);
  digitalWrite(MotorEnablePin, 1);

  strip.begin();
  strip.show();
  strip.setBrightness(50);

  NeoPixel_SetColour(BLUE);
  Start_Tone();

  for (int i = 0; i < 6; i++) {
    ledcSetup(MOTOR_PWM_Channel_A[i], PWM_FREQUENCY, PWM_RESOLUTION);
    ledcSetup(MOTOR_PWM_Channel_B[i], PWM_FREQUENCY, PWM_RESOLUTION);
    ledcAttachPin(motorPins_A[i], MOTOR_PWM_Channel_A[i]);
    ledcAttachPin(motorPins_B[i], MOTOR_PWM_Channel_B[i]);
  }
  Motor_STOP();

  NeoPixel_SetColour(PURPLE);
  Setup_SERVO();

  delay(1500);
  Serial.println("CORE System Ready for testing! - Test Commands: start, loop, stop");
  NeoPixel_SetColour(CYAN);
}

long TimeLimit = 0;
int StepNumber = 0;

void loop() {
  if (!StepNumber && digitalRead(SwitchPin)) NeoPixel_Rainbow();
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
        NeoPixel_SetColour(RED);
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
        NeoPixel_SetColour(YELLOW);
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
        NeoPixel_SetColour(GREEN);
        if (millis() > TimeLimit) {
          TimeLimit = millis() + 1000;
          StepNumber++;
        }
        break;
      case 4:
        DriveControl(200, 200);
        NeoPixel_SetColour(CYAN);
        if (millis() > TimeLimit) {
          TimeLimit = millis() + 1000;
          StepNumber++;
        }
        break;
      case 5:
        DriveControl(-200, -200);
        NeoPixel_SetColour(BLUE);
        if (millis() > TimeLimit) {
          TimeLimit = millis() + 1000;
          StepNumber++;
        }
        break;
      default:
        NeoPixel_SetColour(PURPLE);
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

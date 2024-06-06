/**
 * @file LoR.cpp
 * 
 * Description:
 * This file is part of the LoR (Lord of Robots) library, which is designed to facilitate
 * complex robotic operations. It includes the implementation of the LoRClass, a central
 * class in the library that manages serial input for command execution and controls various
 * motor operations using FreeRTOS tasks.
 * 
 * Key Features:
 * - Motor Control: Manages multiple motor operations via PWM using predefined motor pin arrays.
 * - Serial Communication Handling: Processes incoming serial commands in a dedicated FreeRTOS task.
 * - Responsive Actions: Executes specific actions like Bluetooth key forgetting upon receiving predefined commands.
 * 
 * Major Functions:
 * - begin(): Initializes and starts a FreeRTOS task for handling serial inputs effectively.
 * - handleSerialInput(void* parameter): A static member function that runs as a task to monitor and respond to serial inputs.
 * 
 * Usage:
 * Include this file in your project along with its corresponding header (LoR.h) to manage robotic mechanisms
 * efficiently with a focus on responsiveness and multitasking. Ensure that the ESP32 environment is properly
 * set up as it utilizes specific FreeRTOS functionalities.
 * 
 * Author:
 * Dave Barratt
 * Khushi Tailor
 * 
 * Date:
 * JUNE 2 2024
 */
#include "LoR.h"

//motor pin definitions
const int motorPins_A[] = { motorPin_M1_A, motorPin_M2_A, motorPin_M3_A, motorPin_M4_A, motorPin_M5_A, motorPin_M6_A };
const int motorPins_B[] = { motorPin_M1_B, motorPin_M2_B, motorPin_M3_B, motorPin_M4_B, motorPin_M5_B, motorPin_M6_B };
const int MOTOR_PWM_Channel_A[] = { Motor_M1_A, Motor_M2_A, Motor_M3_A, Motor_M4_A, Motor_M5_A, Motor_M6_A };
const int MOTOR_PWM_Channel_B[] = { Motor_M1_B, Motor_M2_B, Motor_M3_B, Motor_M4_B, Motor_M5_B, Motor_M6_B };

// Define constants
const int PWM_FREQUENCY = 20000;
const int PWM_RESOLUTION = 8;
const int MAX_SPEED = 255;
const int MIN_STARTING_SPEED = 100;
const int STOP = 0;


// Define the specific word you're looking for
const String LoRClass::targetWord = "obliviate";

// Constructor for LoRClass, initializing the stack size and priority for the task
LoRClass::LoRClass(uint16_t stackSize, uint8_t priority)
  : stackSize(stackSize), priority(priority) {}

// Function to initialize and start the task for handling serial input
void LoRClass::begin() {
  // Create a task for handling serial input
  BaseType_t result = xTaskCreate(
    handleSerialInput,  // Function to be called
    "SerialTask",       // Name of the task
    stackSize,          // Stack size (bytes)
    this,               // Parameter to pass to function
    priority,           // Task priority
    NULL                // Task handle (can be used to interact with the task after creation)
  );

  // Check if the task creation was successful
  if (result != pdPASS) {
    Serial.println("Failed to create task!");
  }
  
  LoR.INIT_GPIO();
  LoR.INIT_PWM();
  LoR.INIT_BluePad32();
}

// Function to handle serial input in a separate task
void LoRClass::handleSerialInput(void* parameter) {
    LoRClass* instance = static_cast<LoRClass*>(parameter);  // Cast the parameter to LoRClass instance
    Serial.println("Task running");  // Debug message to indicate the task has started
    while (true) {  // Infinite loop to continuously check for serial input
        if (Serial.available() > 0) {  // Check if data is available to read
            String receivedWord = Serial.readStringUntil('\n');  // Read the incoming data until newline character
            receivedWord.trim();  // Remove any trailing whitespace characters
            Serial.print("Received: ");
            Serial.println(receivedWord);  // Output the received word for debugging
            if (receivedWord.equals(instance->targetWord)) {  // Check if the received word matches the target word
                BP32.forgetBluetoothKeys();
                Serial.println("Mischief managed");  // Output the response if the target word is matched
            }
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);  // Add a small delay to prevent the task from hogging the CPU
    }
}


/**
 * Initializes the GPIO pins used for motors and LEDs.
 * Sets initial states to ensure the system starts correctly.
 * This includes setting motor pins to LOW to avoid unintended movements
 * and ensuring the motor enable pin is activated after a brief delay.
 */
void LoRClass::INIT_GPIO() {
    // Initialize LED and motor enable pins
    pinMode(LED_DataPin, OUTPUT);
    pinMode(SwitchPin, INPUT_PULLUP);
    pinMode(MotorEnablePin, OUTPUT);
    digitalWrite(MotorEnablePin, LOW);

    // Initialize motor control pins
    for (int i = 0; i < 6; i++) {
        pinMode(motorPins_A[i], OUTPUT);
        pinMode(motorPins_B[i], OUTPUT);
        digitalWrite(motorPins_A[i], LOW);
        digitalWrite(motorPins_B[i], LOW);
    }

    // Ensure stable start-up
    delay(1000);
    digitalWrite(MotorEnablePin, HIGH);

    // Check motor functionality with start-up tones
    Start_Tone();
}

/**
 * Configures the PWM settings for motor control.
 * Sets up PWM channels for each motor pin using predefined frequency and resolution.
 */
void LoRClass::INIT_PWM() {
    for (int i = 0; i < 6; i++) {
        ledcSetup(MOTOR_PWM_Channel_A[i], PWM_FREQUENCY, PWM_RESOLUTION);
        ledcSetup(MOTOR_PWM_Channel_B[i], PWM_FREQUENCY, PWM_RESOLUTION);
        ledcAttachPin(motorPins_A[i], MOTOR_PWM_Channel_A[i]);
        ledcAttachPin(motorPins_B[i], MOTOR_PWM_Channel_B[i]);
    }
}

/**
 * Generates a brief tone on each motor to verify correct operation.
 * This function cycles through each motor, toggling its state rapidly to create an audible tone.
 */
void LoRClass::Start_Tone() {
    for (int i = 0; i < 6; i++) {
        unsigned long toneTime = millis() + 200;
        bool state = false;
        while (millis() < toneTime) {
            digitalWrite(motorPins_A[i], state);
            digitalWrite(motorPins_B[i], !state);
            state = !state;
            unsigned long waitTime = micros() + (100 * (i + 1));
            while (micros() < waitTime);
        }
        digitalWrite(motorPins_A[i], LOW);
        digitalWrite(motorPins_B[i], LOW);
        delay(50);
    }
}

/**
 * Controls the rate of change in motor speed to smooth transitions.
 * This function helps in preventing abrupt changes in motor speed which can be mechanically stressful.
 *
 * @param inputTarget The desired motor speed.
 * @param inputCurrent The current motor speed.
 * @return The new motor speed adjusted according to the slew rate.
 */
int LoRClass::SlewRateFunction(int inputTarget, int inputCurrent, int Rate = 200) {
    int speedDiff = inputTarget - inputCurrent;
    if (speedDiff > 0) inputCurrent += min(speedDiff, Rate);
    else if (speedDiff < 0) inputCurrent -= min(-speedDiff, Rate);
    inputCurrent = constrain(inputCurrent, -512, 512);
    return inputCurrent;
}

/**
 * Sets the motor output based on computed values.
 * This function maps the desired motor output to the PWM value range configured for the motor drivers.
 *
 * @param output - The desired output level ranging from -512 to 512.
 * @param M_Output - the desired Mx motor output used.
 */
void LoRClass::Set_Motor_Output(int output, int M_Output) {
	int motorChA = MOTOR_PWM_Channel_A[M_Output];
	int motorChB = MOTOR_PWM_Channel_B[M_Output];
    output = constrain(output, -512, 512);
    int mappedValue = map(abs(output), 0, 512, MIN_STARTING_SPEED, MAX_SPEED);
    int pinA = output > 0 ? mappedValue : STOP;
    int pinB = output < 0 ? mappedValue : STOP;
    ledcWrite(motorChA, pinA);
    ledcWrite(motorChB, pinB);
}

void LoRClass::INIT_BluePad32() {
    const uint8_t* addr = BP32.localBdAddress();
    Serial.printf("BD Addr: %2X:%2X:%2X:%2X:%2X:%2X\n", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
    BP32.setup(&LoRClass::onConnectedController, &LoRClass::onDisconnectedController);
    BP32.enableVirtualDevice(false);
}

void LoRClass::onConnectedController(ControllerPtr ctl) {
    if (LoR.myController == nullptr) {
        LoR.myController = ctl;
		LoR.myController->playDualRumble(100, 100, 255, 255);
	} else {
    Serial.println("Another controller tried to connect but is rejected");
    ctl->disconnect();  // Reject the connection if another controller tries to connect    }
	}
	}

void LoRClass::onDisconnectedController(ControllerPtr ctl) {
    if (LoR.myController == ctl) {
        LoR.myController = nullptr;
		Serial.println("Controller disconnected");
        // Additional code for when controller disconnects
    }
}


// Global instance of LoRClass
LoRClass LoR;


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

// Global instance of LoRClass
LoRClass LoR;

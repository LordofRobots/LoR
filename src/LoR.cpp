#include "LoR.h"

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

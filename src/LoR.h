#ifndef LOR_H
#define LOR_H

#include <Arduino.h>


// Define the LoRClass class to manage serial input handling
class LoRClass {
public:
    // Constructor to initialize the task parameters with default values
    LoRClass(uint16_t stackSize = 2048, uint8_t priority = 0);
    
    // Function to start the serial input handling task
    void begin();
    
private:
    // Static function to handle serial input in a separate task
    static void handleSerialInput(void* parameter);
    
    // Static constant string that holds the target word to trigger the action
    static const String targetWord;
    
    // Task parameters
    uint16_t stackSize;   // Stack size for the task
    uint8_t priority;     // Priority of the task
};

// Externally declare a global instance of LoRClass
extern LoRClass LoR;

#endif // LOR_H


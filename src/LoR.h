/**
 * @file LoR.h
 * 
 * Description:
 * This header file is part of the LoR (Lord of Robots) library, designed to support robust and flexible control
 * systems for robotics applications. It defines the structure of the LoRClass, motor and servo pin configurations,
 * and necessary constants for managing motor operations through PWM on the ESP32 platform.
 * 
 * Key Features:
 * - Motor and Servo Pin Definitions: Provides global definitions for motor and servo control pins.
 * - PWM Configuration: Specifies the channels and parameters for PWM operations tailored to motor control.
 * - LoRClass: Central class for managing serial inputs and executing tasks based on predefined commands.
 * 
 * Components:
 * - LoRClass: Handles initialization of tasks for serial communication and motor control using FreeRTOS.
 * - Motor and IO Definitions: Declares constants used for pin assignments and PWM setup.
 * 
 * Usage:
 * Include this header in any project files that require direct interaction with motor controls or need to
 * utilize the serial communication features provided by the LoR library. Ensure compatibility with the ESP32
 * hardware platform and configure the Arduino environment accordingly.
 * 
 * Author:
 * Dave Barratt
 * Khushi Tailor
 * 
 * Date:
 * JUNE 2 2024
 */

#ifndef LOR_H
#define LOR_H

#include <Arduino.h>
#include <Bluepad32.h>
#include <Adafruit_NeoPixel.h>

// IO Interface Definitions
#define LED_DataPin 12
#define LED_COUNT 36
#define SwitchPin 34
#define channel1Pin 16
#define channel2Pin 17
#define channel3Pin 21
#define channel4Pin 22
#define servo1Pin 16
#define servo2Pin 17
#define servo3Pin 21
#define servo4Pin 22

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
#define MotorEnablePin 13

extern const int motorPins_A[];
extern const int motorPins_B[];

// Motor PWM Configuration Definitions
extern const int MOTOR_PWM_Channel_A[];
extern const int MOTOR_PWM_Channel_B[];

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

const int PWM_FREQUENCY = 20000;
const int PWM_RESOLUTION = 8;

class LoRClass {
public:
    LoRClass(uint16_t stackSize = 2048, uint8_t priority = 0);
    

	void begin();

private:
    static void handleSerialInput(void* parameter);
    static const String targetWord;
    uint16_t stackSize;
    uint8_t priority;
};

extern LoRClass LoR;

#endif // LOR_H


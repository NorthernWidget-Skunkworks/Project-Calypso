/*****************************************************************************
turbidity_logging.ino
Written By:  Aaron Nightingale (night030@umn.edu)
Development Environment: PlatformIO 3.2.1
Hardware Platform: EnviroDIY Mayfly Arduino Datalogger
Software License: BSD-3.
  Copyright (c) 2017, Stroud Water Research Center (SWRC)
  and the EnviroDIY Development Team

This sketch is an example of logging turbidity data to an SD card.
For use with the turbidity sensor from the 2018 EE senior design.

DISCLAIMER:
THIS CODE IS PROVIDED "AS IS" - NO WARRANTY IS GIVEN.
*****************************************************************************/

// Some define statements
#define STANDARD_SERIAL_OUTPUT Serial  // Without this there will be no output

// ==========================================================================
//    Include the base required libraries
// ==========================================================================
#include <Arduino.h>  // The base Arduino library
#include <EnableInterrupt.h>  // for external and pin change interrupts
#include <LoggerBase.h>


// ==========================================================================
//    Basic Logger Settings
// ==========================================================================
// The name of this file
const char *sketchName = "turbidity_logging.ino";

// Logger ID, also becomes the prefix for the name of the data file on SD card
const char *LoggerID = "XXXXX";
// How frequently (in minutes) to log data
const float loggingInterval = 10.0;  //USE 15 or larger!!
// Your logger's timezone.
const int timeZone = -5;
// Create a new logger instance
Logger logger;


// ==========================================================================
//    Primary Arduino-Based Board and Processor
// ==========================================================================
#include <ProcessorStats.h>

const long serialBaud = 57600;  // Baud rate for the primary serial port for debugging
const int greenLED = 8;  // Pin for the green LED (-1 if unconnected)
const int redLED = 9;  // Pin for the red LED (-1 if unconnected)
const int buttonPin = 21;  // Pin for a button to use to enter debugging mode (-1 if unconnected)
const int wakePin = A7;  // Interrupt/Alarm pin to wake from sleep
// Set the wake pin to -1 if you do not want the main processor to sleep.
// In a SAMD system where you are using the built-in rtc, set wakePin to 1
const int sdCardPin = 12;  // SD Card Chip Select/Slave Select Pin (must be defined!)

// Create the processor "sensor"
const char *MFVersion = "v0.5";
ProcessorStats mayfly(MFVersion) ;


// ==========================================================================
//    Maxim DS3231 RTC (Real Time Clock)
// ==========================================================================
#include <MaximDS3231.h>
MaximDS3231 ds3231(1);


// ==========================================================================
//    Turbidity Sensor Sr. Design (Forward Scatter, Side Scatter, NTU)
// ==========================================================================
#include "Turbidity.h"
#include "TurbidityHelper.h"
uint8_t Turbidityi2c_addr = 0x08;
const int I2CPower = 22;  // Pin to switch power on and off (-1 if unconnected)
Turbidity       ntu01(I2CPower, Turbidityi2c_addr);
TurbidityHelper ntu02(I2CPower, Turbidityi2c_addr);

// ==========================================================================
//    The array that contains all variables to be logged
// ==========================================================================
Variable *variableList[] = {
    new ProcessorStats_Batt(&mayfly),
    new ProcessorStats_FreeRam(&mayfly),
    new MaximDS3231_Temp(&ds3231),
    new Turbidity_FS(&ntu01),
    new Turbidity_SS(&ntu01),
    new Turbidity_DARK(&ntu01),
    new Turbidity_NTU(&ntu01),
    new TurbidityHelper_FS(&ntu02),
    new TurbidityHelper_SS(&ntu02),
    new TurbidityHelper_DARK(&ntu02),
    // new YOUR_variableName_HERE(&)
};
int variableCount = sizeof(variableList) / sizeof(variableList[0]);


// ==========================================================================
//    Working Functions
// ==========================================================================

// Flashes the LED's on the primary board
void greenredflash(int numFlash = 4, int rate = 75)
{
  for (int i = 0; i < numFlash; i++) {
    digitalWrite(greenLED, HIGH);
    digitalWrite(redLED, LOW);
    delay(rate);
    digitalWrite(greenLED, LOW);
    digitalWrite(redLED, HIGH);
    delay(rate);
  }
  digitalWrite(redLED, LOW);
}


// ==========================================================================
// Main setup function
// ==========================================================================
void setup()
{
    // Start the primary serial connection
    Serial.begin(serialBaud);
    
    // Set up pins for the LED's
    pinMode(greenLED, OUTPUT);
    pinMode(redLED, OUTPUT);
    // Blink the LEDs to show the board is on and starting up
    greenredflash();

    // Print a start-up note to the first serial port
    Serial.print(F("Now running "));
    Serial.print(sketchName);
    Serial.print(F(" on Logger "));
    Serial.println(LoggerID);

    // Set the timezone and offsets
    // Logging in the given time zone
    Logger::setTimeZone(timeZone);
    // Offset is the same as the time zone because the RTC is in UTC
    Logger::setTZOffset(timeZone);

    // Initialize the logger
    logger.init(sdCardPin, wakePin, variableCount, variableList,
                loggingInterval, LoggerID);
    logger.setAlertPin(greenLED);

    // Begin the logger
    logger.begin();

    // Check for debugging mode
    logger.checkForTestingMode(buttonPin);
}


// ==========================================================================
// Main loop function
// ==========================================================================
void loop()
{
    // Log the data
    logger.log();
}

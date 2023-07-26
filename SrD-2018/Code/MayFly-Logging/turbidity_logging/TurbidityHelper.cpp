/*
 *Turbidity.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Code written by Aaron Nightingale (night030@umn.edu)
 *
*/

#include "TurbidityHelper.h"

// The constructor - because this is I2C, only need the power pin
TurbidityHelper::TurbidityHelper(int powerPin, uint8_t i2cAddressHex, int measurementsToAverage)
     : Sensor(F("Turbidity"), TURBIDITY_HELPER_NUM_VARIABLES,
              TURBIDITY_HELPER_WARM_UP_TIME_MS, TURBIDITY_HELPER_STABILIZATION_TIME_MS, TURBIDITY_HELPER_MEASUREMENT_TIME_MS,
              powerPin, -1, measurementsToAverage)
{
    _i2cAddressHex  = i2cAddressHex;
}


String TurbidityHelper::getSensorLocation(void)
{
    String address = F("I2C_0x");
    address += String(_i2cAddressHex, HEX);
    return address;
}


SENSOR_STATUS TurbidityHelper::getStatus(void)
{
    // Check if the power is on, turn it on if not (Need power to get status)
    bool wasOn = checkPowerOn();
    if(!wasOn){powerUp();}
    // Wait until the sensor is warmed up
    waitForWarmUp();

    // Run begin fxn because it returns true or false for success in contact
    delay(10); // let the sensor settle in after power-up
    
    bool status = sensorPresent();
    
    delay(100); // And now let the sensor boot up (time cannot be decreased)

    // Turn the power back off it it had been turned on
    if(!wasOn){powerDown();}

    if (!status) return SENSOR_ERROR;
    else return SENSOR_READY;
}


SENSOR_STATUS TurbidityHelper::setup(void)
{
    Sensor::setup();
    return getStatus();
}


bool TurbidityHelper::wake(void)
{
    Sensor::wake();
    waitForWarmUp();
    
    delay(100); // And now let the sensor boot up (time cannot be decreased)
    return true;
}


bool TurbidityHelper::addSingleMeasurementResult(void)
{   
    // Make sure we've waited long enough for a new reading to be available
    waitForMeasurementCompletion();
    
    //I2C function here that grabs data from sensor
    //sensorRead();
    
    // Copy measurements to data array
    
    sensorValues[TURBIDITY_HELPER_FS_VAR_NUM]   += response[RES_TRAN2];
    sensorValues[TURBIDITY_HELPER_SS_VAR_NUM]   += response[RES_SCAT2];
    sensorValues[TURBIDITY_HELPER_DARK_VAR_NUM] += response[RES_DARK2];
    
    // Return true when finished
    return true;
}

//Copied from I2C Master
bool TurbidityHelper::sensorPresent()
{
  //Check for a sensor on the bus
  Wire.beginTransmission(_i2cAddressHex);
  if (Wire.endTransmission() == 0) {
    Serial.println("Sensor #" + (String)_i2cAddressHex + " found.");
    return true;
  } else {
    Serial.println("ERROR: Sensor #" + (String)_i2cAddressHex + " not found.");
    return false;
  }
}

void TurbidityHelper::sensorRead()
{
  //testing reading floats from slave
  byte * ptr = (byte *)response;
  if(sensorPresent()) {
    Wire.requestFrom(_i2cAddressHex, (DATA_LEN * 4));
    int i = 0;
    while (Wire.available()) {  // slave may send less than requested
      if(i < (DATA_LEN * 4)) {
        *(ptr + i) = Wire.read();
      }
      i++;
    }
  } else {
    //Fill the sensor data with defaults
    for(int i=0; i<DATA_LEN; i++) {
      response[i] = 1.111 * (i+1);
    }
  }
}


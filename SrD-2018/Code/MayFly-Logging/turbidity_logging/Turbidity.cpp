/*
 *Turbidity.cpp
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Code written by Aaron Nightingale (night030@umn.edu)
 *
*/

#include "Turbidity.h"

// The constructor - because this is I2C, only need the power pin
Turbidity::Turbidity(int powerPin, uint8_t i2cAddressHex, int measurementsToAverage)
     : Sensor(F("Turbidity"), TURBIDITY_NUM_VARIABLES,
              TURBIDITY_WARM_UP_TIME_MS, TURBIDITY_STABILIZATION_TIME_MS, TURBIDITY_MEASUREMENT_TIME_MS,
              powerPin, -1, measurementsToAverage)
{
    _i2cAddressHex  = i2cAddressHex;
}


String Turbidity::getSensorLocation(void)
{
    String address = F("I2C_0x");
    address += String(_i2cAddressHex, HEX);
    return address;
}


SENSOR_STATUS Turbidity::getStatus(void)
{
    // Check if the power is on, turn it on if not (Need power to get status)
    bool wasOn = checkPowerOn();
    if(!wasOn){powerUp();}
    // Wait until the sensor is warmed up
    waitForWarmUp();
    
    bool status = sensorPresent();
    
    // Turn the power back off it it had been turned on
    if(!wasOn){powerDown();}

    if (!status) return SENSOR_ERROR;
    else return SENSOR_READY;
}


SENSOR_STATUS Turbidity::setup(void)
{
    Sensor::setup();
    //pinMode(_busyPin, INPUT);
    return getStatus();
}


bool Turbidity::wake(void)
{
    Sensor::wake();
    waitForWarmUp();
    return true;
}


bool Turbidity::addSingleMeasurementResult(void)
{
    // Make sure we've waited long enough for a new reading to be available
    waitForMeasurementCompletion();
    
	  //I2C function here that grabs data from sensor
	  sensorRead();

    // Copy measurements to data array
    
    sensorValues[TURBIDITY_FS_VAR_NUM]   += response[RES_TRAN1];
    sensorValues[TURBIDITY_SS_VAR_NUM]   += response[RES_SCAT1];
    sensorValues[TURBIDITY_DARK_VAR_NUM] += response[RES_DARK1];
    sensorValues[TURBIDITY_NTU_VAR_NUM]  += calculate_turbidity(response);
    

    //Start taking measurements for the next logging entry
    updateCommand();

    //Wait 8 minutes for the measurement to complete
    //  This can be replaced with checking the busy pin.
    for(int i=0; i<60*8; i++) {
      delay(1000);
    }
    
    // Return true when finished
    return true;
}

//Copied from I2C Master
bool Turbidity::sensorPresent()
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

void Turbidity::sensorRead()
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

void Turbidity::sensorWrite()
{
  command[COMMAND_LEN - 1] = 0;
  Wire.beginTransmission(_i2cAddressHex); // transmit to device #8
  for(int i=0; i<COMMAND_LEN; i++) {
    Wire.write(command[i]);
  }  
  Wire.endTransmission();    // stop transmitting
}

void Turbidity::updateCommand() {
  command[0] = 'U';
  command[1] = 'P';
  command[2] = 'D';
  command[3] = 'A';
  command[4] = 'T';
  command[5] = 'E';
  command[6] = 0;
  sensorWrite();
}

//This function can calculate the turbidity using 
//     the values in the response array.
float Turbidity::calculate_turbidity(float data[6])
{
  float L1T = 0, L1S = 0, L2T = 0, L2S = 0, NTU = 0, temp = 0;
  L1T = data[2]-data[0]; 
  L1S = data[3]-data[1];
  L2T = data[4]-data[1];
  L2S = data[5]-data[0];

  if(L1S < 5000)
  {
    NTU = pow(10, ((L1S-4884.51015963563))/(217.615730769231));
  }
  else if(L1S < 8000)
  {
    NTU = (L1S-4723.4758127632)/(112.799965681699);
  }
  else if(L1S < 55000)
  {
    temp = (L1S-4723.4758127632)/(112.799965681699);
    temp += exp((L1T-905841.461548272)/(-108472.245937816));
    temp += (L2S-6696.20138888889)/(79.3873036786787);
    temp += 2*(exp((L2T-601607.276911029)/(-71291.2384864573)));
    NTU = temp/5;
  }
  else
  {
    temp += exp((L1T-905841.461548272)/(-108472.245937816));
    temp += 2*(exp((L2T-601607.276911029)/(-71291.2384864573)));
    NTU = temp/3;
  }

  return NTU;
}


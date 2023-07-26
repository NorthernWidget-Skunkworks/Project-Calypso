/*
 *Turbidity.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Code written by Aaron Nightingale (night030@umn.edu)
 *
*/

#ifndef TurbidityHelper_h
#define TurbidityHelper_h

#include "SensorBase.h"
#include "VariableBase.h"
#include <Wire.h>

// #define DEBUGGING_SERIAL_OUTPUT Serial
#include "ModSensorDebugger.h"

#define TURBIDITY_HELPER_NUM_VARIABLES 3
#define TURBIDITY_HELPER_WARM_UP_TIME_MS 100      
#define TURBIDITY_HELPER_STABILIZATION_TIME_MS 0
#define TURBIDITY_HELPER_MEASUREMENT_TIME_MS 1000

//Transmission variable
#define TURBIDITY_HELPER_FS_RESOLUTION 2  
#define TURBIDITY_HELPER_FS_VAR_NUM 0

//Side Scatter variable
#define TURBIDITY_HELPER_SS_RESOLUTION 2
#define TURBIDITY_HELPER_SS_VAR_NUM 1

//Dark Counts Variable
#define TURBIDITY_HELPER_DARK_RESOLUTION 2
#define TURBIDITY_HELPER_DARK_VAR_NUM 2

//I2C data transfer parameters
#define DATA_LEN    6

//Key for the Response Array
#define RES_DARK1 0
#define RES_TRAN1 2
#define RES_SCAT1 3
#define RES_DARK2 1
#define RES_TRAN2 4
#define RES_SCAT2 5

// The main class for the Turbidity sensor
class TurbidityHelper : public Sensor
{
public:
  TurbidityHelper(int powerPin, uint8_t i2cAddressHex = 0x08, int measurementsToAverage = 1);

  bool wake(void) override;
  SENSOR_STATUS setup(void) override;
  SENSOR_STATUS getStatus(void) override;
  String getSensorLocation(void) override;
  bool addSingleMeasurementResult(void) override;
  
  //Functions copied from I2C Master
  bool sensorPresent();
  void sensorRead();
  
protected:
    uint8_t _i2cAddressHex;
    float response[DATA_LEN];
};


// Defines the Forward Scatter Variable
class TurbidityHelper_FS : public Variable
{
public:
    TurbidityHelper_FS(Sensor *parentSense,
                     String UUID = "", String customVarCode = "")
      : Variable(parentSense, TURBIDITY_HELPER_FS_VAR_NUM,
               F("Transmission 2"), F("Hz"),
               TURBIDITY_HELPER_FS_RESOLUTION,
               F("TurbidityT2"), UUID, customVarCode)
    {}
};


// Defines the Side Scatter Variable
class TurbidityHelper_SS : public Variable
{
public:
    TurbidityHelper_SS(Sensor *parentSense,
                         String UUID = "", String customVarCode = "")
      : Variable(parentSense, TURBIDITY_HELPER_SS_VAR_NUM,
               F("Side Scatter 2"), F("Hz"),
               TURBIDITY_HELPER_SS_RESOLUTION,
               F("TurbiditySS2"), UUID, customVarCode)
    {}
};

class TurbidityHelper_DARK : public Variable
{
public:
    TurbidityHelper_DARK(Sensor *parentSense,
                         String UUID = "", String customVarCode = "")
      : Variable(parentSense, TURBIDITY_HELPER_DARK_VAR_NUM,
               F("Dark 2"), F("Hz"),
               TURBIDITY_HELPER_DARK_RESOLUTION,
               F("TurbidityD2"), UUID, customVarCode)
    {}
};

#endif

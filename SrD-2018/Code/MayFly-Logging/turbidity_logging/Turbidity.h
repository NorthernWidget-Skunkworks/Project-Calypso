/*
 *Turbidity.h
 *This file is part of the EnviroDIY modular sensors library for Arduino
 *
 *Code written by Aaron Nightingale (night030@umn.edu)
 *
*/

#ifndef Turbidity_h
#define Turbidity_h

#include "SensorBase.h"
#include "VariableBase.h"
#include <Wire.h>

// #define DEBUGGING_SERIAL_OUTPUT Serial
#include "ModSensorDebugger.h"


#define TURBIDITY_NUM_VARIABLES 4
#define TURBIDITY_WARM_UP_TIME_MS 100			//TO DO: Take a look at these times to make sure they're right
#define TURBIDITY_STABILIZATION_TIME_MS 0
#define TURBIDITY_MEASUREMENT_TIME_MS 1000

//Forward scatter variable
#define TURBIDITY_FS_RESOLUTION 2				
#define TURBIDITY_FS_VAR_NUM 0

//Side Scatter variable
#define TURBIDITY_SS_RESOLUTION 2
#define TURBIDITY_SS_VAR_NUM 1

//Calculated turbidity variable
#define TURBIDITY_DARK_RESOLUTION 2
#define TURBIDITY_DARK_VAR_NUM 2

//Calculated turbidity variable
#define TURBIDITY_NTU_RESOLUTION 2
#define TURBIDITY_NTU_VAR_NUM 3

//I2C data transfer parameters
#define DATA_LEN    6
#define COMMAND_LEN 7

//Key for the Response Array
#define RES_DARK1 0   // Sensor #1 Dark Counts
#define RES_TRAN1 2   // LED #1 Transmission
#define RES_SCAT1 3   // LED #1 Side Scatter
#define RES_DARK2 1   // Sensor #2 Dark Counts
#define RES_TRAN2 4   // LED #2 Transmission
#define RES_SCAT2 5   // LED #2 Side Scatter

// The main class for the Turbidity sensor
class Turbidity : public Sensor
{
public:
  Turbidity(int powerPin, uint8_t i2cAddressHex = 0x08, int measurementsToAverage = 1);

  bool wake(void) override;
  SENSOR_STATUS setup(void) override;
  SENSOR_STATUS getStatus(void) override;
  String getSensorLocation(void) override;
	bool addSingleMeasurementResult(void) override;
	
	//Functions copied from I2C Master
	bool sensorPresent();
	void sensorRead();
	void sensorWrite();
  void updateCommand();
	float calculate_turbidity(float data[6]);

protected:
  uint8_t _i2cAddressHex;
  char command[COMMAND_LEN];
  float response[DATA_LEN];
};


// Defines the Forward Scatter Variable
class Turbidity_FS : public Variable
{
public:
    Turbidity_FS(Sensor *parentSense,
                     String UUID = "", String customVarCode = "")
      : Variable(parentSense, TURBIDITY_FS_VAR_NUM,
               F("Transmission 1"), F("Hz"),
               TURBIDITY_FS_RESOLUTION,
               F("TurbidityT1"), UUID, customVarCode)
    {}
};


// Defines the Side Scatter Variable
class Turbidity_SS : public Variable
{
public:
    Turbidity_SS(Sensor *parentSense,
                         String UUID = "", String customVarCode = "")
      : Variable(parentSense, TURBIDITY_SS_VAR_NUM,
               F("Side Scatter 1"), F("Hz"),
               TURBIDITY_SS_RESOLUTION,
               F("TurbiditySS1"), UUID, customVarCode)
    {}
};


// Defines the NTU Variable
class Turbidity_DARK : public Variable
{
public:
    Turbidity_DARK(Sensor *parentSense,
                         String UUID = "", String customVarCode = "")
      : Variable(parentSense, TURBIDITY_DARK_VAR_NUM,
               F("Dark 1"), F("Hz"),
               TURBIDITY_DARK_RESOLUTION,
               F("TurbidityD1"), UUID, customVarCode)
    {}
};

// Defines the NTU Variable
class Turbidity_NTU : public Variable
{
public:
    Turbidity_NTU(Sensor *parentSense,
                         String UUID = "", String customVarCode = "")
      : Variable(parentSense, TURBIDITY_NTU_VAR_NUM,
               F("NTU 1"), F("NTU"),
               TURBIDITY_NTU_RESOLUTION,
               F("TurbidityNTU1"), UUID, customVarCode)
    {}
};

#endif

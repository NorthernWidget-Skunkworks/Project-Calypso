//I2C Master
//
//Taken from: https://www.arduino.cc/en/Tutorial/MasterReader
#include <Wire.h>

//Slave address
#define SLAVE_ADDR  8
#define DATA_LEN    6
#define COMMAND_LEN 7

//Key for the Response Array
#define RES_DARK1 0
#define RES_TRAN1 2
#define RES_SCAT1 3
#define RES_DARK2 1
#define RES_TRAN2 4
#define RES_SCAT2 5

//CUSTOM I2C FUNCTIONS
char command[COMMAND_LEN];
float response[DATA_LEN];   

void setup() {
  Wire.begin();        // join i2c bus (address optional for master)
  Serial.begin(9600);  // start serial for output
  sensorPresent(SLAVE_ADDR);
}

void loop() {
  sensorRead();
  Serial.println("Current sensor status:");
  //dumpSensorData();
  dumpFormattedData();
  
  getSerialCommand();
  
  Serial.println((String)"Sending command: " + command + ".");
  sensorWrite();
  
  //Adding delay for debugging (8min delay)
  for(int i=0; i<60*8; i++) {
    delay(1000);
  }
  
  Serial.println(" ");
  delay(500);
}

void getSerialCommand() {
  char dummy;
  Serial.println("Enter a command: (Type 'UPDATE' and press enter.)");

  //Get command characters from serial, flush extra
  while(Serial.available() < (COMMAND_LEN - 1));
  for(int x = 0; x < COMMAND_LEN; x++) {
    if(Serial.available() > 0) {
      // read the incoming byte:
      command[x] = Serial.read();
    }
  }
  delay(100);
  while(Serial.available() > 0) {
    dummy = Serial.read();
  }
  command[COMMAND_LEN - 1] = 0;
}

bool sensorPresent(int addr) {
  //Check for a sensor on the bus
  Wire.beginTransmission(addr);
  if (Wire.endTransmission() == 0) {
    Serial.println("Sensor #" + (String)addr + " found.");
    return true;
  } else {
    Serial.println("ERROR: Sensor #" + (String)addr + " not found.");
    return false;
  }
}

void sensorRead() {  
  //testing reading floats from slave
  byte * ptr = (byte *)response;
  if(sensorPresent(SLAVE_ADDR)) {
    Wire.requestFrom(SLAVE_ADDR, (DATA_LEN * 4));
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

void sensorWrite() {
  command[COMMAND_LEN - 1] = 0;
  Wire.beginTransmission(SLAVE_ADDR); // transmit to device #8
  for(int i=0; i<COMMAND_LEN; i++) {
    Wire.write(command[i]);
  }  
  Wire.endTransmission();    // stop transmitting
}

void debug(int i) {
  Serial.println("DEBUG: " + (String)i);
}

void dumpSensorData() {
  for(int i=0; i<DATA_LEN; i++) {
    Serial.println(response[i]);
  }
}

//This function can calculate the turbidity using 
//     the values in the response array.
float calculate_turbidity()
{
  float L1T = 0, L1S = 0, L2T = 0, L2S = 0, NTU = 0, temp = 0;
  L1T = response[2]-response[0]; 
  L1S = response[3]-response[1];
  L2T = response[4]-response[1];
  L2S = response[5]-response[0];

  if(L1S < 5000)
  {
    NTU = pow(10, ((L1S-4884.51015963563))/(217.615730769231));
  }
  else if(L1S < 8000)
  {
    Serial.println("debug #2");
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
    Serial.println("debug #4");
    temp += exp((L1T-905841.461548272)/(-108472.245937816));
    temp += 2*(exp((L2T-601607.276911029)/(-71291.2384864573)));
    NTU = temp/3;
  }

  return NTU;
}

void dumpFormattedData() {
  Serial.println((String)"Sensor #1 Dark Counts: " + response[RES_DARK1] + " Hz");
  Serial.println((String)"Sensor #2 Dark Counts: " + response[RES_DARK2] + " Hz");
  Serial.println((String)"LED #1 Transmission:   " + response[RES_TRAN1] + " Hz");
  Serial.println((String)"LED #1 Side Scatter:   " + response[RES_SCAT1] + " Hz");
  Serial.println((String)"LED #2 Transmission:   " + response[RES_TRAN2] + " Hz");
  Serial.println((String)"LED #2 Side Scatter:   " + response[RES_SCAT2] + " Hz");
  
  float ntu = calculate_turbidity();
  Serial.println('.');
  if(ntu <= 0.01) {
    Serial.println((String)"Turbidity:             " + ntu*1000.0 + "E-3 NTU");
  } else {
    Serial.println((String)"Turbidity:             " + ntu + " NTU");
  }
  Serial.println((String)"--------------------------------------------------------");
}

//NOTES:
//Do not power master while slave is connected and off.
//  This causes the I2C to break...
 

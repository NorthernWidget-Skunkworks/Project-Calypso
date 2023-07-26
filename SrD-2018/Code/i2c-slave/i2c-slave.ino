//I2C SLAVE
//
//Taken from: https://www.arduino.cc/en/Tutorial/MasterReader
//
//Response Byte Map
//Bytes 0 - 3: Forward Scatter. Byte 0 is MSB.
//Bytes 4 - 7: Side Scatter. Byte 4 is MSB.
//
#include <Wire.h>

//Slave address
#define SLAVE_ADDR  8
#define DATA_LEN    6
#define COMMAND_LEN 7

//CUSTOM I2C FUNCTIONS
char command[COMMAND_LEN];
float response[DATA_LEN];  

//CUSTOM I2C COMMANDS - (Working w strcmp() )
const char UPDATE[] = {'U','P','D','A','T','E',0};
const char MODE01[] = {'M','O','D','E','0','1',0};
const char MODE02[] = {'M','O','D','E','0','2',0};
const char MODE03[] = {'M','O','D','E','0','3',0};
const char PING_Z[] = {'P','I','N','G',' ','Z',0};
const char PING_Q[] = {'P','I','N','G',' ','Q',0};


void setup() {
  Wire.begin(SLAVE_ADDR);       // join i2c bus with address
  Wire.onRequest(requestEvent); // register event
  Wire.onReceive(receiveEvent); // register event

  //Fill the response with defaults
  for(int i=0; i<DATA_LEN; i++) {
    response[i] = 1.111 * (i+1);
  }
}

void loop() {
  delay(100);
}

// function that executes whenever data is requested by master
// this function is registered as an event, see setup()
// sends floats byte by byte
void requestEvent() {
    byte * ptr = (byte *)response;
    for(int i=0; i<(DATA_LEN * 4); i++) {
      Wire.write(*(ptr + i));
    }
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany) {
  char c;
  int i = 0;
  while (0 < Wire.available()) {  // loop through all bytes
    char c = Wire.read();         // receive byte as a character
    if(i < COMMAND_LEN) {
      command[i] = c;
    }
    i++;
  }

  //Handling different commands
  if(strcmp(command,UPDATE) == 0) {
    for(int x=0; x<DATA_LEN; x++) {
      response[x] = (float)response[x] + (float)1.0; //Adds two for some reason...
    }
  }
  if(strcmp(command,MODE01) == 0) {

  }
  if(strcmp(command,MODE02) == 0) {

  }
  if(strcmp(command,MODE03) == 0) {

  }
  if(strcmp(command,PING_Z) == 0) {
    response[0] = 6.66;
  }
  if(strcmp(command,PING_Q) == 0) {
    response[0] = 2.22;
  }
}

// TO DO:
// More testing on command interface.

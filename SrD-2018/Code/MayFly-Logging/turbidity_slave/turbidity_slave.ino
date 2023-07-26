// I2C Turbidity Slave
// Senior Design Project - Spring 2018
// Written By: Corbin Buechler & Aaron Nightingale
//
// I2C Slave code based on: 
//     https://www.arduino.cc/en/Tutorial/MasterReader
//

#include <Wire.h>

// I2C DEFINITIONS //
//Slave address
#define SLAVE_ADDR  8
#define DATA_LEN    6
#define COMMAND_LEN 7

//CUSTOM I2C FUNCTIONS
char command[COMMAND_LEN];
//Note: response array is called "result[]"! 

//Commands
const char UPDATE[] = {'U','P','D','A','T','E',0};
const char MODE01[] = {'M','O','D','E','0','1',0};
const char MODE02[] = {'M','O','D','E','0','2',0};
const char MODE03[] = {'M','O','D','E','0','3',0};
const char PING_Z[] = {'P','I','N','G',' ','Z',0};
const char PING_Q[] = {'P','I','N','G',' ','Q',0};

// END I2C DEFINITIONS //

//Pin Definitions 
#define SENSOR1 2
#define SENSOR2 3
#define L1_HIGH 5
#define L1_OFF 6
#define L2_HIGH 7
#define L2_OFF 8
#define BUSY 13
#define UNUSED_PIN 12 //ATmega pin 18, white wire in cable

#define TYPES 6
#define REPEATS 5

volatile uint8_t go;
volatile uint32_t count; 
uint8_t test;
float measurements[TYPES][REPEATS], result[TYPES];

bool needsUpdate;

void setup() 
{
  //Debugging
  needsUpdate = false;
  
  Serial.begin(9600);
  pinMode(SENSOR1, INPUT_PULLUP);
  pinMode(SENSOR2, INPUT_PULLUP);
  pinMode(L1_OFF, OUTPUT);
  digitalWrite(L1_OFF, HIGH);
  pinMode(L1_HIGH, OUTPUT);
  digitalWrite(L1_HIGH, LOW);
  pinMode(L2_OFF, OUTPUT);
  digitalWrite(L2_OFF, HIGH);
  pinMode(L2_HIGH, OUTPUT);
  digitalWrite(L2_HIGH, LOW);
  pinMode(BUSY, OUTPUT);
  digitalWrite(BUSY, LOW);

  test = 0;
  count = 0;

  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;
  OCR1A = 31249;
  TCCR1B |= (1 << WGM12);
  TCCR1B |= (1 << CS12);  
  TIMSK1 |= (1 << OCIE1A);
  
  //Set up the I2C interface
  Wire.begin(SLAVE_ADDR);       // join i2c bus with address
  Wire.onRequest(requestEvent); // register event
  Wire.onReceive(receiveEvent); // register event
}

ISR(TIMER1_COMPA_vect)
{
   go--;
}

void freq_counter()
{
  count++;
}

void LED1_OFF()
{
  digitalWrite(L1_OFF, HIGH);
}

void LED1_LOW()
{
  if(digitalRead(L1_HIGH) == HIGH) digitalWrite(L1_HIGH, LOW);
  digitalWrite(L1_OFF, LOW);
}

void LED1_HIGH()
{
  if(digitalRead(L1_HIGH) == LOW) digitalWrite(L1_HIGH, HIGH);
  digitalWrite(L1_OFF, LOW);
}

void LED2_OFF()
{
  digitalWrite(L2_OFF, HIGH);
}

void LED2_LOW()
{
  if(digitalRead(L2_HIGH) == HIGH) digitalWrite(L2_HIGH, LOW);
  digitalWrite(L2_OFF, LOW);
}

void LED2_HIGH()
{
  if(digitalRead(L2_HIGH) == LOW) digitalWrite(L2_HIGH, HIGH);
  digitalWrite(L2_OFF, LOW);
}

uint32_t SENSE1(uint8_t seconds)
{
  count = 0;
  TCNT1  = 0;
  attachInterrupt(digitalPinToInterrupt(SENSOR1), freq_counter, RISING);
  go = seconds;
  while(go);
  detachInterrupt(digitalPinToInterrupt(SENSOR1));
  return count;
}

uint32_t SENSE2(uint8_t seconds)
{ 
  count = 0;
  TCNT1  = 0;
  attachInterrupt(digitalPinToInterrupt(SENSOR2), freq_counter, RISING);
  go = seconds;
  while(go);
  detachInterrupt(digitalPinToInterrupt(SENSOR2));
  return count;
}

float READ_SENSOR1()
{
  uint8_t seconds = 1;
  uint32_t pulses = 0;
  float freq = 0;
  
  pulses = SENSE1(seconds);
  if(pulses < 3)
  {
    seconds = 120;
    pulses = SENSE1(seconds);
  }
  else if(pulses < 100)
  {
    seconds = (100/(pulses-2)) + 1;
    pulses = SENSE1(seconds);
  }
  freq = (((float)pulses*8)/seconds);
  return freq;
}

float READ_SENSOR2()
{
  uint8_t seconds = 1;
  uint32_t pulses = 0;
  float freq = 0;

  pulses = SENSE2(seconds);
  if(pulses < 3)
  {
    seconds = 120;
    pulses = SENSE2(seconds);
  }
  else if(pulses < 100)
  {
    seconds = (100/(pulses-2)) + 1;
    pulses = SENSE2(seconds);
  }
  freq = (((float)pulses*8)/seconds);
  return freq;
}

float SENSOR1_DARK()
{
  float output = 0;
  output = READ_SENSOR1();
  return output;
}

float SENSOR2_DARK()
{
  float output = 0;
  output = READ_SENSOR2();
  return output;
}

float LED1_TRANSMISSION()
{
  float output = 0;
  LED1_LOW();
  delay(10);
  output = READ_SENSOR1();  
  LED1_OFF();
  return output;
}

float LED1_SCATTER()
{
  float output = 0;
  LED1_HIGH();
  delay(10);
  output = READ_SENSOR2();
  LED1_OFF();
  return output;
}

float LED2_TRANSMISSION()
{
  float output = 0;
  LED2_LOW();
  delay(10);
  output = READ_SENSOR2();
  LED2_OFF();
  return output;
}

float LED2_SCATTER()
{
  float output = 0;
  LED2_HIGH();
  delay(10);
  output = READ_SENSOR1();
  LED2_OFF();
  return output;
}

void measure()
{
  measurements[0][0] = SENSOR1_DARK();
  measurements[1][0] = SENSOR2_DARK();
  for(uint8_t entry = 0; entry < REPEATS; entry++)
  { 
    for(uint8_t type = 2; type < TYPES; type++)
    {
      switch(type)
      {
        case 2:
          measurements[type][entry] = LED1_TRANSMISSION();
          break;
        case 3:
          measurements[type][entry] = LED1_SCATTER();
          break;
        case 4:
          measurements[type][entry] = LED2_TRANSMISSION();
          break;
        case 5:
          measurements[type][entry] = LED2_SCATTER();
          break;
      }
    }
  }
  measurements[0][1] = SENSOR1_DARK();
  measurements[1][1] = SENSOR2_DARK();
}

void average()
{
  float avg = 0;
  result[0] = (measurements[0][0]+measurements[0][1])/2;
  result[1] = (measurements[1][0]+measurements[1][1])/2;
  for(uint8_t type = 2; type < TYPES; type++)
  {
    for(uint8_t entry = 0; entry < REPEATS; entry++)
    {
      avg += measurements[type][entry];
    }
    result[type] = avg/5;
    avg = 0;
  }
}

// CODE FOR I2C INTERFACE //
// function that executes whenever data is requested by master
// this function is registered as an event, see setup()
// sends result array byte by byte
void requestEvent() {
    byte * ptr = (byte *)result;
    for(int i=0; i<(DATA_LEN * 4); i++) {
      Wire.write(*(ptr + i));
    }
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
// when the "UPDATE" command is received over I2C, a new 
//		measurement is taken.
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
  	//Take a new reading and save it
    needsUpdate = true;
    //OLD CODE
    //digitalWrite(BUSY, HIGH);
    //FIX ME
  	//measure();
  	//average();
    //delay(100);
  	//digitalWrite(BUSY, LOW);
  }
  if(strcmp(command,MODE01) == 0) {
	//Implement your own commands like so...	
  }
  if(strcmp(command,MODE02) == 0) {

  }
  if(strcmp(command,MODE03) == 0) {

  }
  if(strcmp(command,PING_Z) == 0) {
  	//Used for testing only
    result[0] = 6.66;
  }
  if(strcmp(command,PING_Q) == 0) {
  	//Used for testing only
    result[0] = 2.22;
  }
}
// END CODE FOR I2C INTERFACE //

void loop() 
{
  //only keep this 
  delay(1);
  if(needsUpdate) {
    digitalWrite(BUSY, HIGH);
    measure();
    average();
    delay(100);
    digitalWrite(BUSY, LOW);
    needsUpdate = false;
  }
}



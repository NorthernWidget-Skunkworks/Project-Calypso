#include <Wire.h>
#include <SlowSoftI2CMaster.h>
#define LED1 0  //Define values of LEDs for side indication
#define LED2 1

#define URES 4 //uReset on D4
#define LED1_CTRL 14 //LED 1 control pin on D14
#define LED2_CTRL 13 //LED 2 control pin on D13
#define DATA 5 //Data line to counter on D5
#define CLK 7 //Clock line to counter of D0
#define CLEAR 6 //Clear line to counter on D7
#define DONE 11 //Int line to comparator on D1 //FIX! Change name to INT??
#define F_IN_SW 3 //Input switch on D3
#define THERM_PIN A3 //Thermistor input on ADC3
#define SDA_SOFT PIN_C4
#define SCL_SOFT PIN_C5

#define ADR_DAC 0x60 //Address for DAC
#define ADR_SEL_PIN 15 //Digital pin 15 is used to test which device address should be used

#define VREF_VAL 0x000A //Set both referances to Vref (unbuffered)
#define SYS_STATUS_VAL
#define SYS_STATUS_REG 0x0A //Register which contains system status and gain info
#define VREF_REG 0x08 //Voltage referance set regitser 

#define LED0A 0x02
#define LED1A 0x04
#define COUNT 0x07
#define CTRL 0x00
#define TIME 0x09
#define SCALE 0x0B


#define READ 0x01
#define WRITE 0x00

uint16_t Res = 6800; //Resistance in mOhms of load resistor(s)
uint8_t Div = 4; //Division factor

uint8_t Config = 0; //Global config value

uint8_t Reg[22] = {0}; //Initialize registers

SlowSoftI2CMaster si = SlowSoftI2CMaster(SDA_SOFT, SCL_SOFT, true);  //Initialize software I2C  //FIX! Check pin values!

//FIX! Update values!
volatile uint8_t ADR = 0x40; //Use arbitraty address, change using generall call??
const uint8_t ADR_Alt = 0x41; //Alternative device address  //WARNING! When a #define is used instead, problems are caused

volatile bool StopFlag = false; //Used to indicate a stop condition 
volatile uint8_t RegID = 0; //Used to denote which register will be read from
volatile bool RepeatedStart = false; //Used to show if the start was repeated or not

volatile long TripTime = 0; //value used for measuring time of low pulse 
volatile bool NewData = false; //Used to test for new pulse data 

void setup() {
	Serial.begin(4800); //DEBUG!
	Reg[CTRL] = 0x00; //Set Config to POR value
	pinMode(LED1_CTRL, OUTPUT);
	pinMode(LED2_CTRL, OUTPUT);
	digitalWrite(LED1_CTRL, HIGH);
	digitalWrite(LED2_CTRL, HIGH); //Turn on LED switches 

	// pinMode(LED1_CTRL, OUTPUT);
	pinMode(DATA, OUTPUT);
	pinMode(CLK, OUTPUT);
	pinMode(CLEAR, OUTPUT);
	pinMode(URES, OUTPUT);
	pinMode(DONE, INPUT);
	pinMode(F_IN_SW, OUTPUT);
	digitalWrite(F_IN_SW, LOW); //default to input0
	digitalWrite(CLEAR, HIGH);
	pinMode(ADR_SEL_PIN, INPUT_PULLUP); 
	if(!digitalRead(ADR_SEL_PIN)) ADR = ADR_Alt; //If solder jumper is bridged, use alternate address //DEBUG!
  	Wire.begin(ADR);  //Begin slave I2C

  	// Wire.onAddrReceive(addressEvent); // register event
	Wire.onRequest(requestEvent);     // register event
	Wire.onReceive(receiveEvent);
	// Wire.onStop(stopEvent);

	si.i2c_init(); //Begin I2C master

	si.i2c_start((ADR_DAC << 1) | WRITE);  //Set voltage referance
	si.i2c_write((VREF_REG << 3) | WRITE); 
	si.i2c_write(VREF_VAL >> 8);  
	si.i2c_write(VREF_VAL & 0xFF);
	si.i2c_stop();

	attachInterrupt(digitalPinToInterrupt(11), Trip, CHANGE);
	//Set DAC Gain???? FIX!
}

void loop() {
	if((Reg[CTRL] & 0x80) == 0x80) {  //Test if Sample/rdy bit is set
		//Add sample procedure!
		//Set LEDs
		//Set Counter
		//Clear counter
		//Start timer
		//release counter
		Serial.print("SET");
		// noInterrupts(); //FIX!!
		// Serial.print(Reg[LED0A], HEX); //DEBUG!
		// Serial.print(Reg[LED0A + 1], HEX); //DEBUG!
		TripTime = 0; //Clear timing coutner 
		digitalWrite(URES, LOW); //Hold system in reset
		SetCurrent((Reg[LED0A + 1] << 8) | Reg[LED0A], 0); //Set LED0 to current 0
		SetCurrent((Reg[LED1A + 1] << 8) | Reg[LED1A], 1); //Set LED1 to current 1
		SetCount(Reg[COUNT]); //Set the count of the system
		digitalWrite(URES, HIGH); //Release counter, begin operation
		// interrupts(); //re-enable interrupts to get count //FIX!
		Reg[CTRL] = 0x00; //DEBUG! //Clear control reg
	}
	// Serial.println(Reg[CTRL]); //DEBUG!
	if(NewData) {
		//Load new data into registers
		//Set ready bit in control reg 
		NewData = false;
		long Temp = 0;
		Temp = TripTime/8; //Scale to rollovers
		Reg[TIME] = Temp & 0xFF; //LSB of time val
		Reg[TIME + 1] = (Temp >> 8) & 0xFF; //Middle byte of time val
		Reg[TIME + 2] = (Temp >> 16) & 0xFF; //MSB of time val
		Serial.println(Reg[TIME]); //DEBUG!
		Reg[CTRL] = 0x01; //FIX!!!! 
	}
	delay(100); //FIX! Keep?
}

//Current given in uA
int SetCurrent(long Current, bool Side) //Set current drive for led, use Side to set which LED to set, 0 for LED1, 1 for LED2
{
	//ADD VOLTAGE DIVIDER VALUE!!??
	//RETURN BAD VALUE IF SET OUTSIDE OF RANGE!??
	uint16_t Voltage = ((Current*Res)/500000)*Div; //Determine current set from load resistor 500 = 4096/2048000, bits/uV range
	// Serial.print("VOLT"); //DEBUG!
	// Serial.print(Voltage); //DEBUG!
	si.i2c_start((ADR_DAC << 1) | WRITE); 
	si.i2c_write((Side << 3) | 0b000); //Write to reg 0 vs 1 depending on which DAC you want to write to 
	si.i2c_write(Voltage >> 8); //Write LSB
	si.i2c_write(Voltage & 0xFF); //Write MSB
	si.i2c_stop();
	//FIX! Return???
}

void SetCount(uint8_t Val)
{	//FIX! REPLACE!!
// 	bool Dummy = false; 
// 	DDRB = DDRB | 0x01; //Set pin PB0 as output  //DEBUG!
// 	DDRA = DDRA | 0x28; //Set pin PA5 and pin PA3 as output  //DEBUG!
// 	PORTA = PORTA | 0x08; //Turn Clear Off
// 	PORTA = PORTA & 0xDF; //Clear PA5 
// 	// Val = Val << 0x05; //Offset to allign with PORTA
	for(int i = 7; i >= 0; i--) {
// 		//FAST MODE
// 		PORTB = PORTB & 0xFE; //Clear bit, drive clock low  //DEBUG!
// 		asm("nop \n"); //pause for one clock cycle
// //		 PORTA = PORTA | ((Val >> i) & 0x01 ) << 0x05;  //DEBUG!
//     PORTA = PORTA & 0xDF;
// 		PORTA = PORTA | (Val >> 0x02) & 0x20;  //DEBUG!
// 		//// PORTA = PORTA | (Val  << (0x05 + i)) & 0x20;  //DEBUG!
// 		asm("nop \n"); //pause for one clock cycle
// 		PORTB = PORTB | 0x01; //Set bit, drive clock high  //DEBUG!
// 		asm("nop \n"); //pause for one clock cycle
// 		Val = Val << 0x01; //Shift to new val


		 digitalWrite(CLK, LOW); //Set clock low
		 digitalWrite(DATA, (Val >> i) & 0x01); //Write LSB to DATA pin
		 digitalWrite(CLK, HIGH); //Clock values in on rising edge
	}
}

void ClearCount()
{
	digitalWrite(CLEAR, LOW);
	delayMicroseconds(1);
	// delay(1); //Wait for clear to take place
	digitalWrite(CLEAR, HIGH); //Return to nominal value
}

void SplitAndLoad(uint8_t Pos, unsigned int Val) //Write 16 bits
{
	uint8_t Len = sizeof(Val);
	for(int i = Pos; i < Pos + Len; i++) {
		Reg[i] = (Val >> (i - Pos)*8) & 0xFF; //Pullout the next byte
	}
}

void SplitAndLoad(uint8_t Pos, long Val)  //Write 32 bits  //FIX! Needed?
{
	uint8_t Len = sizeof(Val);
	for(int i = Pos; i < Pos + Len; i++) {
		Reg[i] = (Val >> (i - Pos)*8) & 0xFF; //Pullout the next byte
	}
}

boolean addressEvent(uint16_t address, uint8_t count)
{
	RepeatedStart = (count > 0 ? true : false);
	return true; // send ACK to master
}

void requestEvent()
{	
	//Allow for repeated start condition 
	if(RepeatedStart) {  //FIX! Make repeated start actually work!
		for(int i = 0; i < 2; i++) {
			Wire.write(Reg[RegID + i]);
		}
	}
	else {
		Wire.write(Reg[RegID]);
	}
}

void receiveEvent(int DataLen) 
{
    //Write data to appropriate location
    if(DataLen == 2){
	    //Remove while loop?? 
	    while(Wire.available() < 2); //Only option for writing would be register address, and single 8 bit value
	    uint8_t Pos = Wire.read();
	    uint8_t Val = Wire.read();
	    //Check for validity of write??
	    Reg[Pos] = Val; //Set register value
	}

	if(DataLen == 1){
		RegID = Wire.read(); //Read in the register ID to be used for subsequent read
	}
}

void stopEvent() 
{
	StopFlag = true;
	//End comunication
}

void Trip(void)
{
	TripTime = micros() - TripTime; //Measure delta period
	NewData = true; //Set new data flag 
}
/******************************************************************************
Calypso_Demo.cpp
A serial interface script for testing 
Bobby Schulz @ Northern Widget LLC
2/19/2019

https://github.com/NorthernWidget-Skunkworks/Project-Calypso

This script is designed to be loaded onto a top-side device and provide serial interface control over the Calypso for rapid testing

"What is a scientist after all? It is a curious man looking through a keyhole, the keyhole of nature, trying to know what's going on."
-Jacques Cousteau

Distributed as-is; no warranty is given.
******************************************************************************/

#include <Wire.h>

#define ADR 0x40  //Default calypso address 
#define LED0 0x02 //LED0 current register location
#define LED1 0x04 //LED1 current register location
#define CTRL 0x00 //Control register location
#define COUNT 0x07 //Location for count value

#define ON true
#define OFF false

boolean Safety = false; //FIX!!
char ReadArray[25] = {0};

String LEDVals[2] = {"LED1", "LED2"};
String StatusVals[2] = {"Off", "On"};
String InputVals[2] = {"In0", "In1"};

void setup() {
  	Serial.begin(57600); //DEBUG!
  	Serial.println("Welcome to Calypso...");
	Serial.println("Interface Developed by Dennis Nedry");
	Serial.print("Loading...\n\n");
	Wire.begin();
	Serial.println("Done");
	Serial.println("Enter When Ready...\n");

  
  tone(2, 100);
}

void loop() {
	// WriteWord(LED0, 10000);
	// WriteWord(LED1, 1000);
	// SetLED(LED0, ON);
	// SetLED(LED1, OFF);
	// WriteByte(COUNT, 25);
	// Serial.println(GetSample());
	// // // delay(2000);
	// // WriteByte(CTRL, 0xC4); //Set Sample bit and enable bit, enable LED1, LED2, use input 0
	// // delay(10);
	// // bool Ready = false;
	// // while(!Ready){
	// // 	Wire.beginTransmission(ADR);
	// // 	Wire.write(0x00);
	// // 	Wire.endTransmission(true);

	// // 	Wire.requestFrom(ADR, 1);
	// // 	if((Wire.read() & 0x80) == 0x00) Ready = true; //only proceed if new data is ready, indicated by clearing on sample/rdy bit
	// // }
	// // Serial.println(GetTime());
	// delay(2000); 

	static int ReadLength = 0;
  String ReadString;
  
  if(Serial.available() > 0) {
    char Input = Serial.read();

    if(Input != '\r') { //Wait for carrage return
      ReadArray[ReadLength] = Input;
      ReadLength++;
    }

    if(Input == '\r') {
      ReadString = String(ReadArray);
      ReadString.trim();
      memset(ReadArray, 0, sizeof(ReadArray));
      ReadLength = 0;
      
      if(ReadString == "Help") {
      	Serial.println("Commands:");
      	Serial.print(F("\tSet LED [1,2],[0,1]\n\tSet Current [1,2],[0 ~ 65000uA]\n\tSet Count [0 ~ 255]\n\tSet Input [0,1]\n\tGet Sample\n"));
      	// Serial.print(F("\tSet HP\n\tSet Mode uC\n\tSet Pin uC\n\tRead Pin uC\n\tSet DAC\n\tRead ADC OB\n\tRead ADC Ext\n"));
      }

      if(ReadString.indexOf("Set LED") >= 0) { //"Pin Mode uC xx y", xx is pin position (0 ~ 32), y is direction (0 ~ 1)
    	// Serial.println("BANG, LED");
      	// Serial.println(ReadString.substring(7).toInt()); //DEBUG!
      	// Serial.println(ReadString.indexOf(",")); //DEBUG!
      	uint8_t LED_Pos = ReadString.substring(7).toInt();
      	bool LED_State = ReadString.substring(ReadString.indexOf(",") + 1).toInt();
      	if(LED_Pos > 0 && LED_Pos < 3 && LED_State >= 0 && LED_State < 2) {//Check for valid range
      		SetLED(LED_Pos - 1, LED_State); //Pass int to function
      		Serial.print("\t"); Serial.print(LEDVals[ReadString.substring(7).toInt() - 1]); Serial.print(" is "); Serial.println(StatusVals[ReadString.substring(ReadString.indexOf(",") + 1).toInt()]);
      	}
      	else Serial.println("\tInvaid Input!");
      }

      if(ReadString.indexOf("Set Input") >= 0) { //"Pin Mode uC xx y", xx is pin position (0 ~ 32), y is direction (0 ~ 1)
      	bool Input = ReadString.substring(9).toInt();
      	if(Input >= 0 && Input < 2) {//Check for valid range
      		SetInput(Input); 
      		Serial.print("\t"); Serial.print(InputVals[Input]); Serial.println(" is selected");
      	}
      	else Serial.println("\tInvaid Input!");
      }


      if(ReadString.indexOf("Set Current") >= 0) { //"Pin Mode uC xx y", xx is pin position (0 ~ 32), y is direction (0 ~ 1)  //ADD error checking??
      // Serial.println("BANG, Current");
      // Serial.println(ReadString.substring(11).toInt()); //DEBUG!
      // Serial.println(ReadString.substring(ReadString.indexOf(",") + 1).toInt()); //DEBUG!
      uint8_t LED_Pos = ReadString.substring(11).toInt();
      uint16_t CurrentVal = ReadString.substring(ReadString.indexOf(",") + 1).toInt();
      if(CurrentVal > 20000 && !Safety) Serial.println("\tDangerous Current! Turn off saftey to proceed...");
      else {
      	if(LED_Pos > 0 && LED_Pos < 3) {//Check for valid range
      		WriteWord(2*LED_Pos, CurrentVal); //Pass int to function
      		Serial.print("\t"); Serial.print(LEDVals[LED_Pos - 1]); 
      		Serial.print(" current = "); Serial.print(ReadString.substring(ReadString.indexOf(",") + 1).toInt()); Serial.println(" uA");
      	}
      	else Serial.println("\tInvaid Input!");

      	}
  		}

      if(ReadString.indexOf("Set Count") >= 0) { //"Set uC Pin xx y", xx is pin position (0 ~ 32), y is value (0 ~ 1)
      	// Serial.println(ReadString.substring(ReadString.indexOf(",")).toInt()); //DEBUG!
      	// Serial.println(ReadString.substring(ReadString.indexOf(","))); //DEBUG!
      	// Serial.println(ReadString.substring(ReadString.indexOf(",") + 1).toInt()); //DEBUG!
      	// Serial.println("BANG, Count");
      	uint8_t CountVal = ReadString.substring(9).toInt();
      	if(CountVal <= 255) {
			WriteByte(COUNT, CountVal);
			Serial.print("\tCount = "); Serial.println(ReadString.substring(9).toInt());
		}
		else Serial.println("\tCount value out of range");
      }

      if(ReadString.indexOf("Get Sample") >= 0) { //"Set IO Pin xx y", xx is pin position (0 ~ 32), y is value (0 ~ 1)
      	unsigned long Val = GetSample();
      	Serial.print("\tPeriod = "); Serial.print(Val); Serial.println(" us");
      	Serial.print("\tFrequency = "); Serial.print(1.0/float(Val/1000000.0), 5); Serial.println(" Hz");
      }

      if(ReadString == "ItsAUnixSystem") {
       Serial.println("\nSafety checks disabled");
       Serial.println("Careful not to turn all the fences off...");
        Safety = true;
      }
    }
	}
}

int SetLED(bool Pos, bool State)
{
	uint8_t Temp = ReadByte(CTRL); //Read control register 
	Temp = Temp & (0xF9 | (1 << !Pos + 1)); //Clear position in reg
	Temp = Temp | (State << Pos + 1); //Write value to pos
	WriteByte(CTRL, Temp); //Write moddified value back
	// Serial.println(Temp); //DEBUG!
}

int SetInput(bool Input)
{
	uint8_t Temp = ReadByte(CTRL); //Read control register 
	Temp = Temp & 0xFE; //Clear position in reg
	Temp = Temp | Input; //Write value to pos
	WriteByte(CTRL, Temp); //Write moddified value back
}

unsigned long GetSample()  //Returns the average period in microseconds 
{
	uint8_t Temp = ReadByte(CTRL);  //Read state of control register 
	Temp = Temp | 0xC0; //Set sample and enable bit
	WriteByte(CTRL, Temp); //Start sample

	bool Ready = false;
	while(!Ready && Serial.read() != '\r'){  //Wait for ready bit to be cleared 
		Wire.beginTransmission(ADR);
		Wire.write(0x00);
		Wire.endTransmission(true);

		Wire.requestFrom(ADR, 1);
		if((Wire.read() & 0x80) == 0x00) Ready = true; //only proceed if new data is ready, indicated by clearing on sample/rdy bit
	}
	uint8_t CountVal = ReadByte(COUNT); //Read the multiple of the count

	if(!Ready) return 0; //If exited because of Serial override 
	return GetTime()/CountVal;  //Return the result of the sample divided by the number of counts (period)
}

unsigned long GetTime()  //FIX!
{
	unsigned long Time = 0;
	Wire.beginTransmission(ADR);
	Wire.write(0x0B);
	Wire.endTransmission(true);
	Wire.requestFrom(ADR, 1);
	while(Wire.available() < 1); //wait to get data
	Time = Wire.read();

	Wire.beginTransmission(ADR);
	Wire.write(0x0A);
	Wire.endTransmission(true);
	Wire.requestFrom(ADR, 1);
	while(Wire.available() < 1); //wait to get data
	Time = (Time << 8) | Wire.read();

	Wire.beginTransmission(ADR);
	Wire.write(0x09);
	Wire.endTransmission(true);
	Wire.requestFrom(ADR, 1);
	while(Wire.available() < 1); //wait to get data
	Time = (Time << 8) | Wire.read();	

	return Time*8; //Multiple by fixed resolution/scale factor (FIX!!?)
}

void WriteWord(uint8_t Reg, uint16_t Val)
{
	Wire.beginTransmission(ADR);
	Wire.write(Reg);
	Wire.write(Val & 0xFF); //Write LSB
	Wire.endTransmission();
	Wire.beginTransmission(ADR);
	Wire.write(Reg + 1);
	Wire.write(Val >> 8); //Write MSB
	Wire.endTransmission();
}

int WriteByte(uint8_t Reg, uint8_t Val)
{
	Wire.beginTransmission(ADR);
	Wire.write(Reg);
	Wire.write(Val); //Write full byte
	return Wire.endTransmission();
}

uint8_t ReadByte(uint8_t Reg)
{
	Wire.beginTransmission(ADR);
	Wire.write(Reg); //Set pointer to desired register 
	Wire.endTransmission(); //FIX! Use error flag?

	Wire.requestFrom(ADR, 1); //Read single byte back
	return Wire.read(); 
}

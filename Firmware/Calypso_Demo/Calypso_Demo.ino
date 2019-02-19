#include <Wire.h>

#define ADR 0x40  //Default calypso address 
#define LED0 0x02 //LED0 current register location
#define LED1 0x04 //LED1 current register location
#define CTRL 0x00 //Control register location
#define COUNT 0x07 //Location for count value

void setup() {
  Serial.begin(9600); //DEBUG!
  Wire.begin();
  tone(2, 2000);
  WriteByte(COUNT, 25);
}

void loop() {
	WriteWord(LED0, 10000);
	delay(2000);
	Serial.println(WriteByte(CTRL, 0x80)); //Set Sample bit
	delay(110);
	bool Ready = false;
	// Serial.println(millis()); //DEBUG!
	while(!Ready){
		Wire.beginTransmission(ADR);
		Wire.write(0x00);
		Wire.endTransmission(true);

		Wire.requestFrom(ADR, 1);
		if(Wire.read() == 0x01) Ready = true; //only proceed if new data is ready
	}
	// Serial.println(millis()); //DEBUG!
	uint16_t Time = 0;
	uint8_t Temp = 0;
	Wire.beginTransmission(ADR);
	Wire.write(0x0B);
	Wire.endTransmission(true);
	Wire.requestFrom(ADR, 1);
	while(Wire.available() < 1); //wait to get data
	Temp = Wire.read();
	Time = Temp;
	Serial.println(Temp);

	Wire.beginTransmission(ADR);
	Wire.write(0x0A);
	Wire.endTransmission(true);
	Wire.requestFrom(ADR, 1);
	while(Wire.available() < 1); //wait to get data
	Temp = Wire.read();
	Time = (Time << 8) | Temp;
	Serial.println(Temp);

	Wire.beginTransmission(ADR);
	Wire.write(0x09);
	Wire.endTransmission(true);
	Wire.requestFrom(ADR, 1);
	while(Wire.available() < 1); //wait to get data
	Temp = Wire.read();
	Time = (Time << 8) | Temp;	
	Serial.println(Temp);

	Serial.println(Time*8);
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

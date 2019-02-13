//Calypso_Firmware_Testing.ino
#include <SlowSoftWire.h> 

SlowSoftWire Wire = SlowSoftWire(2, 3);
#define MCP4726_CMD_WRITEDAC            (0x40)  // Writes data to the DAC
#define MCP4726_CMD_WRITEDACEEPROM      (0x60)  // Writes data to the DAC and the EEPROM (persisting the assigned value after reset)

#define LED1 0  //Define values of LEDs for side indication
#define LED2 1

#define URES 9 //uReset on D9
#define LED1_CTRL 10 //LED 1 control pin on D10
#define LED2_CTRL 10 //LED 2 control pin on D10 (Tied together for demo!!!!!!)
#define DATA 5 //Data line to counter on D5
#define CLK 0 //Clock line to counter of D0
#define CLEAR 7 //Clear line to counter on D7
#define DONE 1 //Int line to comparator on D1

// #define DAC_1_ADR 0x63  //Address for DAC that controls LED1 current
// #define DAC_2_ADR 0x62  //Address for DAC that controls LED2 current
uint8_t DAC_Adr[2] = {0x63, 0x62}; //Address for LED1 DAC, LED2 DAC

uint16_t Res = 100; //Resistance in Ohms of load resistor(s)
uint8_t Div = 2; //Division factor

void setup() {
  // put your setup code here, to run once:
  Wire.begin();
  // pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  // digitalWrite(9, HIGH);
  digitalWrite(10, HIGH); //Turn on LED switches 

  pinMode(LED1_CTRL, OUTPUT);
  pinMode(DATA, OUTPUT);
  pinMode(CLK, OUTPUT);
  pinMode(CLEAR, OUTPUT);
  pinMode(URES, OUTPUT);
  pinMode(DONE, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  // for(int i = 0; i < 4096; i += 10) {
  // 	SetVoltageRaw(i, LED1);
  // 	SetVoltageRaw(4095 - i, LED2);
  // }
  // for(int i = 4095; i > 0; i-= 10) {
  // 	SetVoltageRaw(i, LED1);
  // 	SetVoltageRaw(4095 - i, LED2);
  // }

  // SetVoltageRaw(0, LED1);
  // SetVoltageRaw(0, LED2);
  // delay(1000);
    digitalWrite(LED1_CTRL, HIGH);
//    SetVoltageRaw(128, LED1);
//    delay(2000);
//    SetVoltageRaw(256, LED1);
//    delay(2000);
//    SetVoltageRaw(512, LED1);
//    delay(2000);
//    SetVoltageRaw(1024, LED1);
//    delay(2000);
//    SetVoltageRaw(2048, LED1);
//    delay(2000);
    SetVoltageRaw(4095, LED1);
    delay(2000);
//  ClearCount();
//  SetCount(10);
//  digitalWrite(URES, LOW);
//  delayMicroseconds(300);
//  digitalWrite(URES, HIGH);
//  delay(100); //DEBUG!

  // while(1);
  // delay(10);
  // ClearCount();
  // delay(10);
  // SetCount(2);
  // delay(10);
  // ClearCount();
  // delay(10);
}

void LED_Demo()  //Test function only!
{
	for(int i = 0; i < 4096; i += 10) {
		SetVoltageRaw(i, LED1);
		SetVoltageRaw(4095 - i, LED2);
	}
	for(int i = 4095; i > 0; i-= 10) {
		SetVoltageRaw(i, LED1);
		SetVoltageRaw(4095 - i, LED2);
	}
}

//Current given in uA
int SetCurrent(long Current, bool Side) //Set current drive for led, use Side to set which LED to set, 0 for LED1, 1 for LED2
{
	//ADD VOLTAGE DIVIDER VALUE!!??
	//RETURN BAD VALUE IF SET OUTSIDE OF RANGE!??
	uint16_t Voltage = ((Current*Res)/806)*Div; //Determine current set from load resistor 806 = 4096/3300000, bits/uV range
	Wire.beginTransmission(DAC_Adr[Side]);
	Wire.write(MCP4726_CMD_WRITEDAC);
	Wire.write(Voltage / 16);                   // Upper data bits          (D11.D10.D9.D8.D7.D6.D5.D4)
	Wire.write((Voltage % 16) << 4);            // Lower data bits          (D3.D2.D1.D0.x.x.x.x)
	Wire.endTransmission();
}

//Set bit value
int SetVoltageRaw(uint16_t Val, bool Side) 
{
	//ADD VOLTAGE DIVIDER VALUE!!??
	//RETURN BAD VALUE IF SET OUTSIDE OF RANGE!??
	// uint16_t Voltage = ((Current*Res)/806)*Div; //Determine current set from load resistor 806 = 4096/3300000, bits/uV range
	Wire.beginTransmission(DAC_Adr[Side]);
	Wire.write(MCP4726_CMD_WRITEDAC);
	Wire.write(Val / 16);                   // Upper data bits          (D11.D10.D9.D8.D7.D6.D5.D4)
	Wire.write((Val % 16) << 4);            // Lower data bits          (D3.D2.D1.D0.x.x.x.x)
	Wire.endTransmission();
}

void SetCount(uint8_t Val)
{
	bool Dummy = false; 
	DDRB = DDRB | 0x01; //Set pin PB0 as output  //DEBUG!
	DDRA = DDRA | 0x28; //Set pin PA5 and pin PA3 as output  //DEBUG!
	PORTA = PORTA | 0x08; //Turn Clear Off
	PORTA = PORTA & 0xDF; //Clear PA5 
	// Val = Val << 0x05; //Offset to allign with PORTA
	for(int i = 7; i >= 0; i--) {
		//FAST MODE
		PORTB = PORTB & 0xFE; //Clear bit, drive clock low  //DEBUG!
		asm("nop \n"); //pause for one clock cycle
//		 PORTA = PORTA | ((Val >> i) & 0x01 ) << 0x05;  //DEBUG!
    PORTA = PORTA & 0xDF;
		PORTA = PORTA | (Val >> 0x02) & 0x20;  //DEBUG!
		//// PORTA = PORTA | (Val  << (0x05 + i)) & 0x20;  //DEBUG!
		asm("nop \n"); //pause for one clock cycle
		PORTB = PORTB | 0x01; //Set bit, drive clock high  //DEBUG!
		asm("nop \n"); //pause for one clock cycle
		Val = Val << 0x01; //Shift to new val


//		 digitalWrite(CLK, LOW); //Set clock low
//		 digitalWrite(DATA, (Val >> i) & 0x01); //Write LSB to DATA pin
//		 digitalWrite(CLK, HIGH); //Clock values in on rising edge
	}
}

void ClearCount()
{
	digitalWrite(CLEAR, LOW);
	delayMicroseconds(1);
	// delay(1); //Wait for clear to take place
	digitalWrite(CLEAR, HIGH); //Return to nominal value
}

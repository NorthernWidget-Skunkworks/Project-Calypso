//Calypso_Firmware_Testing.ino
#include <SlowSoftWire.h> 

SlowSoftWire Wire = SlowSoftWire(2, 3);
#define MCP4726_CMD_WRITEDAC            (0x40)  // Writes data to the DAC
#define MCP4726_CMD_WRITEDACEEPROM      (0x60)  // Writes data to the DAC and the EEPROM (persisting the assigned value after reset)

#define LED1 0  //Define values of LEDs for side indication
#define LED2 1
// #define DAC_1_ADR 0x63  //Address for DAC that controls LED1 current
// #define DAC_2_ADR 0x62  //Address for DAC that controls LED2 current
uint8_t DAC_Adr[2] = {0x63, 0x62}; //Address for LED1 DAC, LED2 DAC

uint16_t Res = 100; //Resistance in Ohms of load resistor(s)
uint8_t Div = 2; //Division factor

void setup() {
  // put your setup code here, to run once:
  Wire.begin();
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  digitalWrite(9, HIGH);
  digitalWrite(10, HIGH); //Turn on LED switches 
}

void loop() {
  // put your main code here, to run repeatedly:
  for(int i = 0; i < 4096; i += 10) {
  	SetVoltageRaw(i, LED1);
  	SetVoltageRaw(4095 - i, LED2);
  }
  for(int i = 4095; i > 0; i-= 10) {
  	SetVoltageRaw(i, LED1);
  	SetVoltageRaw(4095 - i, LED2);
  }

  SetVoltageRaw(0, LED1);
  SetVoltageRaw(0, LED2);
  delay(1000);
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
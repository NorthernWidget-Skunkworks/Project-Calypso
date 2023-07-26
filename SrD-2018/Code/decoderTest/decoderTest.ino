//test sketch to practive encoding uint32_t into 
//chars and then back to a uint32_t
//
//WORKING - DO NOT CHANGE

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  uint32_t fs = 1234567890;
  Serial.print("Starting fs:");
  Serial.println(fs);
  Serial.print("Starting Hex:");
  Serial.println(fs, HEX);

  Serial.println("Beginning transform...");
  //beginning transform
  uint8_t byte0 = (uint8_t) ((fs & 0xFF000000) >> 24);
  uint8_t byte1 = (uint8_t) ((fs & 0x00FF0000) >> 16);
  uint8_t byte2 = (uint8_t) ((fs & 0x0000FF00) >> 8);
  uint8_t byte3 = (uint8_t) ((fs & 0x000000FF) >> 0);

  Serial.println("Byte transforms...");
  Serial.println(byte0, HEX);
  Serial.println(byte1, HEX);
  Serial.println(byte2, HEX);
  Serial.println(byte3, HEX);

  Serial.println("Decoding bytes...");
  uint32_t newfs = ((uint32_t)byte0 << 24) | ((uint32_t)byte1 << 16) | ((uint32_t)byte2 << 8) | ((uint32_t)byte3 << 0); 
  Serial.print("Ending fs:");
  Serial.println(newfs);
  Serial.print("Ending Hex:");
  Serial.println(newfs, HEX);

  Serial.println("Done.");
}

void loop() {
  // put your main code here, to run repeatedly:

}

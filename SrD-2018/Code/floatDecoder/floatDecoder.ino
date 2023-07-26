//This is a test for sending floats over I2C
//
//Floats are 4 bytes
//This program works :)

float sent[6];
float rcvd[6];

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  //Filling arrays with data
  Serial.println("Filling sent array...");
  sent[0] = 1.111;
  sent[1] = 1.111*2;
  sent[2] = 1.111*3;
  sent[3] = 1.111*4;
  sent[4] = 1.111*5;
  sent[5] = 1.111*6;
  Serial.println(sent[0]);
  Serial.println(sent[5]);
  rcvd[0] = 0;
  rcvd[1] = 0;
  rcvd[2] = 0;
  rcvd[3] = 0;
  rcvd[4] = 0;
  rcvd[5] = 0;

  //Copy algorithm
  byte cbyte;
  byte * ptr = (byte *)sent;
  byte * ptr2 = (byte *)rcvd;
  Serial.println("Copying data...");
  for(int i=0; i<24; i++) {
    cbyte = *(ptr + i);
    *(ptr2 + i) = cbyte;
  }

  //Displaying arrays
  Serial.println("Done copying data.");
  Serial.println(rcvd[0]);
  Serial.println(rcvd[1]);
  Serial.println(rcvd[2]);
  Serial.println(rcvd[3]);
  Serial.println(rcvd[4]);
  Serial.println(rcvd[5]);
  
}

void loop() {
  // put your main code here, to run repeatedly:

}

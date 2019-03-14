

#include <SPI.h>

SPISettings settings(6000000, MSBFIRST, SPI_MODE0 ); //variable to hold SPI settings
//Set Slave Select, MOSI, MISO, CLK 
const int CSN = 4;
const int SO = 12;
const int SI = 11;
const int CLK = 13;

// Needed to convert the bytes from SPI to float
union u_types {
   byte b[4];
   float fval;
} data[3];  // Create 3 unions, one for each euler angle

void setup() {
 //Set Pin Modes
 pinMode(CSN, OUTPUT);
 pinMode(SI, OUTPUT);
 pinMode(SO, INPUT);
 pinMode(CLK, OUTPUT);
 //Set Slave Select High to Start i.e disable chip
 digitalWrite(CSN, HIGH);
 //Initialize SPI
 SPI.begin();
 //pour a bowl of serial
 Serial.begin(9600);
}

//function to transfer commands through SPI
byte transferByte(byte byteToWrite) {

 byte Result = 0x00;
 digitalWrite(CSN,LOW);
 delay(1);
 Result = SPI.transfer(byteToWrite);
 delay(1);
 digitalWrite(CSN,HIGH);
 return Result; 
}

//function to swap endian
void endianSwap(byte temp[4]) {
 byte myTemp = temp[0];
 temp[0] = temp[3];
 temp[3] = myTemp;
 myTemp = temp[1];
 temp[1] = temp[2];
 temp[2] = myTemp;
}

void loop() {
 SPI.beginTransaction(settings);
  
 // Clear the internal data buffer on the IMU
 byte result = transferByte(0x01);
     Serial.print("Cleared internal buffer. Result: "),Serial.println(result);

 // Send start of packet:
 result = transferByte(0xF6);
      Serial.print("Send start of packet. Result: "),Serial.println(result);
 
 // Send command (tared euler angles)
 result = transferByte(0x01);
      Serial.print("Send commmand 0x01. Result: "),Serial.println(result);
 
 // Get status of device:
 result = transferByte(0xFF);
      Serial.print("Status of device. Result: "),Serial.println(result);

 while (result != 0x01) {  // Repeat until device is Ready
   delay(1);
   result = transferByte(0xFF);
   Serial.print("Status of device. Result: "),Serial.println(result);
 }
 
 // Get the 12 bytes of return data from the device:
 for (int ii=0; ii<3; ii++) {
   for (int jj=0; jj<4; jj++) {
     data[ii].b[jj] =  transferByte(0xFF);
     delay(1);
   }
 }  

 SPI.endTransaction();

 for( int mm=0; mm<3; mm++) {
   endianSwap(data[mm].b);
 }
 
 
 Serial.print("fval 1:"), Serial.println(data[0].fval);
 Serial.print("fval 2:"), Serial.println(data[1].fval);
 Serial.print("fval 3:"), Serial.println(data[2].fval);

 delay(3000);
}

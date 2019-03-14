
#include <Wire.h>


void setup() {
  Wire.begin();
  Serial.begin(9600);
}

byte stat = 0;

#define MASTER 0xEE
#define SLAVE 0x8C

void loop() {

  Wire.beginTransmission(byte(SLAVE>>1));
  Wire.write(byte(0x41));
  Wire.write(byte(0x43));
  Wire.endTransmission();

  delay(70);
  
  Wire.requestFrom(byte(SLAVE>>1), 1);

  Serial.println(Wire.available());
  stat = Wire.read();

  Serial.println(stat);
  delay(1000);
    
}

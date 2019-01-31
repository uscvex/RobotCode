#include <Wire.h>

void setup() {
  pinMode(PD2,OUTPUT);
  pinMode(PD3,OUTPUT);
  pinMode(PD4,INPUT_PULLUP);
  digitalWrite(PD2,HIGH);
  Serial.begin(115200);
  Serial.setTimeout(500);
  interrupts();
}

volatile int dist = 0;
volatile int ir = 0;
volatile char updated = 0;

void serialEvent() {
  int noChars = 0;
  int value = 0;
  while (Serial.available()) {
    noChars++;
    char thisChar = Serial.read();
//    Serial.print(thisChar);
//    Serial.print(' ');
    if (thisChar == 'D')        // Start of transmission
      value = 0;
    else if (thisChar == 'I') { // End of 1st number
      if (value > 0) dist = value;
      value = 0;
    }
    else if (thisChar == 'E') { // End of transmission
      if (value > 0) ir = value;
      value = 0;
      updated = 1;
    }
    else if (thisChar >= '0' && thisChar <= '9') {  // Getting a digit
      value *= 10;
      value += (thisChar - '0');
    }
  }
}

void printStuff() {
  Serial.println("");
  Serial.print("Dist: ");
  Serial.println(dist);
  Serial.print("IR: ");
  int temp = ir;
  for (int i = 0; i < 8; i++) {
    temp = temp >> 1;
    Serial.print(temp & 1);
  }
}

void loop() {
  digitalWrite(PD2,LOW);
 
  if (updated) {
    updated = 0;
    printStuff();
  }
  
  if (!digitalRead(PD4)) {
    delay(5);
    while (!digitalRead(PD4)) {}
    printStuff();
  }
  delay(50);
}

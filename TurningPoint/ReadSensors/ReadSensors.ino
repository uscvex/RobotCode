#include <LIDARLite.h>
#include <Wire.h>
#include <JY901.h>

#define RATE 15

#define NO_CODES 2

LIDARLite lidar1;



void setup() {
  pinMode(PD2,OUTPUT);  // Serial
  pinMode(PD3,OUTPUT);  // LED
  pinMode(PD4,INPUT_PULLUP);  // Button
  
  digitalWrite(PD2,LOW);
  Serial.begin(115200);
  lidar1.begin(0, true);
  lidar1.configure(1);

  pinMode(12,INPUT);  // IR8
  pinMode(11,INPUT);  // IR7
  pinMode(10,INPUT);  // IR6
  pinMode(9,INPUT);  // IR5
  pinMode(8,INPUT);  // IR4
  pinMode(7,INPUT);  // IR3
  pinMode(6,INPUT);  // IR2
  pinMode(5,INPUT);  // IR1
}

char flip = 0;
char lit = 0;

int IR = 0;

void loop() {

  IR = 0;
  for (int i = 12; i >= 5; i--) { // Read IR Sensors
    IR = IR << 1;
    IR |= !digitalRead(i);
  }
  IR = IR << 1;
  IR |= 1;
  
  digitalWrite(PD2,HIGH);

  Serial.print('D');  // Send info
  //Serial.print(lidar1.distance());
  Serial.print('I');
  Serial.print(IR);
  Serial.print('A');
  Serial.print("100");//(float)JY901.stcAngle.Angle[2]/32768*180);
  Serial.print('E');

  Serial.flush();

  digitalWrite(PD2,LOW);

  if (flip++ == RATE) { // Flash light
    flip = 0;
    lit = !lit;
  }
  digitalWrite(2, lit);
//
//  if (!digitalRead(PD4)) {  // Button to do something?
//    delay(5);
//    while (!digitalRead(PD4)) {
//      digitalWrite(PD3, HIGH);
//      delay(5);
//    }
//    // Do something
//    /*Serial.println("");
//    Serial.print('I');
//    Serial.println(IR);*/
//  }

  delay(20);
  
}



/*
  SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 */
void serialEvent() 
{
  while (Serial.available()) 
  {
    JY901.CopeSerialData(Serial.read()); //Call JY901 data cope function
  }
}

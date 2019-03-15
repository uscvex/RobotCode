
//#define LIDAR

#ifdef LIDAR
#include <LIDARLite.h>
#include <Wire.h>
LIDARLite lidar1;
#endif

String xStr = "";
String yStr = "";
String zStr = "";

float xVal = 0;
float yVal = 0;
float zVal = 0;

void getData(char* msg) {
  Serial.write(msg);
  delay(100);
  Serial.println(Serial.readStringUntil('\n'));
}

void sendData(char* msg) {
  Serial.write(msg);
}


void sensorStart() {
 
  digitalWrite(2,LOW);
  delay(150);
  digitalWrite(2,HIGH);
  delay(1000);
  digitalWrite(2,LOW);

  sendData(":165\n"); // Gyro calibrate
  sendData(":96\n");    // Tare
  
  delay(2000);
  
  digitalWrite(2,HIGH);
  delay(500);
}


void setup() {
  Serial.begin(115200); // Start serial
  
#ifdef LIDAR
  lidar1.begin(0);
  lidar1.configure(0);
#endif
  
  pinMode(2,OUTPUT);
  pinMode(3,INPUT_PULLUP);
  digitalWrite(2,HIGH);
  Serial.setTimeout(10);
  sendData(":96\n");    // Tare
}

int count = 0;

void loop() {

#ifdef LIDAR
  float lidarDist = lidar1.distance();
  Serial.print('D');  // Send info
  Serial.print(lidarDist);
  Serial.print('I');
#endif

  if (!digitalRead(3)) {
    count++;
    if (count > 15) {
      sensorStart();
      count = 0;
    }
  }
  else {
    count = 0;
  }

  sendData(":1\n");
  digitalWrite(2,HIGH);
  
  delay(20);
  
  if (Serial.available() > 20) {
    xStr = Serial.readStringUntil(',');
    yStr = Serial.readStringUntil(',');
    zStr = Serial.readStringUntil(',');
    xVal = xStr.toFloat()*180/M_PI;
    yVal = yStr.toFloat()*180/M_PI;
    zVal = zStr.toFloat()*180/M_PI;
    digitalWrite(2,LOW);
//    Serial.print("X "); 
//    Serial.println(xVal);
//    Serial.print("Y "); 
//    Serial.println(yVal);
//    Serial.print("Z "); 
//    Serial.println(zVal);
    digitalWrite(PD4,HIGH);

      Serial.print('A');
      Serial.print(yVal);
      Serial.print('E');
      Serial.flush();
      digitalWrite(PD4,LOW);
  }

}

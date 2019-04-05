
// LIDAR Defined if we are using lidar
#define LIDAR

#ifdef LIDAR
    #include <LIDARLite.h>
    #include <Wire.h>
    LIDARLite lidar1;
#endif

// Strings to store data
String xStr = "0";
String yStr = "0";
String zStr = "0";

// Floats to put translated data into
float xVal = 0;
float yVal = 0;
float zVal = 0;

// Function to send command to and recieve data from gyro
void getData(char* msg) {
    Serial.write(msg);
    delay(100);
    Serial.println(Serial.readStringUntil('\n'));
}

// Function to send command to gyro
void sendData(char* msg) {
    Serial.write(msg);
}


void sensorStart() {
    
    // Flash LED
    digitalWrite(2,LOW);
    delay(150);
    digitalWrite(2,HIGH);
    delay(1000);
    digitalWrite(2,LOW);
    
    // Send command to tare gyro
    sendData(":96\n");
    
    // Pause to let command send
    delay(2000);
    
    // Turn LED off
    digitalWrite(2,HIGH);
    delay(500);
}


void setup() {
    // Start serial
    Serial.begin(115200);
    
#ifdef LIDAR
    // Start lidar code
    lidar1.begin(0);
    lidar1.configure(0);
#endif
    
    // Set pinout
    pinMode(2,OUTPUT);
    pinMode(4,OUTPUT);
    pinMode(3,INPUT_PULLUP);
    // Turn light on
    digitalWrite(2,HIGH);
    Serial.setTimeout(10);
    // Tare gyro
    sendData(":96\n");
}

// Var to count how long button help
int count = 0;
void loop() {
    
    // If button held
    if (!digitalRead(3)) {
        // Count how long
        count++;
        if (count > 25) {
            // Run function and reset counter
            sensorStart();
            count = 0;
        }
    }
    else {
        // If button released, count back to 0
        count = 0;
    }
    
    // Send command 'read angle'
    sendData(":1\n");
    // Turn LED off
    digitalWrite(2,HIGH);
    
    // Pause to let command data recieve
    delay(10);
    
    // If the data is back
    if (Serial.available() > 20) {
        // Parse the data
        xStr = Serial.readStringUntil(',');
        yStr = Serial.readStringUntil(',');
        zStr = Serial.readStringUntil('\n');
        // Convert inoto floats
        xVal = xStr.toFloat()*180/M_PI;
        yVal = yStr.toFloat()*180/M_PI;
        zVal = zStr.toFloat()*180/M_PI;
        
        // Turn LED on
        digitalWrite(2,LOW);
        
        // Pull transmit enable high
        digitalWrite(PD4,HIGH);
        
#ifdef LIDAR
        // Find lidar distance
        float lidarDist = lidar1.distance();
        // Send lidar data over serial
        Serial.print('D');
        Serial.print(lidarDist);
        Serial.print('I');
#endif
        // Send gyro data over serial
        Serial.print('A');
        Serial.print(xVal);
        Serial.print('E');
        
        // Ensure all data sent
        Serial.flush();
        
        // Let transmit enable fall low
        digitalWrite(PD4,LOW);
    }
    
}

#include "main.h"

using namespace pros;

// Gyro tuning values
#define CCW_SCALE 0.9562
#define CW_SCALE 0.9677

// Sensor declaration
ADIGyro sensor_gyro(1, GYRO_PORT);

// Global variable for current robot direction
double direction = 0;

// Function to set current direction
void setDirection(double dir) {
    direction = dir;
}

// Task to read and update gyro values
void runGyro(void* params) {
    
    double gyroDirection = 0;       // Current gyro direction
    double gyroLastDirection = 0;   // Last gyro direction
    
    while (true) {
        
        // Read angle from gyro
        gyroDirection = sensor_gyro.get_value();
        
        // Calculate change this frame
        double difference = gyroDirection - gyroLastDirection;
        
        // Take into account wrapping
        if (difference > 2700)
            difference -= 3600;
        if (difference < -2700)
            difference += 3600;
        
        // Scale the change by our fudge factor
        if (difference < 0)
            difference *= CCW_SCALE;
        else
            difference *= CW_SCALE;
        
        // Now add difference to direction
        direction -= difference / 10;
        
        // Bound direction between 0 and 360
        while (direction < 0)
            direction += 360;
        while (direction >= 360)
            direction -= 360;
        
        pros::lcd::print(2, "Dir: %f", direction);
        
        // Record this angle for next time
        gyroLastDirection = gyroDirection;
        
        if (controller.get_digital(DIGITAL_X)) {
            setDirection(0);
        }
        
        delay(20);
    }
}

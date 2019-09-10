#include "main.h"


// Position tracking encoders
ADIEncoder rightEncoder(1,2);
ADIEncoder leftEncoder(3,4);
ADIEncoder backEncoder(5,6);

// Vars to save robot global position
double globalDirection = 0;
double globalX = 0;
double globalY = 0;


// Vars to save previous encoder values
double lastRightEncoder = 0;
double lastLeftEncoder = 0;
double lastBackEncoder = 0;


// Function to update global position based on encoder inputs
void trackPosition() {
    
    // Get values from encoders
    double rightValue = rightEncoder.get_value();
    double leftValue = leftEncoder.get_value();
    double backValue = backEncoder.get_value();
    
    // Calculate change in encoder values
    double rightChange = rightValue - lastRightEncoder;
    double leftChange = leftValue - lastLeftEncoder;
    double backChange = backValue - lastBackEncoder;
    
    // Update last values
    lastRightEncoder = rightValue;
    lastLeftEncoder = leftValue;
    lastBackEncoder = backValue;
    
    // Now find average change in encoders (ticks)
    double angleChange = rightChange + leftChange;
    //angleChange /= 2;
    
    // Take out the rotational component from strafe wheel
    backChange -= angleChange/2;
    
    // Convert ticks to inches
    angleChange *= INCHESPERTICK;
    
    // Find actual angle change (rad) based on distance between encoders
    angleChange /= TRACKINGDIAMETER;
    
    // Convert to degrees
    angleChange = (180 * angleChange) / (double)M_PI;
    
    // Calculate new global direction
    globalDirection += angleChange;
    
    // Wrap to [0,360]
    if (globalDirection > 360) globalDirection -= 360;
    if (globalDirection < 0) globalDirection += 360;
    
    // Get new global direction in degrees
    double globalDirectionRad = ((double)M_PI * globalDirection) / 180;
    
    // Find change in forward position and sideways position
    double forwardChange = INCHESPERTICK * (leftChange - rightChange) / 2;
    double strafeChange = backChange * INCHESPERTICK;
    
    // Calculate global change based on current angle
    double changeX = forwardChange * sin(globalDirectionRad) + strafeChange * cos(globalDirectionRad);
    double changeY = strafeChange * sin(globalDirectionRad) - forwardChange * cos(globalDirectionRad);
    
    // Calculate new global position
    globalX += changeX;
    globalY += changeY;
    
    // Print position info to screen
    pros::lcd::print(1, "(x,y,theta) = (%f, %f, %f)", globalX, globalY, globalDirection);\
    
}

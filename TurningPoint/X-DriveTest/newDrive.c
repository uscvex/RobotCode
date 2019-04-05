
// Task to run the drive
task runDrive()
{
    float upDrive;
    float rightDrive;
    float driveAngle=1;
    float driveMag;
    float gyroAng;
    float seek,angle,ang1;
    
    while (true) {
        // Get desired turn rate
        turn = -vexRT[Ch1];
        
        // Get forward/backward component of desired velocity
        upDrive=-vexRT[Ch3];
        // Get left/right component of desired velocity
        rightDrive=-vexRT[Ch4];
        
        // Prevent div/0 error for next step
        if (rightDrive==0)
            rightDrive=0.1;
        
        // Now convert components of desired velocity into angle
        // Use different tan functions depending on which quad. of unit circle
        if (rightDrive*upDrive>=0)
            driveAngle = atan((upDrive) / (rightDrive));
        else
            driveAngle = atan2((upDrive),  (rightDrive));
        
        //Change it to degrees
        driveAngle = radiansToDegrees(driveAngle);
        
        // Check quad. of unit circle again and adjust if necessary
        if (rightDrive<0 && upDrive<0)
            driveAngle=driveAngle+180;
        
        //Find magnitude of joystick
        driveMag = sqrt((rightDrive*rightDrive) + (upDrive*upDrive));
        if (driveMag>127){driveMag=127;}
        
        // Check if magnitude is less than min. threshold & make zero if yes
        if (driveMag<JOYZONE){
            driveMag=0;
        }
        // Check if turn is less than min. threshold & make zero if yes
        if (abs(turn)<JOYZONE){
            turn=0;
        }
        
        // Find direction robot is facing
        gyroAng=gyroDir/10;
        
        // Change robot's reference frame to be relative to rest of world
        driveAngle=driveAngle-gyroAng;
        
        // Clamp angle to be [0,360] after doing subtraction
        if (driveAngle<0)
            driveAngle=360+driveAngle;
        
        // Set each drive motor to correct add/sub or sin/cos
        motorSlew[driveFL] = driveMag*( cosDegrees(driveAngle) - sinDegrees(driveAngle) ) + turn;
        motorSlew[driveFR] =  driveMag*( (-cosDegrees(driveAngle)) - sinDegrees(driveAngle) ) + turn;
        motorSlew[driveBL] = -driveMag*( cosDegrees(driveAngle) + sinDegrees(driveAngle) ) - turn;
        motorSlew[driveBR] =  -driveMag*( sinDegrees(driveAngle) - cosDegrees(driveAngle) ) - turn;
        
        // Pause to allow other tasks to run
        wait1Msec(20);
    }
}



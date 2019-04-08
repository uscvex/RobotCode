
// Tark to run manipulator
task runManip()
{
    bool justFlipped = false;
    bool justRaised = false;
    while (true) {
        
        // Track current position, set speeds to 0 at start of each loop
        wristSpeed = 0;
        flipSpeed = 0;
        flipPosition = -(-4000.0 + (float)SensorValue(flipPot)) / (40-4);
        wristPosition = (3200.0 - (float)SensorValue(wristPot)) / (32.0-5.0) - (10*(flipPosition < 50));

        // Check if button is pressed & not already running auto-flip
        if (vexRT[Btn6U] && flipStep <=0) {
            // Start auto-flip w/ quick disabled
            flipStep = 1;
            quickToss = false;
        }
        if (vexRT[Btn6D] && flipStep <=0) {
            // Start auto-flip w/ quick enabled
            flipStep = 1;
            quickToss = true;
        }
        
        // State-machine for auto-flip
        // If we are running auto-flip
        if (flipStep == 1) {
            // Wait until user releases the button before continuing
            if (!vexRT[Btn6U]) {
                flipStep++;
            }
        }
        if (flipStep == 2) {
            // Move wrist up
            wristSeek = 60;
            // Wait until it is above threshold before continuing
            if (wristPosition > 50) {
                flipStep++;
            }
        }
        if (flipStep == 3) {
            // Hold the wrist up, and flip the flipper
            wristSeek = 60;
            flipSeek = 1;
            // Wait until flipper flipped, and button is pressed to continue
            if (flipSeek < 30 && (vexRT[Btn6U]||vexRT[Btn6D])) {
                flipStep++;
                // If quick-tossing, skip next step
                if (quickToss) {
                    flipStep++;
                }
                // Clear a timer
                clearTimer(T1);
            }
        }
        if (flipStep == 4) {
            // Gently lower the wrist
            wristSeek = 60 - (wristDownSpeed*time1(T1))/1000;
            flipSeek = 1;
            // Wait until wrist is down
            if (wristPosition < wristHoldPosition) {
                // Skip next step
                flipStep = 6;
            }
        }
        if (flipStep == 5) {
            wristSeek = 60;
            flipSeek = 1;
            // Wait until flipper is all the way round before continueing
            if (flipPosition < 5) {
                flipStep++;
            }
        }
        if (flipStep == 6) {
            // Move wrist down
            flipSeek = 1;
            wristSeek = wristHoldPosition;
            // Wait until it's down
            if (wristPosition < wristHoldPosition) {
                // And reset auto-flip flags
                flipStep = -1;
                flipSeek = -1;
                wristSeek = wristHoldPosition;
            }
        }
        
        // Lerps for wrist and flip
        // If we want to auto-seek the wrist,
        if (wristSeek >= 0) {
            // Run at a speed proportional to the distance left to go
            wristSpeed = -(wristSeek - wristPosition) * wristSeekRate;
        }
        // If we want to auto-seek the wrist,
        if (flipSeek >= 0) {
            // Run at a speed proportional to the distance left to go
            flipSpeed = -(flipSeek - flipPosition) * flipSeekRate;
        }
        
        // Manual overrides
        // If we press the 'abort' button
        if (vexRT[Btn7U]) {
            // Clear all auto-seek flags
            wristSeek = -1;
            flipSeek = -1;
            flipStep = -1;
        }
        // If we press the manual wrist up button
        if (vexRT[Btn8R]) {
            // Clear flag and set speed
            wristSpeed = 127;
            wristSeek = -1;
        }
        // If we press the manual wrist down button
        if (vexRT[Btn8D]) {
            // Clear flag and set speed
            wristSpeed = -127;
            wristSeek = -1;
        }
        // If we press the manual flip left button
        if (vexRT[Btn8L]) {
            // Clear flag and set speed
            flipSpeed = 127;
            flipSeek = -1;
        }
        // If we press the manual flip right button
        if (vexRT[Btn8U]) {
            // Clear flag and set speed
            flipSpeed = -127;
            flipSeek = -1;
        }
        
        // Send values to motors
        motor[wristMotor] = wristSpeed;
        motor[flipMotor] = flipSpeed;
        
        // Pause to let other tasks run
        wait1Msec(10);
    }
}


























































task runLift() {
    int lastHigh = 0;
    int lastLow = 0;
    
    while (1) {
        
        liftSpeed = 0;
        
        // Calculate current position
        liftPosition = -(SensorValue(liftREnc)*100)/450 + liftOffset;
        
        // Calibrate lift position
        // If the lift is below mechanical stop, adjust position
        while (liftPosition < 0) {
            liftPosition++;
            liftOffset++;
        }
        // If the lift is above mechanical stop, adjust position
        while (liftPosition > 101) {
            liftPosition--;
            liftOffset--;
        }
        
        // If we press high lift toggle button (high pole)
        if (vexRT[Btn5U]) {
            // If we've not just toggled
            if (lastHigh == 0) {
                // If it is up, move down
                if (liftSeek == liftHigh) {
                    liftSeek = 0;
                }
                else {
                    // Otherwise, move up
                    liftSeek = liftHigh;
                }
            }
            // Remember that we've just toggled
            lastHigh = 1;
        }
        else {
            // We haven't just toggled
            lastHigh = 0;
        }
        
        // If we press low lift toggle button (low pole)
        if (vexRT[Btn5D]) {
            // If we've not just toggled
            if (lastLow == 0) {
                // If it is up, move down
                if (liftSeek == liftLow) {
                    liftSeek = 0;
                }
                else {
                    // Otherwise, move up
                    liftSeek = liftLow;
                }
            }
            // Remember that we've just toggled
            lastLow = 1;
        }
        else {
            // We haven't just toggled
            lastLow = 0;
        }
        
        // If we want to seek
        if (liftSeek >= 0) {
            // Run lift motors at power proportional to distance to go
            liftSpeed = -(liftPosition - liftSeek)*liftSeekRate;
        }
        
        // Manual Overrides
        // Manual up
        if (vexRT[Btn7R]) {
            // Set speed and clear flags
            liftSpeed = 127;
            liftSeek = -1;
        }
        // Manual down
        if (vexRT[Btn7D]) {
            // Set speed and clear flags
            liftSpeed = -127;
            liftSeek = -1;
        }
        
        // Set Motors
        motor[liftR] = liftSpeed;
        
        // Pause to let other tasks run
        wait1Msec(10);
    }
}


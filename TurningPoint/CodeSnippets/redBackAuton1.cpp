
// AUTONOMOUS CODE FOR BLUE BOT - RED TILE
double redBackAuton[] = {
    0,                                  // START FACING 0°
    
    INTAKE_FLIP,                        // START INTAKE IN REVERSE
    FLIP,                               // FLIP SO FORKS DON'T DRIFT
    WRISTSEEK,WRIST_VERTICAL_POS,       // RAISE WRIST
    
    DRIVE,70,0,DISTANCE,0.3,1,          // DRIVE TO DELIVER BALL
    PAUSE,0.25,                         // PAUSE TO LET BALL LEAVE
    DRIVE,-70,0,DISTANCE,0.7,2,         // DRIVE BACKWARDS TO LINE UP
    PAUSE,0.25,                         // SHORT PAUSE
    TURN,265,2,                         // TURN TO FACE CAP
    INTAKE_ON,                          // TURN INTAKE ON
    WRISTSEEK,WRIST_FORWARD_POS,        // LOWER FORKS
    
    DRIVE,100,265,DISTANCE,1.75,2,      // DRIVE TO GET BALL
    PAUSE,0.25,                         // PAUSE TO LET BALL IN
    DRIVE,-100,260,DISTANCE,0.75,1,     // DRIVE AWAY FROM CAP
    
    TURN,0,0.5,                         // START CCW TURN
    TURN,80,2,                          // TURN TURN TO FACE CAP
    TURN,80,2,                          // TURN TO FACE CAP
    
    DRIVE,-50,80,DISTANCE,0.75,3,       // DRIVE TO GET CAP
    WRISTSEEK,WRIST_VERTICAL_POS,       // PICK UP CAP
    PAUSE,0.5,                          // SHORT PAUSE
    
    TURN,300,2,                         // TURN TO LINE UP WITH POLE
    DRIVE,-100,300,DISTANCE,0.6,2,      // DRIVE NEAR TO POLE
    TURN,250,2,                         // TURN TO FACE POLE
    ARMSEEK,ARM_POS_LOW,                // RAISE ARM
    PAUSE,1,                            // WAIT FOR ARM TO RAISE
    DRIVE,-70,250,DISTANCE,1.85,4,      // DRIVE TO POLE
    DRIVE,40,250,0.2,                   // DRIVE BACK A LITTLE
    PAUSE,2,                            // WAIT TO DECREASE WOBBLE
    
    WRISTSEEK,WRIST_FORWARD_DROP_POS,   // DROP CAP #1
    PAUSE,0.5,                          // PAUSE TO LET IT SCORE
    DRIVE,70,270,DISTANCE,0.5,2,        // DRIVE AWAY FROM POLE
    WRISTSEEK,WRIST_FORWARD_POS,        // LOWER WRIST
    ARMSEEK,1,                          // LOWER ARM
    
    TURN,155,2,                         // TURN TO FACE WALL
    DRIVE,100,155,0.75,                 // DRIVE AGAINST WALL
    PAUSE,0.25,                         // SHORT PAUSE
    SET_GYRO,180,                       // RESET GYRO
    PAUSE,0.25,                         // SHORT PAUSE
    
    DRIVE,-100,180,DISTANCE,0.2,1,      // DRIVE TO LINE UP WITH CAP
    TURN,80,2,                          // TURN TO FACE CAP
    DRIVE,-60,80,DISTANCE,1.1,3,        // DRIVE TO CAP
    WRISTSEEK,WRIST_VERTICAL_POS,       // PICK UP CAP
    PAUSE,0.5,                          // SHORT PAUSE
    
    DRIVE,60,80,DISTANCE,0.3,1,         // DRIVE LINE UP
    TURN,0,2,                           // TURN TO LINE UP WITH POLE
    DRIVE,100,0,DISTANCE,0.5,2,         // DRIVE NEAR POLE
    TURN,340,2,                         // TURN TO FACE POLE
    ARMSEEK,ARM_POS_LOW,                // RAISE ARM
    PAUSE,1,                            // PAUSE TO LET ARM LIFT
    FLIP,                               // FLIP CAP
    PAUSE,0.5,                          // LET CAP FLIP
    
    DRIVE,-100,340,DISTANCE,0.75,2,     // DRIVE TO POLE
    DRIVE,40,340,0.2,                   // DRIVE BACK A LITTLE
    PAUSE,1,                            // PAUSE TO DECREASE WOBBLE
    WRISTSEEK,WRIST_FORWARD_DROP_POS,   // DROP CAP #2
    PAUSE,0.5,                          // PAUSE TO LET CAP DROP
    
    START_COAST,                        // ACTIVATE FLYWHEEL
    DRIVE,70,0,DISTANCE,0.5,2,          // DRIVE AWAY FROM POLE
    WRISTSEEK,WRIST_FORWARD_POS,        // LOWER WRIST
    ARMSEEK,1,                          // LOWER ARM
    
    TURN,310,2,                         // TURN TO FACE OPPONENT FLAG
    FIRE,LEFT,540,                      // SHOOT BALL AT OPPONENT FLAG
    PAUSE,FIRED,5,                      // PAUSE UNTIL FIRED
    
    END                                 // END OF ROUTINE
};

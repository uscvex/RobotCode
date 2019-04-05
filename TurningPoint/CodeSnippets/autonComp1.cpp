
// AUTON ROUTINE FOR MULTI-BOT STARTING ON RED
double redAuton[] = {
    180,                            // ROBOT STARTS 180° RELATIVE TO FIELD
    
    PAUSE,3,                        // WAIT 3 SECONDS
    INTAKE_ON,                      // TURN INTAKE ON
    PAUSE,GOTBALL,5,                // WAIT TILL GOT BALL (OTHER BOT ROLLS IT)
    INTAKE_OFF,                     // TURN INTAKE OFF
    
    TURN,270,2,                     // START TURN GOING CLOCKWISE
    TURN,355,3,                     // TURN TO FACE FLAGS
    DRIVE,100,0,DISTANCE,0.1,1,     // DRIVE CLOSE ENOUGH TO SHOOT
    
    FIRE,                           // SHOOT HIGH FLAG
    PAUSE,0.5,                      // WAIT 0.5 SECONDS
    PAUSE,FIRED,10,                 // WAIT UNTIL SHOT THE BALL
    PAUSE,0.5,                      // WAIT 0.5 SECONDS
    INTAKE_ON,                      // TURN INTAKE ON
    STOP_FLYWHEEL,                  // TURN FLYWHEEL OFF
    
    DRIVE,100,0,DISTANCE,1.35,1,    // DRIVE TO LOW FLAG RANGE
    WRISTSEEK,WRIST_FORWARD_POS,    // HIT LOW FLAG WITH FORKS
    DRIVE,-127,0,DISTANCE,0.5,1,    // DRIVE BACK A LITTLE
    
    FIRE,                           // SHOOT AT MIDDLE FLAG
    PAUSE,0.5,                      // WAIT 0.5 SECONDS
    PAUSE,FIRED,10,                 // WAIT UNTIL FIRED
    INTAKE_OFF,                     // TURN INTAKE OFF
    PAUSE,0.5,                      // WAIT 0.5 SECONDS
    STOP_FLYWHEEL,                  // TURN FLYWHEEL OFF
    
    WRISTSEEK,WRIST_VERTICAL_POS,   // PUT WRIST BACK UP
    PAUSE,1,                        // WAIT FOR FORKS TO GET UP
    
    DRIVE,-100,0,DISTANCE,0.65,2,   // DRIVE AWAY FROM FLAG
    TURN,270,2,                     // TURN TO FACE
    
    DRIVE,-70,270,0.5,              // RESET AGAINST WALL
    
    WRISTSEEK,(WRIST_FORWARD_POS),  // PUT FLIPPER DOWN FOR CAP
    INTAKE_ON,                      // TURN INTAKE ON
    PAUSE,2,                        // PAUSE FOR 2 SECONDS
    
    DRIVE,50,270,DISTANCE,1,4,      // DRIVE TO CAP
    FLIP,                           // FLIP CAP
    PAUSE,1,                        // WAIT 1 SECOND TO FLIP
    
    DRIVE,-70,270,DISTANCE,1,2,     // LINE UP FOR NEXT CAP
    TURN,225,2,                     // TURN TO FACE NEXT CAP
    DRIVE,70,225,DISTANCE,1,2,      // DRIVE TO NEXT CAP
    DRIVE,70,270,DISTANCE,1,2,      // DRIVE A LITTLE MORE
    
    TURN,315,1,                     // TURN TO KNOCK CAP
    DRIVE,-70,270,DISTANCE,1,2,     // MOVE AWAY FROM CAP
    TURN,325,3,                     // TURN TO FACE FLAGS
    
    WRISTSEEK,WRIST_VERTICAL_POS,   // PUT FORKS UP
    
    DRIVE,127,325,DISTANCE,1,2,     // DRIVE TO CORRECT DISTANCE TO FIRE
    
    FIRE,                           // LONG SHOT - SHOOT
    PAUSE,0.5,                      // WAIT 0.5 SECONDS
    PAUSE,FIRED,10,                 // WAIT UNTIL SHOT
    INTAKE_OFF,                     // TURN INTAKE OFF
    PAUSE,0.5,                      // WAIT FOR 0.5 SECONDS
    
    STOP_FLYWHEEL,                  // TURN FLYWHEEL OFF
    
    DRIVE,100,325,DISTANCE,1,2,     // DRIVE TO HIT BOTTOM FLAG
    
    WRISTSEEK,WRIST_FORWARD_POS,    // HIT WITH FORK
    PAUSE,1,                        // WAIT 1 SECOND
    WRISTSEEK,WRIST_VERTICAL_POS,   // RAISE FORKS
    
    DRIVE,-100,325,DISTANCE,1.75,2, // DRIVE AWAY FROM FLAG
    
    END                             // END OF ROUTINE
};

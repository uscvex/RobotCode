

// SIMPLE AUTON ROUTINE FOR SKILLS BOT IF USED IN MATCH
double redAuton[] = {
    0,                                  // START AT 0°
    
    WRISTSEEK,WRIST_FORWARD_POS,        // LOWER WRIST
    FLIPSEEK,FLIP_POS1,                 // FLIPPER SEEK TO STOP DRIFT
    DRIVE,100,0,DISTANCE,0.55,1,        // DRIVE TO SEND BALL TO RED BOT
    DRIVE,-100,0,DISTANCE,0.55,1,       // DRIVE BACK
    
    TURN,90,2,                          // TURN TO FACE CAP
    DRIVE,-127,90,DISTANCE,1.5,2,       // DRIVE TO KNOCK CAP
    DRIVE,-127,90,DISTANCE,0.5,2,       // DRIVE MORE
    DRIVE,127,90,DISTANCE,1,2,          // DRIVE BACK
    
    TURN,40,2,                          // TURN TO FACE NEXT CAP
    DRIVE,-60,40,DISTANCE,1,3,          // DRIVE TO CAP
    FIRE,                               // FIRE SO CATAPULT GOES DOWN
    PAUSE,3,                            // WAIT WHILE CATAPULT LOWERS
    
    WRISTSEEK,WRIST_VERTICAL_POS,       // RAISE WRIST TO DUMP CAP BALLS IN
    PAUSE,1,                            // WAIT FOR BALLS TO LAND
    DRIVE,60,40,DISTANCE,0.3,1,         // DRIVE BACK
    FLIP,                               // FLIP THE CAP
    WRISTSEEK,WRIST_FORWARD_POS,        // AND PUT IT DOWN
    
    TURN,320,2,                         // TURN TO FACE OPPONENT FLAGS
    PAUSE,UNTIL,35,                     // WAIT UNTIL NEAR THE END
    FIRE,                               // FIRE AT OPPONENT FLAGS
    PAUSE,FIRED,5,                      // PAUSE UNTIL FIRE DONE
    
    TURN,350,2,                         // TURN TO FACE PLATFORM
    DRIVE,100,355,DISTANCE,0.85,2,      // DRIVE TO PLATFORM
    PAUSE,1,                            // SHORT PAUSE
    DRIVE,60,0,DISTANCE,2,2,            // PARK
    
    END                                 // END OF ROUTINE
};

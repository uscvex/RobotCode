
// PROGRAMMING SKILLS ROUTINE FOR CATAPULT BOT
double skills[] = {
    90,                                 // START FACING 90°
    
    WRISTSEEK,WRIST_FORWARD_POS*0.5,    // LOWER WRIST
    DRAW_BACK,                          // DRAW BACK CATAPULT
    
    DRIVE,-100,90,BLACK,1,              // DRIVE OFF TILE
    DRIVE,-127,90,DISTANCE,1.5,2,       // DRIVE TO KNOCK CAP
    DRIVE,127,90,DISTANCE,0.1,1,        // DRIVE BACK
    
    TURN,0,2,                           // TURN TO PICK UP
    FLIPSEEK,FLIP_POS1,                 // FLIPPER SEEK TO STOP DRIFE
    WRISTSEEK,WRIST_FORWARD_POS,        // PUT FLIPPER DOWN
    PAUSE,0.5,                          // PAUSE WHILE FLIPPER LOWERS

    DRIVE,-40,0,DISTANCE,0.6,2,         // DRIVE TO GET CAP
    WRISTSEEK,WRIST_VERTICAL_POS,       // LIFT CAP
    DRIVE,40,0,DISTANCE,0.1,2,          // DRIVE FORWARDS A LITTLE
    PAUSE,1.5,                          // PAUSE WHILE CAP LIFTS
    
    FLIP,                               // FLIP CAP
    PAUSE,0.5,                          // WAIT FOR CAP TO FLIP
    WRISTSEEK,WRIST_FORWARD_POS,        // PUT CAP DOWN
    PAUSE,0.5,                          // PAUSE TO LET CAP DOWN
    
    DRIVE,127,0,DISTANCE,0.5,2,         // DRIVE AWAY
    FLIP,                               // PUT FLIPPER STRAIGHT
    TURN,45,2,                          // TURN TO FACE NEXT CAP
    DRIVE,-127,45,DISTANCE,1.25,2,      // DRIVE TO OTHER CAP
    WRISTSEEK,WRIST_VERTICAL_POS,       // LIFT CAP
    PAUSE,0.25,                         // PAUSE TO LET LIFT
    FLIP,                               // FLIP OTHER CAP
    WRISTSEEK,WRIST_FORWARD_POS,        // LOWER CAP
    
    TURN,45,0.5,                        // TURN TO HELP FLIP
    DRIVE,127,45,DISTANCE,1,2,          // DRIVE AWAY
    WRISTSEEK,WRIST_FORWARD_POS,        // MAKE SURE WRIST IS DOWN
    TURN,90,2,                          // TURN TO FACE START TILE
    
    WRISTSEEK,WRIST_VERTICAL_POS,       // RAISE WRIST
    DRIVE,127,90,DISTANCE,2,2,          // DRIVE BACK TO TILE
    DRIVE,-127,90,SONAR,0.3,1,          // DRIVE AWAY FROM WALL
    FLIP,                               // REVERT FLIPPER
    
    WRISTSEEK,WRIST_FORWARD_POS/2,      // LOWER WRIST HALF WAY
    TURN,3,2,                           // TURN TO FACE FLAGS
    DRIVE,127,3,WHITE,2,                // DRIVE ONTO TILE
    DRIVE,127,3,BLACK,2,                // DRIVE OFF TILE
    DRIVE,127,3,WHITE,2,                // DRIVE ONTO NEXT TILE
    DRIVE,70,3,BLACK,2,                 // DRIVE OFF TILE
    DRIVE,-60,3,0.1,                    // DRIVE TO BREAK
    DRIVE,-60,3,DISTANCE,0.1,1,         // DRIVE BACK A LITTLE MORE
    TURN,0,2,                           // MAKE SURE LINED UP
    
    TURN,350,2,                         // TURN TO FACE FLAGS
    WRISTSEEK,WRIST_VERTICAL_POS-50,    // MOVE WRIST UP TO MAKE SURE BALLS IN
    TURN_AIM,BLUE_FLAG,LEFT,2,          // AIM AT LEFT-MOST BLUE FLAG
    WRISTSEEK,WRIST_FORWARD_POS/2,      // LOWER WRIST AGAIN
    PAUSE,0.25,                         // SHORT PAUSE
    
    FIRE_AIM,                           // AIM AND FIRE AT FLAGS
    PAUSE,FIRED,2,                      // PAUSE UNTIL FIRED
    PAUSE,0.5,                          // PAUSE A LITTLE MORE
    DRIVE,127,CDIR,DISTANCE,2,2,        // DRIVE TO HIT BOTTOM FLAG
    TURN,3,2,                           // TURN STRAIGHT
    DRIVE,-127,3,0.2,                   // DRIVE BACK
    TURN,3,2,                           // MAKE SURE STRAIGHT
    
    DRIVE,-127,0,WHITE,2,               // DRIVE TO START TILE
    DRIVE,-127,0,DISTANCE,0.1,2,        // DRIVE A LITTLE MORE
    DRIVE,127,0,DISTANCE,0.05,2,        // DRIVE TO BREAK
    WRISTSEEK,WRIST_FORWARD_POS,        // LOWER WRIST
    TURN,125,2,                         // TURN TO FACE NEXT CAP
    
    DRIVE,-40,125,DISTANCE,1.75,2,      // DRIVE TO NEXT CAP
    PAUSE,0.25,                         // SHORT PAUSE
    WRISTSEEK,WRIST_VERTICAL_POS,       // LIFT CAP
    DRIVE,40,125,DISTANCE,0.1,2,        // DRIVE BACK A LITTLE
    PAUSE,1.5,                          // PAUSE WHILE CAP LIFTS
    FLIP,                               // FLIP CAP
    PAUSE,0.5,                          // PAUSE TO LET FLIP
    WRISTSEEK,WRIST_FORWARD_POS,        // PUT CAP DOWN
    
    TURN,180,2,                         // TURN FOR NEXT MOVE
    DRIVE,100,180,DISTANCE,0.8,2,       // DRIVE TO LINE UP FOR CAP
    FLIP,                               // PUT FORKS UPRIGHT
    WRISTSEEK,WRIST_FORWARD_POS*0.75,   // LOWER WRIST HALF WAY
    TURN,90,2,                          // TURN TO FACE CAP
    DRIVE,-127,90,DISTANCE,1.35,2,      // DRIVE TO KNOCK CAP
    
    TURN,0,2,                           // TURN TO FACE FLAGS
    DRIVE,127,0,DISTANCE,0.1,2,         // DRIVE TO CORRECT RANGE
    TURN,0,2,                           // AIM AT FLAGS
    TURN,350,1,                         // AIM AT FLAGS
    
    WRISTSEEK,WRIST_VERTICAL_POS-50,    // RAISE WRIST TO KNOCK BALLS
    TURN_AIM,BLUE_FLAG,LEFT,2,          // AIM AT LEFT-MOST BLUE FLAG
    WRISTSEEK,WRIST_FORWARD_POS/2,      // LOWER WRIST
    PAUSE,0.25,                         // SHORT PAUSE
    
    FIRE_AIM,                           // FIRE AT FLAGS
    PAUSE,FIRED,2,                      // PAUSE UNTIL FIRED
    PAUSE,0.5,                          // SHORT PAUSE
    
    DRIVE,127,CDIR,DISTANCE,1.5,2,      // DRIVE TO HIT BOTTOM FLAG
    TURN,0,2,                           // TURN STRAIGHT
    DRIVE,-100,0,DISTANCE,2,2,          // DRIVE BACK
    
    DRIVE,127,0,DISTANCE,0.5,2,         // DRIVE FORWARD A LITTLE
    TURN,90,2,                          // TURN TO FACE NEXT CAP
    WRISTSEEK,WRIST_FORWARD_POS,        // LOWER FORKS
    DRIVE,-127,90,DISTANCE,1,2,         // DRIVE CLOSE TO CAP
    
    DRIVE,-40,90,DISTANCE,1,3,          // DRIVE TO GET CAP
    WRISTSEEK,WRIST_VERTICAL_POS,       // LIFT CAP
    DRIVE,-40,90,DISTANCE,1,2,          // DRIVE BACK
    PAUSE,1,                            // PAUSE WHILE CAP LIFTS
    FLIP,                               // FLIP THE CAP
    WRISTSEEK,WRIST_FORWARD_POS,        // PUT THE CAP DOWN
    
    DRIVE,100,90,DISTANCE,1,2,          // DRIVE FORWARDS
    TURN,0,2,                           // TURN TO FACE FLAGS
    WRISTSEEK,WRIST_VERTICAL_POS,       // RAISE WRIST
    DRIVE,127,0,DISTANCE,1.5,2,         // DRIVE TO HIT BOTTOM FLAG
    TURN,0,2,                           // TURN STRAGIGH
    DRIVE,-100,0,DISTANCE,2,2,          // DRIVE BACK TO LINE UP FOR SHOT
    TURN,345,1,                         // TURN TO FACE FLAGS
    
    WRISTSEEK,WRIST_VERTICAL_POS-50,    // RAISE WRIST TO KNOCK BALLS
    TURN_AIM,BLUE_FLAG,CENTER,2,        // AIM AT CENTER-MOST BLUE FLAG
    WRISTSEEK,WRIST_FORWARD_POS/2,      // LOWER WRIST
    PAUSE,0.25,                         // SHORT PAUSE
    
    FIRE_AIM,                           // FIRE AT LAST FLAGS
    PAUSE,FIRED,2,                      // PAUSE UNTIL FIRED
    PAUSE,0.5,                          // SHORT PAUSE
    
    END                                 // END OF ROUTINE
};

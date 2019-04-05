
// AUTONOMOUS CODE FOR RED BOT - RED SQUARE
double redAuton[] = {
    180,                                // ROBOT STARTS FACING 180°
    
    DRIVE,50,180,DISTANCE,0.1,1,        // DRIVE WITHIN EXPANSION ZONE
    
    WRISTSEEK,-1000,                    // DEPLOY FLIPPER
    PAUSE,1.5,                          // PAUSE FOR DEPLOY
    FLIPSEEK,FLIP_POS1,                 // HOLD FLIPPER STILL
    WRISTSEEK,WRIST_FORWARD_POS,        // LOWER FORKS
    PAUSE,0.5,                          // DEPLOY DONE
    
    DRIVE,-70,180,DISTANCE,0.6,1,       // DRIVE TO LINE UP FOR CAP
    TURN,135,1,                         // AIM AT CAP
    TURN,135,2,                         // AIM AT CAP
    
    DRIVE,-50,135,DISTANCE,1,2,         // DRIVE TO CAP 1
    WRISTSEEK,WRIST_VERTICAL_POS,       // PICK IT UP
    PAUSE,1,                            // TIME TO PICK UP
    
    DRIVE,70,135,WHITE_R,2,             // DRIVE TILL RIGHT SENSOR IS OVER START TILE
    TURN,180,2,                         // TURN FOR NEXT DRIVE
    DRIVE,70,180,BLACK_R,2,             // DRIVE UNTIL OFF START TILE
    DRIVE,-90,180,DISTANCE,0.1,1,       // DRIVE TO LINE UP
    TURN,90,2,                          // TURN TO LINE UP
    
    DRIVE,-90,90,0.1,                   // DRIVE AWAY FROM POLE
    TURN,90,2,                          // TURN TO LINE UP
    DRIVE,70,90,0.1,                    // DRIVE BACK TO WALL
    
    DRIVE,50,90,DISTANCE,0.4,2,         // DRIVE TO WALL
    
    STACK_HIGH,                         // STACK CAP 1
    PAUSE,STACKED,10,                   // WAIT TILL STACKED
    
    TURN,90,2,                          // TURN STRAIGHT
    DRIVE,-90,90,DISTANCE,0.5,1,        // DRIVE AWAY FROM POLE
    TURN,0,2,                           // TURN TO FACE FLAGS
    DRIVE,90,0,WHITE_L,2,               // DRIVE ONTO TILE
    DRIVE,90,0,0.1,                     // MAKE SURE ON TILE
    DRIVE,90,0,BLACK_B,2,               // DRIVE OFF TILE
    DRIVE,-90,0,0.1,                    // DRIVE TO BREAK
    DRIVE,-90,0,DISTANCE,1,2,           // DRIVE TO LINE UP
    
    TURN,270,2,                         // TURN TO FACE CAP
    INTAKE_ON,                          // START INTAKE
    WRISTSEEK,WRIST_VERTICAL_POS,       // STOP DEPLOY
    DRIVE,80,270,BLACK_B,1,             // DRIVE OFF TILE
    DRIVE,80,270,DISTANCE,1.4,2,        // DRIVE TO GET BALL
    DRIVE,-80,270,DISTANCE,0.125,2,     // DRIVE BACK FROM CAP
    
    TURN,345,2,                         // TURN TO FACE FLAGS
    DRIVE,90,345,DISTANCE,0.65,2,       // DRIVE TO FLAG DISTANCE
    TURN,330,2,                         // TURN TO FACE FLAGS
    TURN_AIM,BLUE_FLAG,CENTER,2,        // AIM AT CENTRE BLUE FLAG
    
    PAUSE,2,                            // PAUSE AFTER TURN
    PAUSE,UNTIL,35,                     // WAIT AS LATE AS POSSIBLE
    START_COAST,                        // SPINUP FLYWHEEL
    TURN_AIM,BLUE_FLAG,CENTER,2,        // AIM AT CENTRE BLUE FLAG
    PAUSE,2,                            // PAUSE AFTER TURNING
    PAUSE,UNTIL,41.25,                  // WAIT AS LATE AS POSSIBLE
    
    FIRE_AIM,TOP,                       // SHOOT TOP FLAG
    PAUSE,FIRED,FIRE_TRY_TIME,          // WAIT TILL SHOT
    PAUSE,0.1,                          // SHORT PAUSE
    STOP_FIRE,                          // STOP FLYWHEEL
    
    DRIVE,127,CDIR,DISTANCE,            // CONTINUED ON NEXT LINE
    MIDDLE_FLAG_DIST,2,                 // DRIVE TO MIDDLE FLAG DIST
    FIRE_AIM,TOP,                       // SHOOT MIDDLE FLAG
    PAUSE,FIRED,FIRE_TRY_TIME,          // WAIT TILL SHOT
    PAUSE,0.1,                          // SHORT PAUSE
    STOP_FIRE,                          // STOP FLYWHEEL
    
    INTAKE_OFF,                         // TURN INTAKE OFF
    DRIVE,127,CDIR,0.5,                 // DRIVE TO TOGGLE BOTTOM FLAG
    DRIVE,-127,0,0.5,                   // DRIVE AWAY FROM FLAG
    INTAKE_ON,                          // TURN INTAKE BACK ON
    
    END                                 // END OF ROUTINE
};

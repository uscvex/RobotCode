
// SKILLS ROUTINE FOR RED-BOT
double skills[] = {
    270,                                // START FACING 270°
    
    WRISTSEEK,-1000,                    // DEPLOY FLIPPER
    FLIPSEEK,FLIP_POS1,                 // MAKE SURE FLIPPER STRAIGHT
    INTAKE_ON,                          // START INTAKE
    SCRAPER,SCRAPER_DOWN_POS-10,        // LOWER SCRAPER
    PAUSE,0.125,                        // SHORT PAUSE
    DRIVE,90,270,BLACK_B,0.5,           // DRIVE OFF TILE
    DRIVE,90,270,DISTANCE,1.25,2,       // DRIVE TO FLIP CAP
    WRISTSEEK,WRIST_VERTICAL_POS,       // STOP DEPLOY
    
    SCRAPER,SCRAPER_FLIP_POS,           // LOWER SCRAPER FOR NEXT FLIP
    TURN,220,2,                         // TURN TO FACE NEXT CAP
    DRIVE,127,220,DISTANCE,1,2,         // DRIVE TO NEXT CAP
    SCRAPER,SCRAPER_UP_POS,             // FLIP NEXT CAP
    PAUSE,SCRAPER_UP,1,                 // PAUSE UNTIL SCRAPER IS UP
    DRIVE,-127,220,DISTANCE,1,2,        // DRIVE AWAY FROM CAP
    
    SCRAPER,SCRAPER_FLIP_POS,           // LOWER SCRAPER FOR NEXT FLIP
    TURN,160,2,                         // TURN TO FACE NEXT CAP
    DRIVE,127,160,DISTANCE,0.5,1,       // DRIVE TO NEXT CAP
    SCRAPER,SCRAPER_UP_POS,             // FLIP NEXT CAP
    PAUSE,SCRAPER_UP,1,                 // PAUSE UNTIL SCRAPER IS UP
    DRIVE,-127,160,DISTANCE,0.5,2,      // DRIVE AWAY FROM CAP
    
    START_COAST,                        // LET FLYWHEEL SPIN UP TO SPEED
    
    TURN,90,2,                          // TURN TO FACE START TILE
    DRIVE,127,90,WHITE_R,2,             // DRIVE TO START TILE
    DRIVE,-80,90,0.05,                  // BREAK TO STOP
    TURN,0,2,                           // TURN TO FACE FLAGS
    
    DRIVE,127,1,BLACK_L,2,              // DRIVE OFF START TILE
    DRIVE,127,1,0.1,
    DRIVE,127,1,WHITE_L,2,              // DRIVE TO NEXT TILE
    DRIVE,127,2,DISTANCE,0.8,1,         // LINE UP FOR SHOT
    
    FIRE_AIM,TOP,                       // SHOOT TOP FLAG
    PAUSE,FIRED,5,                      // WAIT TILL SHOT
    PAUSE,0.1,                          // SHORT PAUSE
    STOP_FIRE,                          // STOP FLYWHEEL
    
    DRIVE,127,CDIR,DISTANCE,0.9,2,      // DRIVE TO MIDDLE FLAG DIST
    FIRE_AIM,TOP,                       // SHOOT MIDDLE FLAG
    PAUSE,FIRED,5,                      // WAIT TILL SHOT
    PAUSE,0.1,                          // SHORT PAUSE
    STOP_FIRE,                          // STOP FLYWHEEL
    
    INTAKE_OFF,                         // TURN INTAKE OFF
    DRIVE,127,0,0.5,                    // DRIVE TO TOGGLE BOTTOM FLAG
    SCRAPER,SCRAPER_DOWN_POS,           // LOWER SCRAPER SO IT DOESN'T CATCH THE FLAG
    DRIVE,-127,0,0.5,                   // DRIVE AWAY FROM FLAG
    INTAKE_ON,                          // TURN INTAKE BACK ON
    SCRAPER,SCRAPER_UP_POS,             // RAISE SCRAPER AGAIN
    
    TURN,0,2,                           // TURN TO LINE UP
    DRIVE,-127,0,WHITE_L,2,             // DRIVE TO RED TILE
    DRIVE,-127,0,DISTANCE,0.5,1,        // DRIVE TO MIDDLE OF IT
    TURN,90,0,                          // TURN TO FACE WALL
    
    DRIVE,100,90,0.5,                   // DRIVE TO WALL
    SET_GYRO,90,                        // SET GYRO
    
    WRISTSEEK,WRIST_FORWARD_POS,        // LOWER FLIPPER
    TURN,130,2,                         // TURN TO FACE NEXT CAP
    
    DRIVE,-127,130,DISTANCE,1.5,2,      // DRIVE TO NEXT CAP
    WRISTSEEK,WRIST_VERTICAL_POS,       // LIFT CAP
    PAUSE,0.25,                         // SHORT PAUSE
    FLIP,                               // FLIP CAP
    TURN,210,2,                         // TURN TO FACE PLATFORM
    WRISTSEEK,WRIST_FORWARD_POS,        // DROP CAP
    SCRAPER,SCRAPER_UP_POS,             // ENSURE SCRAPER IS UP
    DRIVE,127,210,DISTANCE,1.2,2,       // DRIVE TO PLATFORM
    SCRAPER,SCRAPER_DOWN_POS,           // LOWER SCRAPER
    WRISTSEEK,WRIST_VERTICAL_POS,       // LIFT FLIPPER
    FLIPSEEK,FLIP_POS1,                 // REVERT FLIPPER
    PAUSE,0.5,                          // SHORT PAUSE
    DRIVE,-127,180,DISTANCE,0.25,1,     // DRIVE AWAY FROM PLATFORM
    PAUSE,0.5,                          // LET BALL ROLL IN
    
    SCRAPER,SCRAPER_DOWN_POS-10,        // MOVE SCRAPER TO CAP FLIP POSITION
    
    TURN,270,2,                         // TURN TO FACE NEXT CAP
    DRIVE,90,270,DISTANCE,1,1,          // DRIVE TO FLIP NEXT CAP
    
    DRIVE,-127,270,DISTANCE,0.25,1,     // DRIVE AWAY FROM CAP
    
    TURN,315,2,                         // TURN TO FACE FLAGS
    SCRAPER,SCRAPER_UP_POS,             // LIFT SCRAPER TO SAFETY
    DRIVE,127,0,DISTANCE,0.4,1,         // DRIVE TO CORRECT RANGE
    TURN,0,1,                           // TURN TO FACE FLAGS
    DRIVE,127,0,DISTANCE,0.1,1,         // DRIVE TO CORRECT RANGE
    
    FIRE_AIM,TOP,                       // SHOOT TOP FLAG
    PAUSE,FIRED,5,                      // WAIT TILL SHOT
    PAUSE,0.1,                          // SHORT PAUSE
    STOP_FIRE,                          // STOP FLYWHEEL
    
    DRIVE,127,CDIR,DISTANCE,0.7,2,      // DRIVE TO TOP FLAG DIST
    FIRE_AIM,TOP,                       // SHOOT TOP FLAG
    PAUSE,FIRED,5,                      // WAIT TILL SHOT
    PAUSE,0.1,                          // SHORT PAUSE
    STOP_FIRE,                          // STOP FLYWHEEL
    
    INTAKE_OFF,                         // TURN INTAKE OFF
    DRIVE,127,0,0.5,                    // DRIVE TO TOGGLE BOTTOM FLAG
    SCRAPER,SCRAPER_DOWN_POS,           // LOWER SCRAPER SO IT DOESN'T CATCH THE FLAG
    DRIVE,-127,0,0.25,                  // DRIVE AWAY FROM FLAG
    INTAKE_ON,                          // TURN INTAKE BACK ON
    SCRAPER,SCRAPER_UP_POS,             // RAISE SCRAPER AGAIN
    
    TURN,0,2,                           // TURN TO LINE UP
    DRIVE,-127,0,DISTANCE,0.5,1,        // DRIVE TO LINE UP WITH NEXT CAP
    
    FLIPSEEK,FLIP_POS1,                 // PUT FLIPPER CORRECT WAY UP
    TURN,90,0,                          // TURN TO FACE NEXT CAP
    WRISTSEEK,WRIST_FORWARD_POS,        // LOWER FLIPPER
    DRIVE,-127,90,WHITE_E,1,            // DRIVE ACOSS CENTER WHITE LINE
    DRIVE,-90,90,DISTANCE,2.5,2,        // DRIVE TO GET NEXT CAP (SLOWLY)
    
    WRISTSEEK,WRIST_VERTICAL_POS,       // LIFT CAP
    PAUSE,0.25,                         // PAUSE TO LIFT
    DRIVE,127,90,DISTANCE,0.1,1,        // DRIVE AWAY FROM WALL
    FLIP,                               // FLIP CAP
    TURN,160,2,                         // TURN TO FACE PLATFORM
    WRISTSEEK,WRIST_FORWARD_POS,        // DROP CAP
    DRIVE,127,160,DISTANCE,1.5,2,       // DRIVE TO PLATFORM
    
    SCRAPER,SCRAPER_DOWN_POS,           // LOWER SCRAPER
    WRISTSEEK,WRIST_VERTICAL_POS,       // LIFT FLIPPER
    FLIPSEEK,FLIP_POS1,                 // REVERT FLIPPER
    PAUSE,0.5,                          // SHORT PAUSE
    DRIVE,-127,160,DISTANCE,0.5,1,      // DRIVE AWAY FROM PLATFORM
    PAUSE,0.5,                          // SHORT PAUSE
    
    TURN,90,2,                          // TURN TO FACE NEXT CAP
    SCRAPER,SCRAPER_DOWN_POS-10,        // MOVE SCRAPER TO CAP FLIP POSITION
    DRIVE,127,90,DISTANCE,1.25,2,       // DRIVE TO FLIP CAP
    PAUSE,0.25,
    DRIVE,-127,90,DISTANCE,1.25,2,      // DRIVE TO LINE UP FOR FLAGS
    
    TURN,0,2,                           // TURN TO FACE FLAGS
    SCRAPER,SCRAPER_UP_POS,             // LIFT SCRAPER TO SAFETY
    DRIVE,127,0,DISTANCE,0.1,1,         // DRIVE TO CORRECT RANGE
    
    FIRE_AIM,TOP,                       // SHOOT TOP FLAG
    PAUSE,FIRED,5,                      // WAIT TILL SHOT
    PAUSE,0.1,                          // SHORT PAUSE
    STOP_FIRE,                          // STOP FLYWHEEL
    
    DRIVE,127,CDIR,DISTANCE,0.9,2,      // DRIVE TO TOP FLAG DIST
    FIRE_AIM,TOP,                       // SHOOT TOP FLAG
    PAUSE,FIRED,5,                      // WAIT TILL SHOT
    PAUSE,0.1,                          // SHORT PAUSE
    STOP_FIRE,                          // STOP FLYWHEEL
    
    INTAKE_OFF,                         // TURN INTAKE OFF
    DRIVE,127,0,0.5,                    // DRIVE TO TOGGLE BOTTOM FLAG
    SCRAPER,SCRAPER_DOWN_POS,           // LOWER SCRAPER SO IT DOESN'T CATCH THE FLAG
    DRIVE,-127,0,0.25,                  // DRIVE AWAY FROM FLAG
    INTAKE_ON,                          // TURN INTAKE BACK ON
    SCRAPER,SCRAPER_UP_POS,             // RAISE SCRAPER AGAIN
    
    TURN,20,1,                          // TURN TO LINE UP
    DRIVE,-127,20,WHITE_R,2,            // DRIVE UNTIL ON THE TILE
    DRIVE,-127,20,0.1,                  // DRIVE TO BE FULLY ON TILE
    TURN,0,0.5,                         // TURN TO LINE UP
    DRIVE,-127,0,BLACK_R,2,             // DRIVE UNTIL OFF THE TILE
    DRIVE,-127,0,DISTANCE,0.5,2,        // DRIVE TO LINE UP
    
    TURN,80,2,                          // TURN TO FACE PLATFORM
    INTAKE_ON,                          // ENSURE INTAKE IS RUNNING
    WRISTSEEK,WRIST_FORWARD_POS,        // PUT FLIPPER DOWN
    DRIVE,127,80,DISTANCE,0.5,1,        // DRIVE TO PLATFORM
    DRIVE,127,80,DISTANCE,2.75,4,       // DRIVE UP PLATFORM
    
    END                                 // END OF ROUTINE
};

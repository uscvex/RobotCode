
// SKILLS ROUTINE
double skills[] = {
    270,                                // STARTS FACING 270°
    WRISTSEEK,-1000,                    // DEPLOY FLIPPER
    INTAKE_ON,                          // TURN INTAKE ON
    DRIVE,127,270,DISTANCE,1,2,         // DRIVE TO FLIP CAP
    DRIVE,80,270,DISTANCE,0.7,2,        // DRIVE TO FLIP CAP
    FLIP,                               // FLIP SO FORKS DON'T DRIFT
    WRISTSEEK,WRIST_FORWARD_POS,        // PUT FLIPPER DOWN
    
    DRIVE,-127,270,DISTANCE,0.5,2,      // DRIVE AWAY A LITTLE
    TURN,15,2,                          // TURN TO FACE CAP
    DRIVE,-60,15,DISTANCE,0.6,2,        // DRIVE TO CAP
    WRISTSEEK,WRIST_VERTICAL_POS,       // FLIP CAP
    PAUSE,0.125,                        // SHORT PAUSE
    
    DRIVE,127,15,DISTANCE,0.3,1,        // DRIVE AWAY FROM CAP
    PAUSE,0.25,                         // SHORT PAUSE
    WRISTSEEK,WRIST_FORWARD_POS,        // LOWER FLIPPER
    TURN,50,2,                          // TURN FOR NEXT CAP
    DRIVE,-90,50,DISTANCE,0.9,2,        // DRIVE TO CAP
    WRISTSEEK,WRIST_VERTICAL_POS,       // FLIP CAP
    PAUSE,0.5,                          // SHORT PAUSE
    
    DRIVE,127,50,DISTANCE,0.5,2,        // DRIVE AWAY FROM CAP
    TURN,90,2,                          // AIM AT START TILE
    DRIVE,127,90,SONAR,0.5,2,           // DRIVE BACK TO TILE

    TURN,0,2,                           // TURN TO FACE FLAGS
    START_COAST,                        // START FLYWHEEL SPINUP
    DRIVE,90,0,WHITE_L,0.5,             // DRIVE SO DEF. ON TILE
    DRIVE,127,0,0.5,                    // DRIVE A LITTLE
    DRIVE,127,0,BLACK_L,2,              // DRIVE OFF TILE
    DRIVE,127,0,0.2,                    // DRIVE A LITTLE
    DRIVE,90,0,WHITE_L,2,               // DRIVE TO NEXT TILE
    
    TURN_AIM,BLUE_FLAG,CENTER,2,        // AIM AT LEFT-MOST BLUE FLAG
    DRIVE,90,CDIR,DISTANCE,0.8,2,       // DRIVE TO TOP FLAG DIST
    FIRE_AIM,TOP,                       // SHOOT TOP FLAG
    PAUSE,FIRED,2,                      // WAIT TILL SHOT
    PAUSE,0.5,                          // SHORT PAUSE
    STOP_FIRE,                          // STOP FLYWHEEL
    
    DRIVE,127,CDIR,DISTANCE,0.8,2,      // DRIVE TO MIDDLE FLAG DIST
    FIRE_AIM,MIDDLE,                    // SHOOT MIDDLE FLAG
    PAUSE,FIRED,2,                      // WAIT TILL SHOT
    PAUSE,0.5,                          // SHORT PAUSE
    STOP_FIRE,                          // STOP FLYWHEEL
    STOP_COAST,                         // TURN FLYWHEEL FULLY OFF
    
    DRIVE,127,0,0.3,                    // DRIVE TO TOGGLE BOTTOM FLAG
    TURN,0,0.5,                         // TURN STRAIGHT
    INTAKE_ON,                          // TURN INTAKE ON
    DRIVE,-127,0,0.3,                   // DRIVE AWAY A LITTLE
    TURN,0,1,                           // TURN STRAIGHT
    
    DRIVE,-127,0,WHITE_L,2,             // DRIVE TO START TILE
    DRIVE,-127,0,DISTANCE,0.5,1,        // DRIVE TO LINE UP
    TURN,270,2,                         // TURN FOR NEXT MOVE
    TURN,265,2,                         // TURN FOR NEXT MOVE
    DRIVE,127,270,BLACK_B,1,            // DRIVE OFF TILE
    DRIVE,127,270,DISTANCE,1.3,2,       // DRIVE TO GET BALL
    INTAKE_ON,                          // MAKE SURE INTAKE IS ON
    PAUSE,0.5,                          // PAUSE TO LET BALL IN
    
    WRISTSEEK,WRIST_FORWARD_POS,        // GET WRIST READY FOR FLIP
    DRIVE,-127,270,DISTANCE,0.45,2,     // DRIVE AWAY FROM CAP
    TURN,215,2,                         // TURN FOR CAP FLIP
    DRIVE,-100,215,DISTANCE,0.7,2,      // DRIVE TO CAP
    WRISTSEEK,WRIST_VERTICAL_POS,       // FLIP CAP
    PAUSE,0.125,                        // SHORT PAUSE
    DRIVE,100,215,DISTANCE,0.1,2,       // DRIVE AWAY FROM CAP
    
    TURN,270,2,                         // TURN FOR NEXT MOVE
    START_COAST,                        // START FLYWHEEL SPINUP
    DRIVE,127,270,DISTANCE,0.75,2,      // DRIVE TO LINE UP
    PAUSE,0.25,                         // SHORT PAUSE
    TURN,0,1,                           // TURN TO AIM
    TURN_AIM,BLUE_FLAG,CENTER,2,        // AIM AT CENTRE-MOST BLUE FLAG
    
    DRIVE,-90,CDIR,SONAR,1.2,1,
    FIRE_AIM,TOP,                       // SHOOT TOP FLAG
    PAUSE,FIRED,2,                      // WAIT TILL SHOT
    PAUSE,0.5,                          // SHORT PAUSE
    STOP_FIRE,                          // STOP FLYWHEEL
    
    TURN_AIM,BLUE_FLAG,CENTER,1,        // AIM AT CENTRE-MOST BLUE FLAG
    DRIVE,127,CDIR,SONAR,0.8,2,         // DRIVE TO MIDDLE FLAG DIST
    
    IF, GOTBALL,                        // IF WE HAVE ANOTHER BALL
        FIRE_AIM,MIDDLE,                // SHOOT MIDDLE FLAG
        PAUSE,0.5,                      // SHORT PAUSE
        PAUSE,FIRED,2,                  // WAIT TILL SHOT
    ENDIF,                              // END OF IF
    
    STOP_FIRE,                          // STOP FLYWHEEL
    STOP_COAST,                         // TURN FLYWHEEL FULLY OFF
    INTAKE_OFF,                         // TURN INTAKE OFF
    DRIVE,127,0,0.5,                    // DRIVE TO TOGGLE BOTTOM FLAG
    
    TURN,0,1,                           // TURN STRAIGHT
    INTAKE_ON,                          // TURN INTAKE ON
    DRIVE,-127,0,0.25,                  // DRIVE AWAY A LITTLE
    TURN,0,1,                           // TURN STRAIGHT
    
    DRIVE,-127,0,SONAR,1.2,2,           // DRIVE AWAY FROM FLAG
    WRISTSEEK,WRIST_FORWARD_POS,        // LOWER FORKS
    TURN,83,2,                          // TURN TO FACE CAP
    DRIVE,-127,90,DISTANCE,2.5,2,       // DRIVE TO CAP
    DRIVE,127,90,DISTANCE,0.1,1,        // DRIVE AWAY A LITTLE
    WRISTSEEK,WRIST_VERTICAL_POS,       // FLIP CAP
    PAUSE,0.5,                          // PAUSE A LITTLE
    
    WRISTSEEK,WRIST_FORWARD_POS,        // LOWER FORKS
    DRIVE,127,90,DISTANCE,0.5,1,        // DRIVE AWAY FROM CAP
    TURN,180,2,                         // TURN READY FOR NEXT MOVE
    WRISTSEEK,WRIST_VERTICAL_POS,       // RAISE FORKS
    DRIVE,127,180,DISTANCE,0.9,2,       // DRIVE TO LINE UP FOR CAP
    TURN,90,2,                          // AIM FOR CAP
    
    DRIVE,127,90,DISTANCE,1,2,          // DRIVE TO FLIP CAP
    INTAKE_ON,                          // INTAKE ON TO GET BALL
    START_COAST,                        // TURN FLYWHEEL ON
    PAUSE,0.5,                          // SHORT PAUSE
    DRIVE,-127,90,DISTANCE,1.25,2,      // DRIVE TO LINE UP FOR SHOT
    TURN,350,2,                         // TURN TO FACE FLAGS
    TURN_AIM,BLUE_FLAG,LEFT,2,          // AIM AT RIGHT-MOST BLUE FLAG
    
    IF, GOTBALLS,                       // IF WE HAVE TWO BALLS
        DRIVE,-90,CDIR,SONAR,1.5,1,     // LINE UP FOR SHOT
        DRIVE,90,CDIR,SONAR,1.5,1,      // LINE UP FOR SHOT
        FIRE_AIM,TOP,                   // SHOOT TOP FLAG
        PAUSE,FIRED,2,                  // WAIT TILL SHOT
        PAUSE,0.5,                      // SHORT PAUSE
        STOP_FIRE,                      // STOP FLYWHEEL
    ENDIF,                              // END OF IF
    
    TURN_AIM,BLUE_FLAG,CENTER,1,        // AIM AT CENTRE-MOST BLUE FLAG
    DRIVE,127,CDIR,0.25,                // DRIVE CLOSE
    DRIVE,127,CDIR,SONAR,1,2,           // DRIVE TO MIDDLE FLAG DIST
    FIRE_AIM,MIDDLE,                    // SHOOT MIDDLE FLAG
    PAUSE,FIRED,2,                      // WAIT TILL SHOT
    PAUSE,0.5,                          // SHORT PAUSE
    STOP_FIRE,                          // STOP FLYWHEEL
    
    WRISTSEEK,WRIST_VERTICAL_POS,       // RAISE FORKS
    STOP_COAST,                         // TURN FLYWHEEL FULLY OFF
    INTAKE_OFF,                         // TURN INTAKE OFF
    DRIVE,127,0,0.5,                    // DRIVE TO TOGGLE BOTTOM FLAG
    
    TURN,0,1,                           // TURN STRAIGHT
    INTAKE_ON,                          // TURN INTAKE ON
    DRIVE,-127,0,0.5,                   // DRIVE AWAY A LITTLE
    DRIVE,-127,0,DISTANCE,1.5,2,        // DRIVE BACK
    TURN,270,2,                         // TURN TO FACE BLUE TILE
    DRIVE,127,270,WHITE_E,2,            // DRIVE TO BLUE TILE
    DRIVE,-90,270,0.1,                  // DRIVE TO BREAK
    TURN,180,2,                         // TURN
    DRIVE,127,180,BLACK_L,2,            // DRIVE OFF TILE
    DRIVE,-90,180,0.1,                  // DRIVE TO BREAK
    TURN,80,2,                          // TURN TO FACE PLATFORM
    DRIVE,127,90,DISTANCE,0.5,1,        // DRIVE TO PLATFORM
    DRIVE,127,90,DISTANCE,3,4,          // PARK
    
    END                                 // END OF ROUTINE
};

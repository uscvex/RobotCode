
double skills[] = {
    270,                                // ROBOT STARTS FACING 270°
    
    WRISTSEEK,-1000,                    // DEPLOY FLIPPER
    FLIPSEEK,FLIP_POS1,                 // MAKE SURE FLIPPER STRAIGHT
    INTAKE_ON,                          // START INTAKE
    SCRAPER,SCRAPER_DOWN_POS,           // LOWER SCRAPER
    PAUSE,0.5,                          // PAUSE TO LET SCRAPER DOWN
    DRIVE,127,270,BLACK_B,0.5,          // DRIVE OFF TILE
    DRIVE,110,270,DISTANCE,1.25,2,      // DRIVE TO FLIP CAP
    WRISTSEEK,WRIST_VERTICAL_POS,       // STOP DEPLOY
    
    FLIPSEEK,FLIP_POS1,
    SCRAPER,SCRAPER_UP_POS,             // RAISE SCRAPER
    TURN,45,2,                          // TURN TO FACE CAP
    WRISTSEEK,WRIST_FORWARD_POS,        // LOWER FLIPPER
    DRIVE,-50,45,DISTANCE,1,2,          // DRIVE TO CAP
    WRISTSEEK,-1,                       // TURN WRIST MOTOR OFF
    FLIPSEEK,FLIP_POS2,                 // FLIP
    PAUSE,0.125,
    DRIVE,90,45,DISTANCE,1.2,2,         // DRIVE AWAY
    
    FLIPSEEK,FLIP_POS1,
    
    /////////////////////////////////////////////////////////////
    // COMMENT THIS BLOCK IF SKILLS TAKING TOO LONG
    // SAVES ~5s AT THE EXPENSE OF 1 POINT
    TURN,10,2,                          // TURN TO FACE NEXT CAP
    WRISTSEEK,WRIST_FORWARD_POS,        // LOWER FORKS
    DRIVE,-50,10,DISTANCE,0.5,2,        // DRIVE TO CAP
    WRISTSEEK,WRIST_VERTICAL_POS,       // RAISE FORKS
    DRIVE,60,10,DISTANCE,0.1,0.5,       // DRIVE AWAY FROM WALL
    DRIVE,90,10,DISTANCE,0.4,1,         // DRIVE AWAY
    // STOP COMMENTING HERE
    /////////////////////////////////////////////////////////////
    
    START_COAST,                        // LET FLYWHEEL SPIN UP TO SPEED
    
    TURN,90,1.5,                        // TURN TO FACE START TILE
    FLIPSEEK,FLIP_POS2,                 // FLIP
    WRISTSEEK,-1,
    DRIVE,127,90,WHITE_R,2,             // DRIVE TO START TILE
    DRIVE,-80,90,0.05,                  // BREAK TO STOP
    TURN,0,2,                           // TURN TO FACE FLAGS
    WRISTSEEK,WRIST_VERTICAL_POS,
    FLIPSEEK,FLIP_POS1,
    
    
    DRIVE,127,1,BLACK_L,2,              // DRIVE OFF START TILE
    DRIVE,127,1,0.1,                    // DRIVE TO ENSURE FULLY ON TILE
    DRIVE,127,1,WHITE_L,2,              // DRIVE TO NEXT TILE
    DRIVE,127,2,DISTANCE,0.8,1,         // LINE UP FOR SHOT
    
    PAUSE,0.1,                          // STOP BOUNCE
    
    FIRE_AIM,TOP,                       // SHOOT TOP FLAG
    PAUSE,FIRED,FIRE_TRY_TIME,          // WAIT TILL SHOT
    PAUSE,0.1,                          // SHORT PAUSE
    STOP_FIRE,                          // STOP FLYWHEEL
    
    DRIVE,127,CDIR,DISTANCE,            // CONTINUED ON NEXT LINE
    MIDDLE_FLAG_DIST-0.05,2,            // DRIVE TO MIDDLE FLAG DIST
    FIRE_AIM,TOP,                       // SHOOT MIDDLE FLAG
    PAUSE,FIRED,FIRE_TRY_TIME,          // WAIT TILL SHOT
    PAUSE,0.1,                          // SHORT PAUSE
    STOP_FIRE,                          // STOP FLYWHEEL
    
    
    INTAKE_OFF,                         // TURN INTAKE OFF
    DRIVE,127,0,0.5,                    // DRIVE TO TOGGLE BOTTOM FLAG
    DRIVE,-127,0,0.5,                   // DRIVE AWAY FROM FLAG
    INTAKE_ON,                          // TURN INTAKE BACK ON
    SCRAPER,SCRAPER_UP_POS,             // RAISE SCRAPER AGAIN
    FLIPSEEK,-1,                        // STOP TANGLE WITH NET
    
    TURN,0,1,                           // TURN TO LINE UP
    DRIVE,-127,0,WHITE_L,2,             // DRIVE TO RED TILE
    DRIVE,90,0,0.05,                    // BREAK TO STOP
    DRIVE,-127,0,DISTANCE,0.5,1,        // DRIVE TO MIDDLE OF IT
    TURN,90,0,                          // TURN TO FACE WALL
    
    FLIPSEEK,FLIP_POS1,                 // PUT FLIPPER BACK
    DRIVE,100,90,0.5,                   // DRIVE TO WALL
    SET_GYRO,90,                        // SET GYRO
    
    WRISTSEEK,WRIST_FORWARD_POS,        // LOWER FLIPPER
    DRIVE,-100,90,DISTANCE,0.1,1,       // DRIVE AWAY FROM WALL
    TURN,135,2,                         // TURN TO FACE NEXT CAP
    
    DRIVE,-90,135,DISTANCE,1.7,3,       // DRIVE TO NEXT CAP
    WRISTSEEK,WRIST_VERTICAL_POS,       // LIFT CAP
    PAUSE,0.25,                         // PAUSE TO LET IT LIFT
    FLIP,                               // FLIP THE CAP
    
    TURN,180,2,                         // TURN TO FACE PLATFORM
    WRISTSEEK,-1,                       // DROP CAP
    SCRAPER,SCRAPER_UP_POS,             // ENSURE SCRAPER IS UP
    DRIVE,127,180,DISTANCE,1.3,2,       // DRIVE TO PLATFORM
    SCRAPER,SCRAPER_DOWN_POS,           // LOWER SCRAPER
    WRISTSEEK,WRIST_VERTICAL_POS,       // LIFT FLIPPER
    FLIPSEEK,FLIP_POS1,                 // REVERT FLIPPER
    PAUSE,0.25,                         // PAUSE TO LET SCRAPER DROP
    DRIVE,-127,180,DISTANCE,0.25,1,     // DRIVE AWAY FROM PLATFORM
    PAUSE,0.25,                         // LET BALL ROLL IN
    
    SCRAPER,SCRAPER_DOWN_POS,           // MOVE SCRAPER TO CAP FLIP POSITION
    TURN,270,1,                         // TURN TO FACE NEXT CAP
    DRIVE,90,270,DISTANCE,1.1,1,        // DRIVE TO FLIP NEXT CAP
    PAUSE,0.25,                         // PAUSE TO LET BALL INTAKE
    DRIVE,-127,270,DISTANCE,0.25,1,     // DRIVE AWAY FROM CAP
    SCRAPER,SCRAPER_UP_POS,             // LIFT SCRAPER TO SAFETY
    
    TURN,315,2,                         // TURN TO FACE 315
    DRIVE,127,315,DISTANCE,0.35,2,      // DRIVE TO LINE UP
    
    TURN,350,0.5,                       // TURN TO FACE 350
    FIRE_AIM,TOP,                       // SHOOT TOP FLAG
    PAUSE,FIRED,FIRE_TRY_TIME,          // WAIT TILL SHOT
    PAUSE,0.1,                          // SHORT PAUSE
    STOP_FIRE,                          // STOP FLYWHEEL
    
    DRIVE,127,CDIR,DISTANCE,            // CONTINUED ON NEXT LINE
    MIDDLE_FLAG_DIST,2,                 // DRIVE TO MIDDLE FLAG DIST
    FIRE,TOP,                           // SHOOT MIDDLE FLAG
    PAUSE,FIRED,FIRE_TRY_TIME,          // WAIT TILL SHOT
    PAUSE,0.1,                          // SHORT PAUSE
    STOP_FIRE,                          // STOP FLYWHEEL
    
    INTAKE_OFF,                         // TURN INTAKE OFF
    DRIVE,127,0,0.5,                    // DRIVE TO TOGGLE BOTTOM FLAG
    DRIVE,-127,0,0.5,                   // DRIVE AWAY FROM FLAG
    INTAKE_ON,                          // TURN INTAKE BACK ON
    SCRAPER,SCRAPER_UP_POS,             // RAISE SCRAPER AGAIN
    
    TURN,0,0.5,                         // TURN TO LINE UP
    DRIVE,-127,0,SONAR,1.25,2,          // LINE UP FOR NEXT CAP
    
    FLIPSEEK,FLIP_POS1,                 // PUT FLIPPER CORRECT WAY UP
    TURN,90,0,                          // TURN TO FACE NEXT CAP
    WRISTSEEK,WRIST_FORWARD_POS,        // LOWER FLIPPER
    DRIVE,-127,90,WHITE_E,1,            // DRIVE ACOSS CENTER WHITE LINE
    DRIVE,-90,90,DISTANCE,2.5,2,        // DRIVE TO GET NEXT CAP (SLOWLY)
    
    WRISTSEEK,WRIST_VERTICAL_POS,       // LIFT CAP
    DRIVE,127,90,DISTANCE,0.1,1,        // DRIVE AWAY FROM WALL
    FLIP,                               // FLIP CAP
    WRISTSEEK,-1,                       // DROP CAP
    TURN,155,2,                         // TURN TO FACE PLATFORM
    SCRAPER,SCRAPER_DOWN_POS/3,         // LOWER SCRAPER HALF WAY
    DRIVE,127,155,DISTANCE,1.4,2,       // DRIVE TO PLATFORM
    
    SCRAPER,SCRAPER_DOWN_POS,           // LOWER SCRAPER
    WRISTSEEK,WRIST_VERTICAL_POS,       // LIFT FLIPPER
    FLIPSEEK,FLIP_POS1,                 // REVERT FLIPPER
    PAUSE,0.25,                         // PAUSE TO LET SCRAPER DROP
    DRIVE,-127,155,DISTANCE,0.5,1,      // DRIVE AWAY FROM PLATFORM
    PAUSE,0.25,                         // PAUSE TO LET BALL ROLL IN
    
    TURN,90,2,                          // TURN TO FACE NEXT CAP
    SCRAPER,SCRAPER_DOWN_POS,           // MOVE SCRAPER TO CAP FLIP POSITION
    DRIVE,127,90,DISTANCE,1.25,2,       // DRIVE TO FLIP CAP
    SCRAPER,SCRAPER_UP_POS,             // LIFT TO FLIP CAP FAR
    PAUSE,0.25,                         // PAUSE TO LET BALL INTAKE
    DRIVE,-127,90,WHITE_E,2,            // DRIVE TO LINE UP FOR FLAGS
    DRIVE,90,90,0.05,                   // DRIVE TO BREAK
    PAUSE,0.5,                          // PAUSE TO STOP TIPPING
    
    TURN,1,2,
    DRIVE,90,0,BLACK_B,2,               // DRIVE TO EDGE OF TILE
    DRIVE,-90,0,WHITE_E,2,              // DRIVE ONTO TILE
    DRIVE,-80,0,DISTANCE,0.1,1,         // DRIVE TO BREAK
    
    FIRE_AIM,TOP,                       // SHOOT TOP FLAG
    PAUSE,FIRED,FIRE_TRY_TIME,          // WAIT TILL SHOT
    PAUSE,0.1,                          // SHORT PAUSE
    STOP_FIRE,                          // STOP FLYWHEEL
    
    DRIVE,127,CDIR,DISTANCE,            // CONTINUED ON NEXT LINE
    MIDDLE_FLAG_DIST,2,                 // DRIVE TO MIDDLE FLAG DIST
    IF,GOTBALL,                         // IF WE'VE GOT A BALL
        FIRE,TOP,                       // SHOOT MIDDLE FLAG
        PAUSE,FIRED,FIRE_TRY_TIME,      // WAIT TILL SHOT
        PAUSE,0.1,                      // SHORT PAUSE
    ENDIF,
    STOP_FIRE,                          // STOP FLYWHEEL
    
    TURN,5,0.25,                        // TURN A LITTLE BIT
    INTAKE_OFF,                         // TURN INTAKE OFF
    DRIVE,127,0,0.5,                    // DRIVE TO TOGGLE BOTTOM FLAG 
    DRIVE,-127,0,0.5,                   // DRIVE AWAY FROM FLAG
    INTAKE_ON,                          // TURN INTAKE BACK ON
    SCRAPER,SCRAPER_UP_POS,             // RAISE SCRAPER AGAIN
    
    STOP_COAST,                         // TURN FLYWHEEL OFF NOW
    
    INTAKE_FLIP,                        // TURN INTAKE BACKWARDS TO LOSE ANY BALLS
    
    FLIPSEEK,-1,                        // DON'T LET THE FORKS GET CAUGHT
    
    TURN,20,0.8,                        // TURN TO LINE UP
    DRIVE,-127,20,WHITE_R,2,            // DRIVE UNTIL ON THE TILE
    DRIVE,-127,20,0.1,                  // DRIVE TO BE FULLY ON TILE
    TURN,0,0.5,                         // TURN TO LINE UP
    DRIVE,-127,0,BLACK_R,2,             // DRIVE UNTIL OFF THE TILE
    DRIVE,-127,0,DISTANCE,0.4,2,        // DRIVE TO LINE UP
    
    TURN,80,0.8,                        // TURN TO FACE PLATFORM
    INTAKE_ON,                          // ENSURE INTAKE IS RUNNING
    WRISTSEEK,WRIST_FORWARD_POS,        // PUT FLIPPER DOWN
    FLIPSEEK,FLIP_POS1,                 // MAKE SURE FLIPPER STRAIGHT
    DRIVE,127,80,DISTANCE,0.75,1,       // DRIVE TO PLATFORM
    DRIVE,127,80,WHITE_B,1,             // DRIVE UP FIRST PLATFORM
    DRIVE,127,80,DISTANCE,2,4,          // DRIVE UP PLATFORM 2.75
    
    END,                                // END OF ROUTINE
};

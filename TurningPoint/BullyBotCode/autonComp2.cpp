
// RED SIDE, WE WANT 19 PT SWING
double redAuton[] = {
    180,                                // START FACING 180
    
    INTAKE_ON,                          // TURN INTAKE ON
    
    DRIVE,50,180,DISTANCE,0.1,0.125,    // DRIVE WITHIN EXPANSION ZONE
    
    WRISTSEEK,-1000,                    // DEPLOY FLIPPER
    PAUSE,1.5,                          // WAIT FOR DEPLOY TO HAPPEN
    FLIP,                               // FLIP THE FLIPPER SO IT DOESN'T DRIFT
    WRISTSEEK,WRIST_FORWARD_POS,        // LOWER THE WRIST
    PAUSE,0.5,                          // DEPLOY DONE
    PAUSE,GOTBALLS,5,                   // WAIT UNTIL CAUGHT BALL FROM OTHER BOT
    
    DRIVE,-70,180,DISTANCE,0.6,1,       // DRIVE TO LINE UP FOR CAP
    TURN,135,1,                         // AIM AT CAP
    TURN,135,2,                         // AIM AT CAP
    
    DRIVE,-70,135,DISTANCE,0.75,2,      // DRIVE TO CAP 1
    WRISTSEEK,WRIST_VERTICAL_POS,       // PICK IT UP
    PAUSE,0.5,                          // TIME TO PICK UP
    
    DRIVE,70,135,WHITE_R,2,             // DRIVE TILL RIGHT SENSOR IS OVER START TILE
    TURN,180,2,                         // TURN FOR NEXT DRIVE
    DRIVE,70,180,BLACK_R,2,             // DRIVE UNTIL OFF START TILE
    DRIVE,-90,180,DISTANCE,0.075,1,     // DRIVE TO LINE UP
    TURN,90,2,                          // TURN TO LINE UP
    
    DRIVE,-127,90,0.1,                  // DRIVE AWAY FROM POLE
    TURN,90,2,                          // TURN TO LINE UP
    DRIVE,127,90,0.1,                   // DRIVE BACK TO WALL
    
    DRIVE,100,90,DISTANCE,0.4,0.5,      // DRIVE TO WALL
    
    STACK_HIGH,                         // STACK CAP 1
    PAUSE,STACKED,10,                   // WAIT TILL STACKED
    
    START_COAST,                        // START FLYWHEEL SPIN-UP
    
    DRIVE,-127,90,DISTANCE,0.5,2,       // DRIVE AWAY FROM WALL
    
    TURN,45,2,                          // TURN READY TO DRIVE - 245
    DRIVE,127,45,DISTANCE,0.3,2,        // DRIVE TO LINE UP
    
    TURN,0,2,                           // TURN TOWARDS FLAGS
    TURN_AIM,BLUE_FLAG,LEFT,2,          // AIM AT LEFT-MOST BLUE FLAG
    DRIVE,127,CDIR,DISTANCE,0.6,2,      // DRIVE TO TOP FLAG DIST
    FIRE_AIM,TOP,                       // SHOOT TOP FLAG
    PAUSE,FIRED,5,                      // WAIT TILL SHOT
    PAUSE,0.5,                          // SHORT PAUSE
    STOP_FIRE,                          // STOP FLYWHEEL
    
    DRIVE,127,CDIR,DISTANCE,0.7,2,      // DRIVE TO MIDDLE FLAG DIST
    FIRE_AIM,MIDDLE,                    // SHOOT MIDDLE FLAG
    PAUSE,FIRED,5,                      // WAIT TILL SHOT
    PAUSE,0.5,                          // SHORT PAUSE
    STOP_FIRE,                          // STOP FLYWHEEL
    
    STOP_COAST,                         // TURN FLYWHEEL FULLY OFF
    
    INTAKE_OFF,                         // TURN INTAKE OFF
    
    DRIVE,70,0,1,                       // DRIVE TO TOGGLE BOTTOM FLAG
    PAUSE,0.25,                         // SHORT PAUSE
    
    TURN,30,1,                          // TURN STRAIGHT
    TURN,0,1,                           // TURN STRAIGHT
    
    INTAKE_ON,                          // TURN INTAKE ON
    
    DRIVE,-127,0,DISTANCE,0.5,2,        // DRIVE AWAY A LITTLE
    
    DRIVE,-127,0,WHITE_L,4,             // DRIVE TO START TILE
    DRIVE,-127,0,DISTANCE,0.5,1,        // DRIVE TO LINE UP
    
    TURN,270,2,                         // TURN FOR NEXT MOVE
    TURN,265,2,                         // TURN FOR NEXT MOVE
    DRIVE,127,270,DISTANCE,1.75,2,      // DRIVE TO GET BALL FROM UNDER CAP
    
    DRIVE,-127,270,DISTANCE,0.65,2,     // DRIVE AWAY FROM CAP
    
    TURN,345,2,                         // TURN TOWARDS CENTER FLAGS
    TURN_AIM,BLUE_FLAG,CENTER,2,        // AIM AT CENTER-MOST BLUE FLAG
    DRIVE,127,CDIR,DISTANCE,0.75,2,     // DRIVE TO TOP FLAG DIST
    
    START_COAST,                        // START FLYWHEEL SPINUP
    
    PAUSE,UNTIL,35,                     // WAIT TILL LAST 10 SECONDS
    
    IF, GOTBALL,                        // ONLY SHOOT IF GOT A BALL
        FIRE_AIM,TOP,                   // SHOOT TOP FLAG
        PAUSE,FIRED,5,                  // WAIT TILL SHOT
        PAUSE,0.25,                     // SHORT PAUSE
    ENDIF,                              // END OF IF
    
    STOP_FIRE,                          // STOP FLYWHEEL
    
    DRIVE,127,CDIR,DISTANCE,0.75,2,     // DRIVE TO MIDDLE FLAG DIST
    
    FIRE_AIM,MIDDLE,                    // SHOOT MIDDLE FLAG
    PAUSE,FIRED,5,                      // WAIT TILL SHOT
    PAUSE,0.25,                         // SHORT PAUSE
    
    STOP_FIRE,                          // STOP FLYWHEEL
    
    INTAKE_OFF,                         // TURN INTAKE OFF
    
    TURN_REL,15,0.5,                    // TURN TO FACE 0
    DRIVE,127,0,0.5,                    // DRIVE TO TOGGLE BOTTOM FLAG
    PAUSE,UNTIL,44,                     // WAIT UNTIL LAST SECOND
    DRIVE,-127,270,1,                   // DRIVE AWAY FROM FLAGS
    
    END                                 // END OF ROUTINE
};

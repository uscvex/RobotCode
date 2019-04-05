
// AUTONOMOUS ROUTINE FOR BLUE-BOT, RED TEAM
double redBackAuton[] = {
    270,                                // ROBOT STARTS FACING 270°
    
    WRISTSEEK,-1000,                    // DEPLOY FLIPPER
    FLIPSEEK,FLIP_POS1,                 // MAKE SURE FLIPPER STRAIGHT
    INTAKE_ON,                          // START INTAKE
    DRIVE,127,270,BLACK_B,0.5,          // DRIVE OFF TILE
    DRIVE,80,270,DISTANCE,1.3,2,        // DRIVE TO FLIP CAP
    PAUSE,0.5,                          // SHORT PAUSE
    WRISTSEEK,WRIST_VERTICAL_POS,       // STOP DEPLOY
    
    DRIVE,-127,270,DISTANCE,1,2,        // DRIVE AWAY FROM CAP
    TURN,0,0.125,                       // SHORT TURN TO CONTROL DIRECTION
    TURN,90,2,                          // TURN TO FACE CAP
    WRISTSEEK,WRIST_FORWARD_POS,        // LOWER FLIPPER
    PAUSE,0.5,                          // PAUSE TO LET FLIPPER DOWN
    DRIVE,-70,90,DISTANCE,1,2,          // DRIVE TO GET CAP
    WRISTSEEK,WRIST_VERTICAL_POS,       // LIFT CAP UP
    
    PAUSE,0.5,                          // WAIT FOR CAP TO LIFT
    DRIVE,90,90,WHITE_R,3,              // DRIVE TO START TILE
    DRIVE,-90,90,0.05,                  // DRIVE TO BREAK
    PAUSE,1,                            // PAUSE TO STOP TIP
    DRIVE,-90,90,DISTANCE,0.125,1,      // DRIVE AWAY A LITTLE
    PAUSE,0.25,                         // SHORT PAUSE
    TURN,180,2,                         // TURN TO LINE UP
    PAUSE,0.5,                          // SHORT PAUSE
    DRIVE,90,180,WHITE_L,2,             // DRIVE TO WHITE LINE
    DRIVE,-90,180,0.1,                  // DRIVE TO BREAK
    DRIVE,-60,180,DISTANCE,0.125,1,     // DRIVE BACK A LITTLE
    PAUSE,0.5,                          // SHORT PAUSE
    TURN,270,2,                         // TURN TO FACE POLE
    PAUSE,0.25,                         // SHORT PAUSE
    ARMSEEK,ARM_POS_LOW,                // RAISE ARM
    PAUSE,2,                            // WAIT FOR ARM TO RAISE
    
    DRIVE,-90,270,WHITE_L,2,            // DRIVE ONTO TILE
    DRIVE,-90,270,DISTANCE,0.5,2,       // DRIVE TO POLE
    PAUSE,0.5,                          // PAUSE TO STOP MOMENTUM
    WRISTSEEK,-1,                       // DROP CAP
    PAUSE,1,                            // LET CAP SCORE
    DRIVE,90,270,DISTANCE,0.5,2,        // DRIVE AWAY
    ARMSEEK,1,                          // LOWER ARM
    WRISTSEEK,WRIST_FORWARD_POS,        // RAISE WRIST
    PAUSE,0.5,                          // SHORT PAUSE
    WRISTSEEK,WRIST_VERTICAL_POS,       // RAISE WRIST
    
    TURN,180,2,                         // TURN TO FACE WALL
    DRIVE,127,180,DISTANCE,1,1,         // DRIVE TO WALL
    DRIVE,127,180,0.5,                  // ENSURE AGAINST WALL
    SET_GYRO,180,                       // RESET GYRO TO 180
    
    DRIVE,-127,180,DISTANCE,0.2,1,      // DRIVE AWAY FROM WALL
    TURN,90,2,                          // TURN TO FACE NEXT CAP
    WRISTSEEK,WRIST_FORWARD_POS,        // LOWER FLIPPER
    PAUSE,0.5,                          // PAUSE TO LET FLIPPER DOWN
    DRIVE,-80,90,DISTANCE,1.5,2 ,       // DRIVE TO NEXT CAP
    WRISTSEEK,WRIST_VERTICAL_POS,       // LIFT CAP
    PAUSE,0.5,                          // PAUSE SO CAP LIFTS
    DRIVE,127,90,DISTANCE,0.25,1,       // DRIVE TO LINE UP
    TURN,45,1,                          // TURN READY TO DRIVE
    DRIVE,127,45,DISTANCE,0.8,1,        // DRIVE TO LINE UP
    TURN,0,2,                           // TURN TO FACE POLE
    ARMSEEK,ARM_POS_LOW,                // RAISE ARM
    FLIP,                               // FLIP CAP
    PAUSE,2,                            // WAIT FOR ARM TO RAIS
    
    DRIVE,-90,0,WHITE_E,1,              // DRIVE TO LINE
    DRIVE,-90,0,DISTANCE,0.5,2,         // DRIVE TO POLE
    PAUSE,0.5,                          // PAUSE TO STOP MOMENTUM
    WRISTSEEK,-1,                       // DROP CAPHIGH SCORE CAP 2
    PAUSE,1,                            // LET CAP SCORE
    DRIVE,90,0,WHITE_R,2,               // DRIVE AWAY
    ARMSEEK,1,                          // LOWER ARM
    WRISTSEEK,WRIST_VERTICAL_POS,       // RAISE WRIST
    
    START_COAST,                        // LET FLYWHEEL SPIN UP
    TURN,90,2,                          // TURN TO FACE START TILE
    DRIVE,127,90,WHITE_R,2,             // DRIVE TO START TILE
    DRIVE,127,90,DISTANCE,0.125,1,      // DRIVE A LITTLE MORE
    
    TURN,2,2,                           // TURN TO FACE FLAGS
    
    DRIVE,127,2,WHITE_L,2,              // DRIVE TO TILE
    DRIVE,127,2,0.1,                    // DRIVE TO ENSURE FULLY ON TILE
    DRIVE,127,2,BLACK_L,2,              // DRIVE OFF START TILE
    DRIVE,127,2,0.1,                    // DRIVE TO ENSURE FULLY ON TILE
    DRIVE,127,2,WHITE_L,2,              // DRIVE TO NEXT TILE
    DRIVE,127,2,DISTANCE,0.8,1,         // LINE UP FOR SHOT
    
    TURN_AIM,BLUE_FLAG,LEFT,2,          // AIM AT RIGHT-MOST BLUE FLAG
    PAUSE,UNTIL,41.25,                  // WAIT AS LATE AS POSSIBLE
    
    FIRE_AIM,TOP,                       // SHOOT TOP FLAG
    PAUSE,FIRED,FIRE_TRY_TIME,          // WAIT TILL SHOT
    PAUSE,0.1,                          // SHORT PAUSE
    STOP_FIRE,                          // STOP FLYWHEEL
    
    DRIVE,127,CDIR,DISTANCE,            // CONTINUED ON NEXT LINE
    MIDDLE_FLAG_DIST+0.1,2,             // DRIVE TO MIDDLE FLAG DIST
    FIRE_AIM,TOP,                       // SHOOT MIDDLE FLAG
    PAUSE,FIRED,FIRE_TRY_TIME,          // WAIT TILL SHOT
    PAUSE,0.1,                          // SHORT PAUSE
    STOP_FIRE,                          // STOP FLYWHEEL
    
    INTAKE_OFF,                         // TURN INTAKE OFF
    DRIVE,127,CDIR,0.5,                 // DRIVE TO TOGGLE BOTTOM FLAG 
    DRIVE,-127,0,0.5,                   // DRIVE AWAY FROM FLAG
    INTAKE_ON,                          // TURN INTAKE BACK ON
    
    END,                                // END OF ROUTINE
};

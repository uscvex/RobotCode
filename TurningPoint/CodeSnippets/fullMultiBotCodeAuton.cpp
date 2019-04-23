#ifndef __BALLBOTAUTONS__
#define __BALLBOTAUTONS__
#include "main.h"
#define NUMBER_AUTONS 5                 // Number of programmed routines
int autonSelect = 0;                    // Routine to start on
// Selecting one of the back autons will switch controls to arcade

// #defines for auton drive modes
#define DRIVEMODE_USER 0                // User mode
#define DRIVEMODE_TIME -1               // Drive for time
#define DRIVEMODE_DIST -2               // Drive for distance
#define DRIVEMODE_CUSTOM -3             // Drive custom
#define DRIVEMODE_SONAR -4              // Drive for sonar dist
#define DRIVEMODE_TURN 1                // Turn
#define TURNMODE_GYRO 1                 // Do turns with gyro
#define TURNMODE_ENCODER 2              // Do turns with encoders

// #defines for commands & conditions
#define END 0                           // End of auton routine
#define DRIVE -1                        // Drive (until some condition met)
#define TURN -2                         // Turn to some angle at some speed
#define TURN_REL -3                     // Turn for some angle at some speed
#define TURN_ENC -4                     // Turn for some angle using encoder ticks
#define FIRE -5                         // Shoot flywheel at preset speed
#define FIRE_AIM -6                     // Aim and fire ball
#define INTAKE_ON -7                    // Turn intake on
#define INTAKE_OFF -8                   // Turn intake off
#define ARMSEEK -9                      // Move arm to pos
#define WRISTSEEK -10                   // Move wrist to pos
#define FLIPSEEK -11                    // Move flipper to pos
#define FLIP -12                        // Auto flip 180°
#define STACK_LOW -13                   // Auto stack on low
#define STACK_HIGH -14                  // Auto stack on high
#define STACK_LOW_FROM -15              // Auto stack from a step low
#define STACK_HIGH_FROM -16             // Auto stack from a step high
#define PAUSE -17                       // Do nothing for some time
#define SET_GYRO -18                    // Set gyro to some angle
#define STOP_FLYWHEEL -19               // Stop Flywheel running
#define FINISH_LOW_STACK -20            // Finish low stack
#define TURN_AIM -21                    // Turn to aim at _____ (target, location, time)
#define STOP_FIRE -22                   // Cancel Fire
#define START_COAST -23                 // Start flywheel coast
#define STOP_COAST -24                  // Stop flywheel coast
#define DRIVE_TO -25                    // Drive to point, (s,x,y,t)
#define TURN_TO -26                     // Turn to face point
#define INTAKE_FLIP -27                 // Run outer intake backwards to flip cap
#define SCRAPER -28                     // Position seek for scraper
#define FIRE_AIM_BOTH - 29


// Conditionals
#define IF -42386                       // If
#define ELSE -41987                     // Else
#define ENDIF -3298                     // End if



#define BLUE_FLAG 1                     // Blue flag for aiming
#define RED_FLAG 2                      // Red flag for aiming
#define GREEN_FLAG 3                    // Green flag for aiming
#define CENTER 0                        // Aim at center-most obj.
#define LEFT 1                          // Aim at left-most obj.
#define RIGHT 2                         // Aim at right-most obj.

#define CDIR -563                       // Turn Parameter for drive facing current direction
#define DISTANCE -1                     // Drive condition for distance based on encoders
#define LIDAR -2                        // Drive condition for distance based on lidar
#define SONAR -11                        // Drive condition for distance based on sonar

// Drive conditions for white lines
#define WHITE_E -3                      // Either white
#define WHITE_B -4                      // Both white
#define WHITE_L -5                      // Left white
#define WHITE_R -6                      // Right white
#define BLACK_E -7                      // Either black
#define BLACK_B -8                      // Both black
#define BLACK_L -9                      // Left black
#define BLACK_R -10                     // Right black

#define FIRED -1                        // Pause condition wait till fired
#define GOTBALL -2                      // Pause condition wait till one ball
#define GOTBALLS -3                     // Pause condition wait till two balls
#define UNTIL -4                        // Pause condition wait till time (not for time)
#define STACKED -5                      // Pause condition wait till stacked
#define AFTER -6
#define BEFORE -7
// #define SCRAPER_UP -8
// #defines for arm positions
#define FLIP_POS1 1                     // 1:1 Ratio, 0°
#define FLIP_POS2 180                   // 1:1 Ratio, 180°
#define WRIST_BACK_POS (200*3)          // 1:3 Ratio, 200°
#define WRIST_BACKWARD_DROP_POS (-70*3) // 1:3 Ratio, -70°
#define WRIST_FORWARD_POS (80*3)        // 1:3 Ratio, 80°
#define WRIST_FORWARD_EXTRA (0*3)       // 1:3 Ratio, 30°
#define WRIST_FORWARD_DROP_POS (67*3)   // 1:3 Ratio, 65°
#define WRIST_VERTICAL_POS 1            // 1:3 Ratio, 0°
#define ARM_POS_HIGH (125*5)            // 1:5 Ratio, 120°
#define ARM_POS_LOW (90*5)              // 1:5 Ratio, 90°
#define ARM_POS_DOWN 1                  // 1:5 Ratio, 0°
#define ARM_SKILLS_POS 180*5
#define ARM_HOLD_POS 25*5
#define SCRAPER_UP_POS -10

#define SCRAPER_FLIP_POS 90
#define SCRAPER_UP -10
#define SCRAPER_DOWN_POS 50
#define ARM_KNOCK_POS 200
#define WRIST_KNOCK_POS 160

#define FAR_FLAG_DIST 1.2
#define MIDDLE_FLAG_DIST 0.8
#define FIRE_TRY_TIME 2.5


#define TOP 2                           // Top Flag
#define MIDDLE 1                        // Middle Flag


double defaultAuton[] = {
    0,
    END
};

double redAuton[] = {
    180,                                // ROBOT STARTS FACING 180°
    
    DRIVE,50,180,DISTANCE,0.1,1,        // DRIVE WITHIN EXPANSION ZONE
    
    WRISTSEEK,-1000,                    // DEPLOY FLIPPER
    PAUSE,1.5,
    FLIPSEEK,FLIP_POS1,
    WRISTSEEK,WRIST_FORWARD_POS,
    PAUSE,0.5,                          // DEPLOY DONE
    
    DRIVE,-70,180,DISTANCE,0.6,1,       // DRIVE TO LINE UP FOR CAP
    TURN,135,1,                         // AIM AT CAP
    TURN,135,2,                         // AIM AT CAP
    
    
    DRIVE,-50,135,DISTANCE,1,2,         // DRIVE TO CAP 1
    WRISTSEEK,WRIST_VERTICAL_POS,       // PICK IT UP                               GRAB CAP 1
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
    
    STACK_HIGH,                         // STACK CAP 1                              STACK CAP 1
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
    DRIVE,-80,270,DISTANCE,0.125,2,     // DRIVE BACK FROM CAP                      KNOCK CAP 2
    
    TURN,345,2,                         // TURN TO FACE FLAGS
    DRIVE,90,345,DISTANCE,0.65,2,       // DRIVE TO FLAG DISTANCE
    TURN,330,2,                         // TURN TO FACE FLAGS
    TURN_AIM,BLUE_FLAG,CENTER,2,        // AIM AT CENTRE-MOST BLUE FLAG
    
    PAUSE,2,                            // PAUSE AFTER TURN
    PAUSE,UNTIL,35,                     // WAIT AS LATE AS POSSIBLE
    START_COAST,                        // SPINUP FLYWHEEL
    TURN_AIM,BLUE_FLAG,CENTER,2,        // AIM AT CENTRE-MOST BLUE FLAG
    PAUSE,2,                            // PAUSE AFTER TURNING
    PAUSE,UNTIL,41.25,                  // WAIT AS LATE AS POSSIBLE
    
    FIRE_AIM,TOP,                       // SHOOT TOP FLAG                           TOP MIDDLE FLAG
    PAUSE,FIRED,FIRE_TRY_TIME,          // WAIT TILL SHOT
    PAUSE,0.1,                          // SHORT PAUSE
    STOP_FIRE,                          // STOP FLYWHEEL
    
    DRIVE,127,CDIR,DISTANCE,            // CONTINUED ON NEXT LINE
    MIDDLE_FLAG_DIST,2,                 // DRIVE TO MIDDLE FLAG DIST
    FIRE_AIM,TOP,                       // SHOOT MIDDLE FLAG                       MIDDLE MIDDLE FLAG
    PAUSE,FIRED,FIRE_TRY_TIME,          // WAIT TILL SHOT
    PAUSE,0.1,                          // SHORT PAUSE
    STOP_FIRE,                          // STOP FLYWHEEL
    
    INTAKE_OFF,                         // TURN INTAKE OFF
    DRIVE,127,CDIR,0.5,                 // DRIVE TO TOGGLE BOTTOM FLAG              BOTTOM MIDDLE FLAG
    DRIVE,-127,0,0.5,                   // DRIVE AWAY FROM FLAG
    INTAKE_ON,                          // TURN INTAKE BACK ON
    
    END                                 // END OF ROUTINE
};

double blueAuton[] = {
    180,                                // ROBOT STARTS FACING 180°
    
    DRIVE,50,180,DISTANCE,0.1,1,        // DRIVE WITHIN EXPANSION ZONE
    
    WRISTSEEK,-1000,                    // DEPLOY FLIPPER
    PAUSE,1.5,
    FLIPSEEK,FLIP_POS1,
    WRISTSEEK,WRIST_FORWARD_POS,
    PAUSE,0.5,                          // DEPLOY DONE
    
    DRIVE,-70,180,DISTANCE,0.7,1,       // DRIVE TO LINE UP FOR CAP
    TURN,225,1,                         // AIM AT CAP
    TURN,225,2,                         // AIM AT CAP
    
    
    DRIVE,-50,225,DISTANCE,1,2,         // DRIVE TO CAP 1
    WRISTSEEK,WRIST_VERTICAL_POS,       // PICK IT UP                               GRAB CAP 1
    PAUSE,0.25,                         // TIME TO PICK UP
    FLIP,                               // FLIP SO BALLS ROLL OFF
    PAUSE,0.75,                         // LET FLIP HAPPEN
    
    DRIVE,70,225,WHITE_L,2,             // DRIVE TILL LEFT SENSOR IS OVER START TILE
    TURN,180,2,                         // TURN FOR NEXT DRIVE
    FLIP,                               // FLIP CAP BACK
    DRIVE,70,180,BLACK_L,2,             // DRIVE UNTIL OFF START TILE
    DRIVE,-70,180,0.05,                 // DRIVE TO BREAK
    DRIVE,-70,180,DISTANCE,0.05,1,      // DRIVE TO LINE UP
    TURN,270,2,                         // TURN TO LINE UP
    
    DRIVE,-90,270,0.1,                  // DRIVE AWAY FROM POLE
    TURN,265,2,                         // TURN TO LINE UP
    DRIVE,70,270,0.1,                   // DRIVE BACK TO WALL
    
    DRIVE,50,270,DISTANCE,0.4,2,        // DRIVE TO WALL
    
    STACK_HIGH,                         // STACK CAP 1                              STACK CAP 1
    PAUSE,STACKED,10,                   // WAIT TILL STACKED
    
    TURN,270,2,                         // TURN STRAIGHT
    DRIVE,-90,270,DISTANCE,0.5,1,       // DRIVE AWAY FROM POLE
    TURN,0,2,                           // TURN TO FACE FLAGS
    DRIVE,90,0,WHITE_R,2,               // DRIVE ONTO TILE
    DRIVE,90,0,0.1,                     // MAKE SURE ON TILE
    DRIVE,90,0,BLACK_B,2,               // DRIVE OFF TILE
    DRIVE,-90,0,0.1,                    // DRIVE TO BREAK
    DRIVE,-90,0,DISTANCE,0.9,2,         // DRIVE TO LINE UP
    
    TURN,90,2,                          // TURN TO FACE CAP
    INTAKE_ON,                          // START INTAKE
    WRISTSEEK,WRIST_VERTICAL_POS,       // STOP DEPLOY
    DRIVE,80,90,BLACK_B,1,              // DRIVE OFF TILE
    DRIVE,80,90,DISTANCE,1.4,2,         // DRIVE TO GET BALL
    DRIVE,-80,90,DISTANCE,0.25,2,       // DRIVE BACK FROM CAP                      KNOCK CAP 2
    
    TURN,5,2,                           // TURN TO FACE FLAGS
    DRIVE,90,5,DISTANCE,0.6,2,          // DRIVE TO FLAG DISTANCE
    TURN,15,2,                          // TURN TO FACE FLAGS
    TURN_AIM,RED_FLAG,CENTER,2,         // AIM AT CENTRE-MOST BLUE FLAG
    
    PAUSE,2,                            // PAUSE AFTER TURN
    PAUSE,UNTIL,35,                     // WAIT AS LATE AS POSSIBLE
    START_COAST,                        // SPINUP FLYWHEEL
    TURN_AIM,RED_FLAG,CENTER,2,         // AIM AT CENTRE-MOST BLUE FLAG
    PAUSE,2,                            // PAUSE AFTER TURNING
    PAUSE,UNTIL,41.25,                  // WAIT AS LATE AS POSSIBLE
    
    FIRE_AIM,TOP,                       // SHOOT TOP FLAG                           TOP MIDDLE FLAG
    PAUSE,FIRED,FIRE_TRY_TIME,          // WAIT TILL SHOT
    PAUSE,0.1,                          // SHORT PAUSE
    STOP_FIRE,                          // STOP FLYWHEEL
    
    DRIVE,127,CDIR,DISTANCE,            // CONTINUED ON NEXT LINE
    MIDDLE_FLAG_DIST-0.2,2,             // DRIVE TO MIDDLE FLAG DIST
    FIRE_AIM,TOP,                       // SHOOT MIDDLE FLAG                       MIDDLE MIDDLE FLAG
    PAUSE,FIRED,FIRE_TRY_TIME,          // WAIT TILL SHOT
    PAUSE,0.1,                          // SHORT PAUSE
    STOP_FIRE,                          // STOP FLYWHEEL
    
    TURN,0,0.5,                         // TURN TO 0°
    INTAKE_OFF,                         // TURN INTAKE OFF
    DRIVE,127,CDIR,0.5,                 // DRIVE TO TOGGLE BOTTOM FLAG              BOTTOM MIDDLE FLAG
    DRIVE,-127,0,0.5,                   // DRIVE AWAY FROM FLAG
    INTAKE_ON,                          // TURN INTAKE BACK ON
    
    END                                 // END OF ROUTINE
};

double redBackAuton[] = {
    270,                                // ROBOT STARTS FACING 270°
    
    WRISTSEEK,WRIST_FORWARD_POS,        // START DEPLOY FLIPPER
    FLIPSEEK,FLIP_POS1,                 // MAKE SURE FLIPPER STRAIGHT
    INTAKE_ON,                          // START INTAKE
    DRIVE,127,270,BLACK_B,0.5,          // DRIVE OFF TILE
    DRIVE,80,270,DISTANCE,1.3,2,        // DRIVE TO FLIP CAP                        KNOCK CAP 1
    WRISTSEEK,WRIST_VERTICAL_POS,       // FINISH DEPLOY FLIPPER
    PAUSE,0.5,                          // SHORT PAUSE
    WRISTSEEK,WRIST_VERTICAL_POS,       // STOP DEPLOY
    
    DRIVE,-127,270,DISTANCE,1,2,        // DRIVE AWAY FROM CAP
    TURN,0,0.125,                       // SHORT TURN TO CONTROL DIRECTION
    TURN,90,2,                          // TURN TO FACE CAP
    WRISTSEEK,WRIST_FORWARD_POS,        // LOWER FLIPPER
    PAUSE,0.5,                          // PAUSE TO LET FLIPPER DOWN
    DRIVE,-70,90,DISTANCE,1,2,          // DRIVE TO GET CAP
    WRISTSEEK,WRIST_VERTICAL_POS,       // LIFT CAP UP                              COLLECT CAP 1
    
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
    WRISTSEEK,-1,                       // DROP CAP                                 HIGH SCORE CAP 1
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
    WRISTSEEK,WRIST_VERTICAL_POS,       // LIFT CAP                                 COLLECT CAP 2
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
    WRISTSEEK,-1,                       // DROP CAPHIGH SCORE CAP 2                 HIGH SCORE CAP 2
    PAUSE,1,                            // LET CAP SCORE
    DRIVE,90,0,WHITE_R,2,               // DRIVE AWAY
    ARMSEEK,1,                          // LOWER ARM
    WRISTSEEK,WRIST_VERTICAL_POS,       // RAISE WRIST
    
    START_COAST,                        // LET FLYWHEEL SPIN UP
    TURN,90,2,                          // TURN TO FACE START TILE
    DRIVE,127,90,WHITE_R,2,             // DRIVE TO START TILE
    DRIVE,127,90,DISTANCE,0.125,1,      // DRIVE A LITTLE MORE
    
    TURN,1,2,                           // TURN TO FACE FLAGS
    
    DRIVE,127,1,WHITE_L,2,              // DRIVE TO TILE
    DRIVE,127,1,0.1,                    // DRIVE TO ENSURE FULLY ON TILE
    DRIVE,127,1,BLACK_L,2,              // DRIVE OFF START TILE
    DRIVE,127,1,0.1,                    // DRIVE TO ENSURE FULLY ON TILE
    DRIVE,127,1,WHITE_L,2,              // DRIVE TO NEXT TILE
    DRIVE,127,1,DISTANCE,0.8,1,         // LINE UP FOR SHOT
    
    
    PAUSE,UNTIL,41.25,                  // WAIT AS LATE AS POSSIBLE
    
    FIRE_AIM,TOP,                       // SHOOT TOP FLAG                           TOP LEFT FLAG
    PAUSE,FIRED,FIRE_TRY_TIME,          // WAIT TILL SHOT
    PAUSE,0.1,                          // SHORT PAUSE
    STOP_FIRE,                          // STOP FLYWHEEL
    
    DRIVE,127,CDIR,DISTANCE,            // CONTINUED ON NEXT LINE
    MIDDLE_FLAG_DIST+0.1,2,             // DRIVE TO MIDDLE FLAG DIST
    FIRE_AIM,TOP,                       // SHOOT MIDDLE FLAG                       MIDDLE LEFT FLAG
    PAUSE,FIRED,FIRE_TRY_TIME,          // WAIT TILL SHOT
    PAUSE,0.1,                          // SHORT PAUSE
    STOP_FIRE,                          // STOP FLYWHEEL
    FLIP,                               // PUT FLIPPER STRAIGHT
    
    INTAKE_OFF,                         // TURN INTAKE OFF
    DRIVE,127,CDIR,0.5,                 // DRIVE TO TOGGLE BOTTOM FLAG              BOTTOM LEFT FLAG
    DRIVE,-127,0,0.5,                   // DRIVE AWAY FROM FLAG
    INTAKE_ON,                          // TURN INTAKE BACK ON
    
    
    END,                                // END OF ROUTINE
};

double blueBackAuton[] = {
    90,                                 // ROBOT STARTS FACING 90°
    
    WRISTSEEK,WRIST_FORWARD_POS,        // START DEPLOY FLIPPER
    FLIPSEEK,FLIP_POS1,                 // MAKE SURE FLIPPER STRAIGHT
    INTAKE_ON,                          // START INTAKE
    DRIVE,127,90,BLACK_B,0.5,           // DRIVE OFF TILE
    DRIVE,80,90,DISTANCE,1.3,2,         // DRIVE TO FLIP CAP                        KNOCK CAP 1
    WRISTSEEK,WRIST_VERTICAL_POS,       // FINISH DEPLOY FLIPPER
    PAUSE,0.5,                          // SHORT PAUSE
    WRISTSEEK,WRIST_VERTICAL_POS,       // STOP DEPLOY
    
    DRIVE,-127,90,DISTANCE,1,2,         // DRIVE AWAY FROM CAP
    TURN,0,0.25,                        // SHORT TURN TO CONTROL DIRECTION
    TURN,270,2,                         // TURN TO FACE CAP
    WRISTSEEK,WRIST_FORWARD_POS,        // LOWER FLIPPER
    PAUSE,0.5,                          // PAUSE TO LET FLIPPER DOWN
    DRIVE,-70,270,DISTANCE,1,2,         // DRIVE TO GET CAP
    WRISTSEEK,WRIST_VERTICAL_POS,       // LIFT CAP UP                              COLLECT CAP 1
    
    PAUSE,0.5,                          // WAIT FOR CAP TO LIFT
    DRIVE,90,270,WHITE_L,3,             // DRIVE TO START TILE
    DRIVE,-90,270,0.05,                 // DRIVE TO BREAK
    PAUSE,1,                            // PAUSE TO STOP TIP
    DRIVE,-90,270,DISTANCE,0.125,1,     // DRIVE AWAY A LITTLE
    PAUSE,0.25,                         // SHORT PAUSE
    TURN,180,2,                         // TURN TO LINE UP
    PAUSE,0.5,                          // SHORT PAUSE
    DRIVE,90,180,WHITE_R,2,             // DRIVE TO WHITE LINE
    DRIVE,-90,180,0.1,                  // DRIVE TO BREAK
    DRIVE,-60,180,DISTANCE,0.125,1,     // DRIVE BACK A LITTLE
    PAUSE,0.5,                          // SHORT PAUSE
    TURN,90,2,                         // TURN TO FACE POLE
    PAUSE,0.25,                         // SHORT PAUSE
    ARMSEEK,ARM_POS_LOW,                // RAISE ARM
    PAUSE,2,                            // WAIT FOR ARM TO RAISE
    
    DRIVE,-90,90,WHITE_R,2,            // DRIVE ONTO TILE
    DRIVE,-90,90,DISTANCE,0.5,2,       // DRIVE TO POLE
    PAUSE,0.5,                          // PAUSE TO STOP MOMENTUM
    WRISTSEEK,-1,                       // DROP CAP                                 HIGH SCORE CAP 1
    PAUSE,1,                            // LET CAP SCORE
    DRIVE,90,90,DISTANCE,0.5,2,        // DRIVE AWAY
    ARMSEEK,1,                          // LOWER ARM
    WRISTSEEK,WRIST_FORWARD_POS,        // RAISE WRIST
    PAUSE,0.5,                          // SHORT PAUSE
    WRISTSEEK,WRIST_VERTICAL_POS,       // RAISE WRIST
    
    TURN,180,2,                         // TURN TO FACE WALL
    DRIVE,127,180,DISTANCE,1,1,         // DRIVE TO WALL
    DRIVE,127,180,0.5,                  // ENSURE AGAINST WALL
    SET_GYRO,180,                       // RESET GYRO TO 180
    
    DRIVE,-127,180,DISTANCE,0.2,1,      // DRIVE AWAY FROM WALL
    TURN,270,2,                          // TURN TO FACE NEXT CAP
    WRISTSEEK,WRIST_FORWARD_POS,        // LOWER FLIPPER
    PAUSE,0.5,                          // PAUSE TO LET FLIPPER DOWN
    DRIVE,-80,270,DISTANCE,1.35,2,       // DRIVE TO NEXT CAP
    WRISTSEEK,WRIST_VERTICAL_POS,       // LIFT CAP                                 COLLECT CAP 2
    PAUSE,0.5,                          // PAUSE SO CAP LIFTS
    DRIVE,127,270,DISTANCE,0.25,1,       // DRIVE TO LINE UP
    TURN,315,1,                          // TURN READY TO DRIVE
    DRIVE,127,315,DISTANCE,0.8,1,        // DRIVE TO LINE UP
    TURN,0,2,                           // TURN TO FACE POLE
    ARMSEEK,ARM_POS_LOW,                // RAISE ARM
    FLIP,                               // FLIP CAP
    PAUSE,2,                            // WAIT FOR ARM TO RAISE
    
    DRIVE,-90,0,WHITE_E,1,              // DRIVE TO LINE
    DRIVE,-90,0,DISTANCE,0.5,2,         // DRIVE TO POLE
    PAUSE,0.5,                          // PAUSE TO STOP MOMENTUM
    WRISTSEEK,-1,                       // DROP CAPHIGH SCORE CAP 2                 HIGH SCORE CAP 2
    PAUSE,1,                            // LET CAP SCORE
    DRIVE,90,0,WHITE_L,2,               // DRIVE AWAY
    ARMSEEK,1,                          // LOWER ARM
    WRISTSEEK,WRIST_VERTICAL_POS,       // RAISE WRIST
    
    START_COAST,                        // LET FLYWHEEL SPIN UP
    TURN,270,2,                          // TURN TO FACE START TILE
    DRIVE,127,270,WHITE_L,2,             // DRIVE TO START TILE
    DRIVE,127,270,DISTANCE,0.125,1,      // DRIVE A LITTLE MORE
    
    TURN,359,2,                           // TURN TO FACE FLAGS
    
    DRIVE,127,359,WHITE_R,2,              // DRIVE TO TILE
    DRIVE,127,359,0.1,                    // DRIVE TO ENSURE FULLY ON TILE
    DRIVE,127,359,BLACK_R,2,              // DRIVE OFF START TILE
    DRIVE,127,359,0.1,                    // DRIVE TO ENSURE FULLY ON TILE
    DRIVE,127,359,WHITE_R,2,              // DRIVE TO NEXT TILE
    DRIVE,127,359,DISTANCE,0.8,1,         // LINE UP FOR SHOT
    
    
    PAUSE,UNTIL,41.25,                  // WAIT AS LATE AS POSSIBLE
    
    FIRE_AIM,TOP,                       // SHOOT TOP FLAG                           TOP LEFT FLAG
    PAUSE,FIRED,FIRE_TRY_TIME,          // WAIT TILL SHOT
    PAUSE,0.1,                          // SHORT PAUSE
    STOP_FIRE,                          // STOP FLYWHEEL
    
    DRIVE,127,CDIR,DISTANCE,            // CONTINUED ON NEXT LINE
    MIDDLE_FLAG_DIST+0.1,2,             // DRIVE TO MIDDLE FLAG DIST
    FIRE_AIM,TOP,                       // SHOOT MIDDLE FLAG                       MIDDLE LEFT FLAG
    PAUSE,FIRED,FIRE_TRY_TIME,          // WAIT TILL SHOT
    PAUSE,0.1,                          // SHORT PAUSE
    STOP_FIRE,                          // STOP FLYWHEEL
    FLIP,                               // PUT FLIPPER STRAIGHT
    
    TURN,0,0.5,                         // TURN STRAIGHT
    INTAKE_OFF,                         // TURN INTAKE OFF
    DRIVE,127,CDIR,0.5,                 // DRIVE TO TOGGLE BOTTOM FLAG              BOTTOM LEFT FLAG
    DRIVE,-127,0,0.5,                   // DRIVE AWAY FROM FLAG
    INTAKE_ON,                          // TURN INTAKE BACK ON
    
    
    END,                                // END OF ROUTINE
};


double skills[] = {
    270,                                // ROBOT STARTS FACING 270°
    
    WRISTSEEK,-1000,                    // DEPLOY FLIPPER
    FLIPSEEK,FLIP_POS1,                 // MAKE SURE FLIPPER STRAIGHT
    INTAKE_ON,                          // START INTAKE
    SCRAPER,SCRAPER_DOWN_POS,           // LOWER SCRAPER
    PAUSE,0.5,                          // PAUSE TO LET SCRAPER DOWN
    DRIVE,127,270,BLACK_B,0.5,          // DRIVE OFF TILE
    DRIVE,110,270,DISTANCE,1.25,2,      // DRIVE TO FLIP CAP                        FLIP CAP 1
    WRISTSEEK,WRIST_VERTICAL_POS,       // STOP DEPLOY
    
    FLIPSEEK,FLIP_POS1,
    SCRAPER,SCRAPER_UP_POS,             // RAISE SCRAPER
    TURN,45,2,                          // TURN TO FACE CAP
    WRISTSEEK,WRIST_FORWARD_POS,        // LOWER FLIPPER
    //PAUSE,0.25,                          // PAUSE TO LET FLIPPER DOWN
    DRIVE,-50,45,DISTANCE,1,2,          // DRIVE TO CAP
    WRISTSEEK,-1,                       // TURN WRIST MOTOR OFF
    //WRISTSEEK,WRIST_VERTICAL_POS,     // RAISE FORKS
    FLIPSEEK,FLIP_POS2,                 // FLIP                                     FLIP CAP 2
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
    FLIPSEEK,FLIP_POS2,                 // FLIP                                     FLIP CAP 3
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
    
    FIRE_AIM,TOP,                       // SHOOT TOP FLAG                           TOP LEFT FLAG
    PAUSE,FIRED,FIRE_TRY_TIME,          // WAIT TILL SHOT
    PAUSE,0.1,                          // SHORT PAUSE
    STOP_FIRE,                          // STOP FLYWHEEL
    
    DRIVE,127,CDIR,DISTANCE,            // CONTINUED ON NEXT LINE
    MIDDLE_FLAG_DIST-0.05,2,            // DRIVE TO MIDDLE FLAG DIST
    FIRE_AIM,TOP,                       // SHOOT MIDDLE FLAG                       MIDDLE LEFT FLAG
    PAUSE,FIRED,FIRE_TRY_TIME,          // WAIT TILL SHOT
    PAUSE,0.1,                          // SHORT PAUSE
    STOP_FIRE,                          // STOP FLYWHEEL
    
    
    INTAKE_OFF,                         // TURN INTAKE OFF
    DRIVE,127,0,0.5,                    // DRIVE TO TOGGLE BOTTOM FLAG              BOTTOM LEFT FLAG
    //SCRAPER,SCRAPER_DOWN_POS,           // LOWER SCRAPER SO IT DOESN'T CATCH THE FLAG
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
    //WRISTSEEK,WRIST_FORWARD_POS,        // DROP CAP                                 FLIP CAP 4
    
    TURN,180,2,                         // TURN TO FACE PLATFORM
    WRISTSEEK,-1,                       // DROP CAP                                 FLIP CAP 4
    SCRAPER,SCRAPER_UP_POS,             // ENSURE SCRAPER IS UP
    DRIVE,127,180,DISTANCE,1.3,2,       // DRIVE TO PLATFORM
    SCRAPER,SCRAPER_DOWN_POS,           // LOWER SCRAPER
    WRISTSEEK,WRIST_VERTICAL_POS,       // LIFT FLIPPER
    FLIPSEEK,FLIP_POS1,                 // REVERT FLIPPER
    PAUSE,0.25,                         // PAUSE TO LET SCRAPER DROP
    DRIVE,-127,180,DISTANCE,0.25,1,     // DRIVE AWAY FROM PLATFORM
    PAUSE,0.25,                         // LET BALL ROLL IN
    
    SCRAPER,SCRAPER_DOWN_POS,           // MOVE SCRAPER TO CAP FLIP POSITION
    TURN,270,1,                         // TURN TO FACE NEXT CAP - WAS 2                    FLIP CAP 5
    DRIVE,90,270,DISTANCE,1.1,1,        // DRIVE TO FLIP NEXT CAP
    PAUSE,0.25,                         // PAUSE TO LET BALL INTAKE
    DRIVE,-127,270,DISTANCE,0.25,1,     // DRIVE AWAY FROM CAP
    SCRAPER,SCRAPER_UP_POS,             // LIFT SCRAPER TO SAFETY
    
    TURN,315,2,                         // TURN TO FACE 315
    DRIVE,127,315,DISTANCE,0.35,2,      // DRIVE TO LINE UP
    
    TURN,350,0.5,                       // TURN TO FACE 350
    FIRE_AIM,TOP,                       // SHOOT TOP FLAG                           TOP MIDDLE FLAG
    PAUSE,FIRED,FIRE_TRY_TIME,          // WAIT TILL SHOT
    PAUSE,0.1,                          // SHORT PAUSE
    STOP_FIRE,                          // STOP FLYWHEEL
    
    DRIVE,127,CDIR,DISTANCE,            // CONTINUED ON NEXT LINE
    MIDDLE_FLAG_DIST,2,                 // DRIVE TO MIDDLE FLAG DIST
    FIRE,TOP,                           // SHOOT MIDDLE FLAG                       MIDDLE MIDDLE FLAG
    PAUSE,FIRED,FIRE_TRY_TIME,          // WAIT TILL SHOT
    PAUSE,0.1,                          // SHORT PAUSE
    STOP_FIRE,                          // STOP FLYWHEEL
    
    INTAKE_OFF,                         // TURN INTAKE OFF
    DRIVE,127,0,0.5,                    // DRIVE TO TOGGLE BOTTOM FLAG              BOTTOM MIDDLE FLAG
    // SCRAPER,SCRAPER_DOWN_POS,           // LOWER SCRAPER SO IT DOESN'T CATCH THE FLAG
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
    //PAUSE,0.25,                         // PAUSE TO LIFT    0.25
    DRIVE,127,90,DISTANCE,0.1,1,        // DRIVE AWAY FROM WALL
    FLIP,                               // FLIP CAP
    //PAUSE,0.25,                         // PAUSE TO LET FLIP    0.25
    //WRISTSEEK,WRIST_FORWARD_POS,        // DROP CAP                                 FLIP CAP 6
    WRISTSEEK,-1,                       // DROP CAP                                 FLIP CAP 6
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
    DRIVE,127,90,DISTANCE,1.25,2,       // DRIVE TO FLIP CAP                        FLIP CAP 7
    SCRAPER,SCRAPER_UP_POS,             // LIFT TO FLIP CAP FAR
    PAUSE,0.25,                         // PAUSE TO LET BALL INTAKE
    DRIVE,-127,90,WHITE_E,2,            // DRIVE TO LINE UP FOR FLAGS
    DRIVE,90,90,0.05,                   // DRIVE TO BREAK
    PAUSE,0.5,                          // PAUSE TO STOP TIPPING
    
    TURN,1,2,
    DRIVE,90,0,BLACK_B,2,               // DRIVE TO EDGE OF TILE
    DRIVE,-90,0,WHITE_E,2,              // DRIVE ONTO TILE
    DRIVE,-80,0,DISTANCE,0.1,1,         // DRIVE TO BREAK
    
    //
    //    DRIVE,127,90,DISTANCE,0.75,2,       // DRIVE TO LINE UP
    //    SCRAPER,SCRAPER_UP_POS,             // LIFT SCRAPER TO SAFETY
    //    TURN,0,1,                           // TURN TO FACE FLAGS
    //    DRIVE,60,0,SONAR,                   // CONTINUED ON NEXT LINE
    //    FAR_FLAG_DIST+0.5,2,                // LINE UP FOR SHOT
    //    TURN,335,1,                         // TURN ROUGHLY TO FACE FLAG
    
    FIRE_AIM,TOP,                       // SHOOT TOP FLAG                           TOP RIGHT FLAG
    PAUSE,FIRED,FIRE_TRY_TIME,          // WAIT TILL SHOT
    PAUSE,0.1,                          // SHORT PAUSE
    STOP_FIRE,                          // STOP FLYWHEEL
    
    DRIVE,127,CDIR,DISTANCE,            // CONTINUED ON NEXT LINE
    MIDDLE_FLAG_DIST,2,                 // DRIVE TO MIDDLE FLAG DIST
    IF,GOTBALL,                         // IF WE'VE GOT A BALL
    FIRE,TOP,                       // SHOOT MIDDLE FLAG                       MIDDLE RIGHT FLAG
    PAUSE,FIRED,FIRE_TRY_TIME,      // WAIT TILL SHOT
    PAUSE,0.1,                      // SHORT PAUSE
    ENDIF,
    STOP_FIRE,                          // STOP FLYWHEEL
    
    TURN,5,0.25,                        // TURN A LITTLE BIT
    INTAKE_OFF,                         // TURN INTAKE OFF
    DRIVE,127,0,0.5,                    // DRIVE TO TOGGLE BOTTOM FLAG              BOTTOM RIGHT FLAG
    //SCRAPER,SCRAPER_DOWN_POS,           // LOWER SCRAPER SO IT DOESN'T CATCH THE FLAG
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

#endif

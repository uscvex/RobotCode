
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

#define TOP 2                           // Top Flag
#define MIDDLE 1                        // Middle Flag


double defaultAuton[] = {
    0,
    END
};

// Arrays for routines
double blueAuton[] = {                  // FRONT BLUE SIDE, WE WANT 19 PT SWING
    
    180,                                // START FACING 180
    
    INTAKE_ON,
    
    DRIVE,50,180,DISTANCE,0.1,0.125,    // DRIVE WITHIN EXPANSION ZONE
    
    WRISTSEEK,-1000,                    // DEPLOY FLIPPER
    PAUSE,1.5,
    FLIP,
    WRISTSEEK,WRIST_FORWARD_POS,
    PAUSE,0.5,                          // DEPLOY DONE
    PAUSE,GOTBALLS,5,
    
    DRIVE,-70,180,DISTANCE,0.7,1,       // DRIVE TO LINE UP FOR CAP
    TURN,225,1,                         // AIM AT CAP
    TURN,225,2,                         // AIM AT CAP
    
    
    DRIVE,-50,225,DISTANCE,0.75,2,      // DRIVE TO CAP 1
    WRISTSEEK,WRIST_VERTICAL_POS,       // PICK IT UP
    PAUSE,0.5,                          // TIME TO PICK UP
    
    TURN,225,1,                         // TURN FOR NEXT DRIVE
    DRIVE,70,225,WHITE_L,2,             // DRIVE TILL RIGHT SENSOR IS OVER START TILE
    TURN,180,2,                         // TURN FOR NEXT DRIVE
    DRIVE,70,180,BLACK_L,2,             // DRIVE UNTIL OFF START TILE
    DRIVE,-50,180,DISTANCE,0.0375,1,    // DRIVE TO LINE UP
    DRIVE,50,180,DISTANCE,0.0125,1,     // DRIVE TO LINE UP
    TURN,270,2,                         // TURN TO LINE UP
    
    DRIVE,-127,27090,0.1,               // DRIVE AWAY FROM POLE
    TURN,270,2,                         // TURN TO LINE UP
    DRIVE,127,270,0.1,                  // DRIVE BACK TO WALL
    
    DRIVE,100,270,DISTANCE,0.5,0.7,     // DRIVE TO WALL
    
    STACK_HIGH,                         // STACK CAP 1
    PAUSE,STACKED,10,                   // WAIT TILL STACKED
    
    START_COAST,                        // START FLYWHEEL SPIN-UP
    
    DRIVE,-127,270,DISTANCE,0.5,2,      // DRIVE AWAY FROM WALL
    
    TURN,315,2,                         // TURN READY TO DRIVE - 245
    DRIVE,127,315,DISTANCE,0.6,2,       // DRIVE TO LINE UP
    
    TURN,0,2,                           // TURN TOWARDS FLAGS
    TURN_AIM,RED_FLAG,RIGHT,2,          // AIM AT RIGHT-MOST BLUE FLAG
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
    
    DRIVE,70,340,1,                     // DRIVE TO TOGGLE BOTTOM FLAG
    PAUSE,0.25,                         // SHORT PAUSE
    
    TURN,330,1,                         // TURN STRAIGHT
    TURN,0,1,                           // TURN STRAIGHT
    
    INTAKE_ON,                          // TURN INTAKE ON
    
    DRIVE,-127,0,DISTANCE,0.5,2,        // DRIVE AWAY A LITTLE
    
    TURN,0,1,                           // TURN STRAIGHT
    
    DRIVE,-127,0,WHITE_R,4,             // DRIVE TO START TILE
    DRIVE,-127,0,DISTANCE,0.5,1,        // DRIVE TO LINE UP
    
    TURN,90,2,                          // TURN FOR NEXT MOVE
    TURN,85,2,                          // TURN FOR NEXT MOVE
    DRIVE,127,90,DISTANCE,1.75,2,       // DRIVE TO GET BALL FROM UNDER CAP
    
    DRIVE,-127,90,DISTANCE,0.65,2,      // DRIVE AWAY FROM CAP
    
    TURN,15,2,                          // TURN TOWARDS CENTER FLAGS
    TURN_AIM,RED_FLAG,CENTER,2,         // AIM AT CENTER-MOST BLUE FLAG
    DRIVE,127,CDIR,DISTANCE,0.75,2,     // DRIVE TO TOP FLAG DIST
    
    START_COAST,                        // START FLYWHEEL SPINUP
    
    PAUSE,UNTIL,35,                     // WAIT TILL LAST 10 SECONDS
    
    IF, GOTBALL,                        // ONLY SHOOT IF GOT A BALL
        FIRE_AIM,TOP,                   // SHOOT TOP FLAG
        PAUSE,FIRED,5,                  // WAIT TILL SHOT
        PAUSE,0.25,                     // SHORT PAUSE
    ENDIF,
    
    STOP_FIRE,                          // STOP FLYWHEEL
    
    DRIVE,127,CDIR,DISTANCE,0.75,2,     // DRIVE TO MIDDLE FLAG DIST
    
    FIRE_AIM,MIDDLE,                    // SHOOT MIDDLE FLAG
    PAUSE,FIRED,5,                      // WAIT TILL SHOT
    PAUSE,0.25,                         // SHORT PAUSE
    
    STOP_FIRE,                          // STOP FLYWHEEL
    
    INTAKE_OFF,                         // TURN INTAKE OFF
    
    TURN_REL,345,0.5,                    // TURN TO FACE 0
    DRIVE,127,0,0.5,                    // DRIVE TO TOGGLE BOTTOM FLAG
    PAUSE,UNTIL,44,                     // WAIT UNTIL LAST SECOND
    DRIVE,-127,90,1,                   // DRIVE AWAY FROM FLAGS
    
    END                                 // END OF ROUTINE
};

double redAuton[] = {                   // FRONT RED SIDE, WE WANT 19 PT SWING
    
    180,                                // START FACING 180
    
    INTAKE_ON,
    
    DRIVE,50,180,DISTANCE,0.1,0.125,      // DRIVE WITHIN EXPANSION ZONE
    
    WRISTSEEK,-1000,                    // DEPLOY FLIPPER
    PAUSE,1.5,
    FLIP,
    WRISTSEEK,WRIST_FORWARD_POS,
    PAUSE,0.5,                          // DEPLOY DONE
    PAUSE,GOTBALLS,5,
    
    DRIVE,-70,180,DISTANCE,0.6,1,         // DRIVE TO LINE UP FOR CAP
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
    
    DRIVE,-90,90,0.1,                  // DRIVE AWAY FROM POLE
    TURN,90,2,                          // TURN TO LINE UP
    DRIVE,90,90,0.1,                   // DRIVE BACK TO WALL
    
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
    ENDIF,
    
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

double redBackAuton[] = {               // BACK RED SIDE, WE WANT 10PT SWING
    0,                                  // START FACING 0 DEGREES
    
    INTAKE_FLIP,
    FLIP,
    WRISTSEEK,WRIST_VERTICAL_POS,
    DRIVE,70,0,DISTANCE,0.3,1,          // DRIVE TO DELIVER BALL
    PAUSE,0.25,
    DRIVE,-70,0,DISTANCE,0.7,2,
    PAUSE,0.25,
    
    TURN,265,2,                          // TURN TO FACE CAP
    INTAKE_ON,
    WRISTSEEK,(WRIST_FORWARD_POS+WRIST_FORWARD_EXTRA),
    DRIVE,100,265,DISTANCE,1.75,2,       // DRIVE TO GET BALL
    PAUSE,0.25,
    DRIVE,-100,260,DISTANCE,0.75,1,
    
    TURN,0,0.5,                         // TURN TO GET CAP
    TURN,80,2,
    TURN,80,2,
    
    DRIVE,-50,80,DISTANCE,0.75,3,      // DRIVE TO GET CAP
    
    WRISTSEEK,WRIST_VERTICAL_POS,       // PICK UP CAP
    PAUSE,0.5,
    
    TURN,300,2,                          // DRIVE TO LINE UP WITH POLE
    DRIVE,-100,300,DISTANCE,0.6,2,
    TURN,250,2,
    ARMSEEK,ARM_POS_LOW,                // RAISE ARM
    PAUSE,1,
    DRIVE,-70,250,DISTANCE,1.85,4,        // DRIVE TO POLE
    DRIVE,40,250,0.2,
    PAUSE,2,
    WRISTSEEK,WRIST_FORWARD_DROP_POS,   // DROP CAP #1
    PAUSE,0.5,
    
    DRIVE,70,270,DISTANCE,0.5,2,
    
    WRISTSEEK,(WRIST_FORWARD_POS+WRIST_FORWARD_EXTRA),
    ARMSEEK,1,
    
    TURN,155,2,
    
    DRIVE,100,155,0.75,                 // LINE UP WITH WALL
    PAUSE,0.25,
    SET_GYRO,180,
    PAUSE,0.25,
    
    DRIVE,-100,180,DISTANCE,0.2,1,      // DRIVE TO LINE UP WITH CAP
    TURN,80,2,
    DRIVE,-60,80,DISTANCE,1.1,3,       // DRIVE TO CAP
    WRISTSEEK,WRIST_VERTICAL_POS,       // PICK UP CAP
    
    PAUSE,0.5,
    DRIVE,60,80,DISTANCE,0.3,1,        // DRIVE LINE UP
    TURN,0,2,                         // DRIVE TO LINE UP WITH POLE
    DRIVE,100,0,DISTANCE,0.5,2,
    TURN,340,2,
    ARMSEEK,ARM_POS_LOW,                // RAISE ARM
    PAUSE,1,
    FLIP,
    PAUSE,0.5,
    DRIVE,-100,340,DISTANCE,0.75,2,       // DRIVE TO POLE
    DRIVE,40,340,0.2,
    PAUSE,1,
    WRISTSEEK,WRIST_FORWARD_DROP_POS,   // DROP CAP #2
    PAUSE,0.5,
    
    START_COAST,
    
    DRIVE,70,0,DISTANCE,0.5,2,
    
    WRISTSEEK,(WRIST_FORWARD_POS+WRIST_FORWARD_EXTRA),
    ARMSEEK,1,
    
    TURN,310,2,
    
    FIRE,LEFT,540,                      // SHOOT BALL AT OPPONENT FLAG
    PAUSE,FIRED,5,
    
    END                                 // END OF ROUTINE
};

double blueBackAuton[] = {              // BACK BLUE SIDE, WE WANT 10PT SWING
    0,                                  // START FACING 0 DEGREES
    
    INTAKE_FLIP,
    FLIP,
    WRISTSEEK,WRIST_VERTICAL_POS,
    DRIVE,70,0,DISTANCE,0.3,1,          // DRIVE TO DELIVER BALL
    PAUSE,0.25,
    DRIVE,-70,0,DISTANCE,0.7,2,
    PAUSE,0.25,
    
    TURN,90,2,                          // TURN TO FACE CAP
    INTAKE_ON,
    WRISTSEEK,(WRIST_FORWARD_POS+WRIST_FORWARD_EXTRA),
    DRIVE,100,90,DISTANCE,1.75,2,       // DRIVE TO GET BALL
    PAUSE,0.25,
    DRIVE,-100,90,DISTANCE,0.75,1,
    
    TURN,0,0.5,                         // TURN TO GET CAP
    TURN,260,2,
    TURN,260,2,
    
    DRIVE,-50,260,DISTANCE,0.75,3,      // DRIVE TO GET CAP
    
    WRISTSEEK,WRIST_VERTICAL_POS,       // PICK UP CAP
    PAUSE,0.5,
    
    TURN,45,2,                          // DRIVE TO LINE UP WITH POLE
    DRIVE,-100,45,DISTANCE,0.8,2,
    TURN,80,2,
    ARMSEEK,ARM_POS_LOW,                // RAISE ARM
    PAUSE,1,
    DRIVE,-70,80,DISTANCE,1.4,4,        // DRIVE TO POLE
    DRIVE,40,80,0.2,
    PAUSE,2,
    WRISTSEEK,WRIST_FORWARD_DROP_POS,   // DROP CAP #1
    PAUSE,0.5,
    
    DRIVE,70,90,DISTANCE,0.5,2,
    
    WRISTSEEK,(WRIST_FORWARD_POS+WRIST_FORWARD_EXTRA),
    ARMSEEK,1,
    
    TURN,180,2,
    
    DRIVE,100,180,0.75,                 // LINE UP WITH WALL
    PAUSE,0.25,
    SET_GYRO,180,
    PAUSE,0.25,
    
    DRIVE,-100,180,DISTANCE,0.2,1,      // DRIVE TO LINE UP WITH CAP
    TURN,265,2,
    DRIVE,-60,265,DISTANCE,1.1,3,       // DRIVE TO CAP
    WRISTSEEK,WRIST_VERTICAL_POS,       // PICK UP CAP
    
    PAUSE,0.5,
    DRIVE,60,265,DISTANCE,0.3,1,        // DRIVE LINE UP
    TURN,0,2,                         // DRIVE TO LINE UP WITH POLE
    DRIVE,100,0,DISTANCE,0.5,2,
    TURN,0,2,
    ARMSEEK,ARM_POS_LOW,                // RAISE ARM
    PAUSE,1,
    FLIP,
    PAUSE,0.5,
    DRIVE,-100,0,DISTANCE,0.75,2,       // DRIVE TO POLE
    DRIVE,40,0,0.2,
    PAUSE,1,
    WRISTSEEK,WRIST_FORWARD_DROP_POS,   // DROP CAP #2
    PAUSE,0.5,
    
    START_COAST,
    
    DRIVE,70,0,DISTANCE,0.5,2,
    
    WRISTSEEK,(WRIST_FORWARD_POS+WRIST_FORWARD_EXTRA),
    ARMSEEK,1,
    
    TURN,40,2,
    
    FIRE,LEFT,550,                      // SHOOT BALL AT OPPONENT FLAG
    PAUSE,FIRED,5,
    
    END                                 // END OF ROUTINE
};

double skills[] = {
    270,
    
    WRISTSEEK,-1000,                    // DEPLOY FLIPPER
    INTAKE_ON,
    DRIVE,127,270,DISTANCE,1,2,         // DRIVE TO FLIP CAP
    DRIVE,80,270,DISTANCE,0.7,2,        // DRIVE TO FLIP CAP
    FLIP,
    
    WRISTSEEK,WRIST_FORWARD_POS,        // PUT FLIPPER DOWN
    
    DRIVE,-127,270,DISTANCE,0.5,2,      // DRIVE AWAY A LITTLE
    TURN,15,2,                          // TURN TO FACE CAP
    
    DRIVE,-60,15,DISTANCE,0.6,2,        // DRIVE TO CAP
    
    WRISTSEEK,WRIST_VERTICAL_POS,       // FLIP CAP
    PAUSE,0.125,
    DRIVE,127,15,DISTANCE,0.3,1,        // DRIVE TO CAP
    
    PAUSE,0.25,
    
    WRISTSEEK,WRIST_FORWARD_POS,        // LOWER FLIPPER
    
    TURN,50,2,                          // TURN FOR NEXT CAP
    
    DRIVE,-90,50,DISTANCE,0.9,2,        // DRIVE TO CAP
    WRISTSEEK,WRIST_VERTICAL_POS,       // FLIP CAP
    PAUSE,0.5,
    DRIVE,127,50,DISTANCE,0.5,2,
    
    TURN,90,2,                          // AIM AT START TILE
    
    DRIVE,127,90,SONAR,0.5,2,           // DRIVE BACK TO TILE
    //DRIVE,-127,90,DISTANCE,0.1,0.5,     // DRIVE TO BREAK
    
    TURN,0,2,                           // TURN TO FACE FLAGS
    
    START_COAST,                        // START FLYWHEEL SPINUP
    
    DRIVE,90,0,WHITE_L,0.5,
    
    DRIVE,127,0,0.5,                    // DRIVE A LITTLE
    
    DRIVE,127,0,BLACK_L,2,
    DRIVE,127,0,0.2,                    // DRIVE A LITTLE
    DRIVE,90,0,WHITE_L,2,
                                                                        // 1st Flag Set
    TURN_AIM,BLUE_FLAG,CENTER,2,        // AIM AT LEFT-MOST BLUE FLAG
    DRIVE,90,CDIR,DISTANCE,0.8,2,       // DRIVE TO TOP FLAG DIST
    FIRE_AIM,TOP,                       // SHOOT TOP FLAG
    PAUSE,FIRED,2,                      // WAIT TILL SHOT
    PAUSE,0.5,
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
    
    DRIVE,127,270,BLACK_B,1,
    DRIVE,127,270,DISTANCE,1.3,2,       // DRIVE TO GET BALL
    INTAKE_ON,
    PAUSE,0.5,
    
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
    
    PAUSE,0.25,
    
    TURN,0,1,                           // TURN TO AIM
    TURN_AIM,BLUE_FLAG,CENTER,2,        // AIM AT CENTRE-MOST BLUE FLAG
                                                                        // 2nd Flag Set
    //IF, GOTBALLS,
    DRIVE,-90,CDIR,SONAR,1.2,1,
        FIRE_AIM,TOP,                    // SHOOT TOP FLAG
        PAUSE,FIRED,2,                      // WAIT TILL SHOT
        PAUSE,0.5,                          // SHORT PAUSE
        STOP_FIRE,                          // STOP FLYWHEEL
    //ENDIF,
    
    TURN_AIM,BLUE_FLAG,CENTER,1,        // AIM AT CENTRE-MOST BLUE FLAG
    
    DRIVE,127,CDIR,SONAR,0.8,2,           // DRIVE TO MIDDLE FLAG DIST
    IF, GOTBALL,
        FIRE_AIM,MIDDLE,                    // SHOOT MIDDLE FLAG
        PAUSE,0.5,                          // SHORT PAUSE
        PAUSE,FIRED,2,                  // WAIT TILL SHOT
    ENDIF,
    STOP_FIRE,                          // STOP FLYWHEEL
    
    STOP_COAST,                         // TURN FLYWHEEL FULLY OFF
    
    INTAKE_OFF,                         // TURN INTAKE OFF
    
    DRIVE,127,0,0.5,                    // DRIVE TO TOGGLE BOTTOM FLAG
    //PAUSE,0.25,                         // SHORT PAUSE
    
    TURN,0,1,                           // TURN STRAIGHT
    
    INTAKE_ON,                          // TURN INTAKE ON
    DRIVE,-127,0,0.25,
    TURN,0,1,                           // TURN STRAIGHT
    
    //DRIVE,-127,0,DISTANCE,0.5,2,        // DRIVE AWAY FROM FLAG
    DRIVE,-127,0,SONAR,1.2,2,           // DRIVE AWAY FROM FLAG
    
    
    WRISTSEEK,WRIST_FORWARD_POS,
    TURN,83,2,                          // TURN TO FACE CAP
    
    DRIVE,-127,90,DISTANCE,2.5,2,       // DRIVE TO CAP
    DRIVE,127,90,DISTANCE,0.1,1,        // DRIVE AWAY A LITTLE
    WRISTSEEK,WRIST_VERTICAL_POS,       // FLIP CAP
    PAUSE,0.5,
    WRISTSEEK,WRIST_FORWARD_POS,
    DRIVE,127,90,DISTANCE,0.5,1,        // DRIVE AWAY FROM CAP
    
    TURN,180,2,                         // TURN READY FOR NEXT MOVE
    WRISTSEEK,WRIST_VERTICAL_POS,
    DRIVE,127,180,DISTANCE,0.9,2,       // DRIVE TO LINE UP FOR CAP
    
    TURN,90,2,                          // AIM FOR CAP
    //INTAKE_FLIP,                        // INTAKE REVERSE TO FLIP CAP
    DRIVE,127,90,DISTANCE,1,2,          // DRIVE TO FLIP CAP
    
    INTAKE_ON,                          // INTAKE ON TO GET BALL
    
    START_COAST,
    
    PAUSE,0.5,
    
    DRIVE,-127,90,DISTANCE,1.25,2,      // DRIVE TO LINE UP FOR SHOT
    
    TURN,350,2,                         // TURN TO FACE FLAGS
    TURN_AIM,BLUE_FLAG,LEFT,2,          // AIM AT RIGHT-MOST BLUE FLAG
                                                                        // 3RD FLAG SET
    IF, GOTBALLS,
        DRIVE,-90,CDIR,SONAR,1.5,1,
        DRIVE,90,CDIR,SONAR,1.5,1,
        FIRE_AIM,TOP,                    // SHOOT TOP FLAG
        PAUSE,FIRED,2,                      // WAIT TILL SHOT
        PAUSE,0.5,                          // SHORT PAUSE
        STOP_FIRE,                          // STOP FLYWHEEL
    ENDIF,
    
    TURN_AIM,BLUE_FLAG,CENTER,1,        // AIM AT CENTRE-MOST BLUE FLAG
    DRIVE,127,CDIR,0.25,
    DRIVE,127,CDIR,SONAR,1,2,           // DRIVE TO MIDDLE FLAG DIST
    FIRE_AIM,MIDDLE,                    // SHOOT MIDDLE FLAG
    PAUSE,FIRED,2,                      // WAIT TILL SHOT
    PAUSE,0.5,                          // SHORT PAUSE
    STOP_FIRE,                          // STOP FLYWHEEL
    
    WRISTSEEK,WRIST_VERTICAL_POS,
    
    STOP_COAST,                         // TURN FLYWHEEL FULLY OFF
    
    INTAKE_OFF,                         // TURN INTAKE OFF
    
    DRIVE,127,0,0.5,                    // DRIVE TO TOGGLE BOTTOM FLAG
    //PAUSE,0.25,                         // SHORT PAUSE
    
    TURN,0,1,                           // TURN STRAIGHT
    
    INTAKE_ON,
    
    DRIVE,-127,0,0.5,                   // DRIVE AWAY A LITTLE
    DRIVE,-127,0,DISTANCE,1.5,2,
    TURN,270,2,
    DRIVE,127,270,WHITE_E,2,
    DRIVE,-90,270,0.1,
    TURN,180,2,
    DRIVE,127,180,BLACK_L,2,
    DRIVE,-90,180,0.1,
    TURN,80,2,
    
    DRIVE,127,90,DISTANCE,0.5,1,       // PARK
    DRIVE,127,90,DISTANCE,3,4,
    END
};

#endif

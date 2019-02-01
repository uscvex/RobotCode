
#ifndef __BALLBOTAUTONS__
#define __BALLBOTAUTONS__

#define NUMBER_AUTONS 3                 // Number of programmed routines
int autonSelect = 0;                    // Routine to start on

                                        // #defines for auton drive modes
#define DRIVEMODE_USER 0                // User mode
#define DRIVEMODE_TIME -1               // Drive for time
#define DRIVEMODE_DIST -2               // Drive for distance
#define DRIVEMODE_CUSTOM -3             // Drive custom
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
double blueAuton[] = {
    0,
    END
};

double redAuton[] = {                   // RED SIDE, WE WANT 19 PT SWING
    
    180,                                  // START FACING 135
    
    DRIVE,50,0,DISTANCE,0.1,0.125,         // DRIVE WITHIN EXPANSION ZONE
    
    WRISTSEEK,-1000,                    // DEPLOY FLIPPER
    PAUSE,1.5,
    FLIP,
    WRISTSEEK,WRIST_FORWARD_POS,
    PAUSE,0.5,                          // DEPLOY DONE
    
    DRIVE,-70,0,DISTANCE,0.6,1,          // DRIVE TO LINE UP FOR CAP
    TURN,135,1,                         // AIM AT CAP
    TURN,135,2,                         // AIM AT CAP
    
    
    DRIVE,-70,135,DISTANCE,0.75,2,      // DRIVE TO CAP 1
    WRISTSEEK,WRIST_VERTICAL_POS,       // PICK IT UP
    PAUSE,0.5,                          // TIME TO PICK UP
    
    DRIVE,70,135,WHITE_R,2,             // DRIVE TILL RIGHT SENSOR IS OVER START TILE
    TURN,180,2,                         // TURN FOR NEXT DRIVE
    DRIVE,70,180,BLACK_B,2,             // DRIVE UNTIL OFF START TILE
    DRIVE,-90,180,DISTANCE,0.075,1,      // DRIVE TO LINE UP
    TURN,90,2,                          // TURN TO LINE UP
    DRIVE,100,90,DISTANCE,0.4,0.5,      // DRIVE TO WALL
    
    STACK_HIGH,                         // STACK CAP 1
    PAUSE,STACKED,10,                   // WAIT TILL STACKED
    
    START_COAST,                        // START FLYWHEEL SPIN-UP
    
    DRIVE,-127,90,DISTANCE,0.5,2,       // DRIVE AWAY FROM WALL
    TURN,180,2,                         // TURN TO INTAKE BALL
    INTAKE_ON,                          // TURN INTAKE ON
    DRIVE,127,180,DISTANCE,0.5,2,       // DRIVE WITHIN RANGE OF OTHER BOT
    
    PAUSE,GOTBALLS,5,                   // WAIT UNTIL BALL ARRIVED
    
    DRIVE,-127,180,DISTANCE,1,2,      // DRIVE AWAY FORM OTHER BOT
    
    TURN,90,0.5,                        // TURN READY TO DRIVE - 245
    TURN,65,2,                         // TURN READY TO DRIVE - 245
    DRIVE,127,65,DISTANCE,0.3,2,         // DRIVE AWAY FROM OTHER BOT
    
    TURN,0,2,                           // TURN TOWARDS FLAGS
    TURN_AIM,BLUE_FLAG,LEFT,2,          // AIM AT LEFT-MOST BLUE FLAG
    DRIVE,127,CDIR,DISTANCE,0.1,2,        // DRIVE TO TOP FLAG DIST
    FIRE_AIM,TOP,                       // SHOOT TOP FLAG
    PAUSE,FIRED,5,                      // WAIT TILL SHOT
    PAUSE,0.5,                         // SHORT PAUSE
    STOP_FIRE,                          // STOP FLYWHEEL
    
    DRIVE,127,CDIR,DISTANCE,1,2,        // DRIVE TO MIDDLE FLAG DIST
    FIRE_AIM,MIDDLE,                    // SHOOT MIDDLE FLAG
    PAUSE,FIRED,5,                      // WAIT TILL SHOT
    PAUSE,0.5,                         // SHORT PAUSE
    STOP_FIRE,                          // STOP FLYWHEEL
    
    STOP_COAST,                         // TURN FLYWHEEL FULLY OFF
    
    INTAKE_OFF,                         // TURN INTAKE OFF
    
    DRIVE,70,0,1,                    // DRIVE TO TOGGLE BOTTOM FLAG
    PAUSE,0.25,                         // SHORT PAUSE
    
    TURN,0,1,                           // TURN STRAIGHT
    
    INTAKE_ON,                          // TURN INTAKE ON
    
    DRIVE,-127,0,DISTANCE,0.5,2,        // DRIVE AWAY A LITTLE
    
    DRIVE,-127,0,WHITE_L,4,             // DRIVE TO START TILE
    DRIVE,-127,0,DISTANCE,0.5,1,        // DRIVE TO LINE UP
    
    TURN,270,2,                         // TURN FOR NEXT MOVE
    TURN,265,2,                         // TURN FOR NEXT MOVE
    DRIVE,127,270,DISTANCE,1.75,2,      // DRIVE TO GET BALL FROM UNDER CAP
    
    DRIVE,-127,270,DISTANCE,0.5,2,        // DRIVE AWAY FROM CAP
    
    TURN,315,2,                         // TURN TOWARDS CENTER FLAGS
    TURN_AIM,BLUE_FLAG,CENTER,2,        // AIM AT CENTER-MOST BLUE FLAG
    DRIVE,127,CDIR,DISTANCE,0.75,2,     // DRIVE TO TOP FLAG DIST
    
    PAUSE,UNTIL,35,                     // WAIT TILL LAST 10 SECONDS
    
    START_COAST,                        // START FLYWHEEL SPINUP
    
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


double skills[] = {
    0,
    END
};

#endif

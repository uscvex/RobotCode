
#ifndef __BALLBOTAUTONS__
#define __BALLBOTAUTONS__

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
#define WHITE -3                      // White
#define BLACK -4                      // Black

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
#define WRIST_FORWARD_POS (110)        // 1:1 Ratio, 80°
#define WRIST_VERTICAL_POS 15            // 1:3 Ratio, 0°
#define CAT_HOLD_POS (175*5)            // 1:5 Ratio, 180°

#define TOP 2                           // Top Flag
#define MIDDLE 1                        // Middle Flag


double defaultAuton[] = {
    0,
    END
};

// Arrays for routines
double blueAuton[] = {                  // FRONT BLUE SIDE, WE WANT 19 PT SWING
    0,
    
    END                                 // END OF ROUTINE
};

double redAuton[] = {                   // FRONT RED SIDE, WE WANT 19 PT SWING
    0,
    
    END                                 // END OF ROUTINE
};

double skills[] = {
    0,
    
    END                                 // END OF ROUTINE
};

#endif

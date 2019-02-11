#ifndef __BALLBOTAUTONS__
#define __BALLBOTAUTONS__


/*
 Most things you could need to change are in the top part of this file,
 Auton Routines are at the bottom...
 
 Make command is slightly different, must do:
 prosv5 make all
 
 To download without overriding 'trial' code:
 prosv5 u --slot 2
 
 To Swap Auton Routines Once Downloaded: Press button UP (^) and button X - ROBOT WILL DISPLAY MODE ON SCREEN
 To change default auton, change autonSelect (below)
 */
int autonSelect = 0;                    // Default routine to start on (0 = RED, 1 = BLUE)

/*
 Below are values you can change to change how robot behaves
 Turning section controls the speed/accuracy of turn, slower generally means more accurate
 Driving section controls the speed/accuracy of drive, slower generally means more accurate
 */


// AUTON TUNING PARAMETERS (Should be about right, but may still need tuning - especially turn)
// Turning
double ticksPerDegree = 6.85;//4.75;   // increase if turning too short, decrease if too far (6.85)
double turnAccepted = 0;      // decrease if turn is too inaccurate (0.5)
double turnRate = 150;          // increase if turning too fast, decrease if turning too slow (150)
double minSpeed = 25;           // increase if turn 'gets stuck' (25)
double maxTurn = 90;           // decrease if turning too fast (127)
// Driving
double ticksPerTile = 1400;     // increase if driving too short (1400)
double driveRate = 0.35;        // increase if driving distances too fast (overshooting), decrease if too slow (0.35)
double minForward = 40;         // increase if drive 'gets stuck', decrease if drive too innacurate (40)


// #defines for arm positions (User and Auton) - CAN CHANGE THESE
#define FLIP_POS1 1                     // DEFAULT POS FOR FLIPPER (1)
#define FLIP_POS2 180                   // FLIPPED POS FOR FLIPPER (180)
#define ARM_POS_DOWN 1                  // HEIGHT TO MOVE ARM DOWN TO (1)
#define ARM_POS_UP (85*5)               // HEIGHT TO RAISE ARM TO DESCORE (85 DEGREES, 1:5 GEAR RATIO) (85*5)
#define ARM_LIFT_DIST 35*5              // HEIGHT TO MOVE ARM TO LIFT A LITTLE / STACK (35 DEGREES, 1:5 GEAR RATIO) (35*5)
// WHEN STACKING, ARMSEEK SHOULD BE (ARM_POS_UP+ARM_LIFT_DIST), THEN MOVE DOWN TO ARM_POS_UP TO STACK


// Controller Mapping (User)
// #defines for buttons -- CHANGE IF YOU LIKE
#define BTN_ABORT DIGITAL_UP        // BUTTON TO CANCEL AUTO ARMSEEK/FLIP
#define BTN_CHOOSE_AUTON DIGITAL_X  // BUTTON TO CHANGE AUTON MODE (must also push BTN_ABORT)
#define BTN_ARM_UP DIGITAL_X        // MANUAL ARM UP
#define BTN_ARM_DOWN DIGITAL_B      // MANUAL ARM DOWN
#define BTN_FLIPPER_LEFT DIGITAL_Y  // MANUAL SPIN FLIPPER
#define BTN_FLIPPER_RIGHT DIGITAL_A // MANUAL SPIN FLIPPER
#define BTN_FLIP DIGITAL_L1         // AUTO FLIP
#define BTN_ARM_HIGH DIGITAL_R1     // AUTO MOVE ARM HIGH
#define BTN_ARM_LOW DIGITAL_R2      // AUTO MOVE ARM DOWN (TO FLOOR)
#define BTN_ARM_LIFT DIGITAL_L2     // TOGGLE MOVE ARM UP A LITTLE (ADDS/SUBTRACTS ARM_LIFT_DIST FROM SEEK)


/*
 IF YOU ADD TWO IR SENSORS, PLUG THE LEFT ONE INTO PORT 4(D), AND THE RIGHT INTO PORT 5(E)
 YOU MUST ATTATCH THE SENSORS FAIRLY HIGH OFF THE GROUND, YOU CAN USE PRINT-BOT AS EXAMPLE
 YOU MUST TUNE THE SENSORS BY TURNING THE TINY SCREW ON THEM UNTIL THE LIGHT IS OFF WHEN OVER GREY, AND ON WHEN OVER WHITE
 
 TO CONNECT THEM, YOU HAVE TO CUT THE CONNECTOR OFF AND CONNECT TO VEX CABLE, USE MOTOR CONTROLLER WIRES SINCE USELESS FOR V5
 CONNECT BLUE TO BLACK, BLACK TO WHITE, BROWN TO ORANGE
 THEN COVER EACH CONNECTION IN ELECTRICAL TAPE, AND WRAP WHOLE CONNECTION
 
 THEN YOU CAN USE:
 DRIVE,[SPEED],WHITE_,[MAX TIME],
 DRIVE,[SPEED],BLACK_,[MAX TIME],
 WHERE BLACK_ IS BLACK_L, BLACK_R, BLACK_E, OR BLACK_B (LEFT, RIGHT, EITHER, BOTH)
 AND WHITE_ IS WHITE_L, WHITE_R, WHITE_E, OR WHITE_B (LEFT, RIGHT, EITHER, BOTH)
 
 IF YOU ONLY ADD ONE SENSOR, YOU CAN ONLY USE WHITE_E AND BLACK_B
 DRIVE,[SPEED],WHITE_E,[MAX TIME],
 DRIVE,[SPEED],BLACK_B,[MAX TIME],
 BUT YOU CAN PLUG THE SENSOR INTO EITHER PORT 4(D) OR 5(E)
 */


// PLEASE DON'T CHANGE THIS SECTION
// Lots of #defines for auton modes, only really work on other bot though
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define NUMBER_AUTONS 2                 // Number of programmed routines

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

#define TOP 2                           // Top Flag
#define MIDDLE 1                        // Middle Flag
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// AUTON ROUTINE SECTION -- CHANGE THESE

/*
 Commands to build auton routine with (use only these):
 
 PAUSE,[TIME],                                  // WAITS FOR TIME IN SECONDS BEFORE DOING NEXT COMMAND
 DRIVE,[SPEED],[TIME],                          // DRIVES FOR TIME IN SECONDS, AT SPEED [-127,127]
 DRIVE,[SPEED],DISTANCE,[DISTANCE],[MAX TIME],  // DRIVES AT SPEED [-127,127], FOR DISTANCE IN TILES, OR UNTIL MAX TIME IN SECONDS, WHICHEVER COMES FIRST -- ROBOT WILL SLOW DOWN AS IT NEARS DESIRED POSITION
 TURN,[ANGLE],[MAX TIME],                       // TURNS USING ENCODERS FOR AN ANGLE IN DECREES [-180, 180] (POS = COUNTER CLOCKWISE)
 ARMSEEK,[POSITION],                            // STARTS MOVING ARM TO A POSITION
 FLIPSEEK,[POSITION],                           // MOVES FLIPPER TO POSITION IN DEGREES
 FLIP,                                          // MOVES FLIPPER 180 DEGREES
 END,                                           // FINISHES ROUTINE -- ALWAYS END ARRAY WITH THIS
 */

// Arrays for Red & Blue routines -- CHANGE THESE
double blueAuton[] = {                      // BLUE SIDE
    0,
    
    DRIVE,-127,DISTANCE,0.5,2,              // DRIVE BACK TO FLING BALL
    DRIVE,100,DISTANCE,1.5,2,               // DRIVE FORWARD TO LINE UP FOR CAP
    TURN,-90,2,                          // TURN TO FACE CAP
    DRIVE,-90,2,                            // DRIVE BACK TO ALIGN ON WALL
    DRIVE,100,DISTANCE,2,2,                 // DRIVE FORWARD TO CAP
    ARMSEEK,(ARM_POS_UP+ARM_LIFT_DIST),     // LIFT UP CAP
    PAUSE,0.5,                              // PAUSE A LITTLE TO LET ARM PICK UP CAP
    DRIVE,-100,DISTANCE,1,2,                // DRIVE BACK A LITTLE
    
    // DRIVE,100,WHITE_E,2,                    // DRIVE FORWARDS UNTIL EITHER SENSOR SEES WHITE
    
    END                                     // END OF ROUTINE
};

double redAuton[] = {                       // RED SIDE
    0,
    
    DRIVE,-127,0.1,                         // DRIVE BACK TO FLING BALL
    DRIVE,100,DISTANCE,1,2,                 // DRIVE FORWARD TO LINE UP FOR CAP
    TURN,90,2,                              // TURN TO FACE CAP
    DRIVE,-90,1,                            // DRIVE BACK TO ALIGN ON WALL
    TURN,-5,1,                              // TURN 90 DEGREES
    DRIVE,90,DISTANCE,0.8,2,                // DRIVE FORWARD TO CAP
    ARMSEEK,(ARM_LIFT_DIST),                // LIFT UP CAP
    FLIP,                                   // FLIP THE CAP
    PAUSE,0.5,                              // PAUSE A LITTLE TO LET ARM PICK UP CAP
    ARMSEEK,(0),                            // SET THE ARM BACK DOWN TO 0
    DRIVE,-90,DISTANCE,.7,2,                // DRIVE BACK A LITTLE
    TURN, 45, 1,                            // TURN 45 DEGREES
    DRIVE, 127, 0.5,                        // RAM FORWARD FOR .5 SECONDS AT MAXIMUM OVERDRIVE
    DRIVE,-90,DISTANCE,0.25,2,              // DRIVE BACKWARD TO CAP
    TURN, -45, 1,                           // TURN BACK TO PREPARE FOR CLIMB
    ARMSEEK, (ARM_LIFT_DIST),               // LIFT UP THE ARM A TINY BIT
    PAUSE,1,                                // PAUSE A LITTLE TO LET ARM PICK UP CAP
    DRIVE,127,DISTANCE,2,2,                 // DRIVE FORWARD TO PARK
    
    
    // DRIVE,100,WHITE_E,2,                    // DRIVE FORWARDS UNTIL EITHER SENSOR SEES WHITE
    
    END                                     // END OF ROUTINE
};





////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Don't need to change this
double defaultAuton[] = {
    // Do nothing
    END
};

#endif

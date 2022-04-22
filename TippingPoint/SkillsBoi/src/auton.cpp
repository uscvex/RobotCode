#include "main.h"


                            // FIRST LINE: <X>, <Y>, <DIRECTION>
// Auton commands
#define TURN 1              // TURN, <FACEDIR>, <TIMEOUT>
#define DRIVE 2             // DRIVE, <SPEED>, <DRIVEDIR>, <TIMEOUT>
#define DRIVEDIST 3         // DRIVEDIST, <SPEED>, <DRIVEDIR>, <DISTANCE> <TIMEOUT>
#define END 7               // END
#define PAUSE 8             // PAUSE, <TIMEOUT>
#define WAIT 9              // WAIT, <CONDITION>, <PARAMETER>, <TIMEOUT>
#define DEPLOY 14           // DEPLOY
#define DRIVETO 15          // DRIVETO, <SPEED>, <X>, <Y>, <TIMOUT>
#define SETPOS 16           // SETPOS, <X>, <Y>

#define YEET 20             // YEET, DISTANCE, <TIMEOUT>
#define RETRACTYEET 21      // RETRACTYEET

#define SETDIR 24           // SETDIR, <DIRECTION>
#define FACE 25             // TURNTO, <X>, <Y>, <TIMEOUT>

#define CHILLYEET 26        // CHILLYEET

#define SPIKEBACKWARDSCORE 27    // SPIKE_BACKWARDS_SCORE

#define INTAKE 28           // INTAKE, <true/false>
#define WOBBLE 29           // WOBBLE, <true/false>
#define FRONTARM 30         // FRONTARM, <pos>
#define BACKARM 31          // FRONTARM, <pos>
#define TIPBASE 32          // TIPBASE, <true/false>
#define FRONTDROP 33        // FRONTDROP,  <true/false>
#define BACKDROP 34         // BACKDROP, <true/false>
#define SIDEARM 35          // SIDEARM, <pos>
#define BRAKE 36
#define ULTRABOOST 37
#define UNBOOST 38


#define ON true
#define OFF false

// Wait conditions
#define LIFTBELOW 1
#define LIFTABOVE 2
#define TIME 3


int which_auton = 2;
int num_autons = 5;
string auton_names[] = {"MID", "LEFT", "TEST", "SK_LEFT", "SK_RGHT"};
double* auton_ptr[] = {&mid_auton[0], &left_auton[0], &test_auton[0], &left_skills[0], &right_skills[0]};

double test_auton[] = {
    0, 0, 0,
    DRIVE, 40, 0, 1, 
    DRIVEDIST, -60, 0, 4.5, 2, 
    DRIVE, 40, 0, 0.05, 
    FRONTARM, READY, 
    PAUSE, 2, 
    ULTRABOOST,
    FRONTARM, PARK, 
    DRIVEDIST, 12700, 0, 42, 10,    // 38 FOR BALANCED
    DRIVE, -40, 0, 0.05, 
    // DRIVEDIST, -12700, 0, 1, 2,
    BRAKE, 10, 
    END,
};


double mid_auton[] = {
   -61.9, -42.5, 235,                  // STARTING POSITION

    // HEKKIN YEET TO THE MIDDLE GOAL, LIFT AND SPIN
    FRONTARM, HOLD,                     // HOLD FRONT ARM UP
    YEET, 48, 2,                        // YEET
    DRIVE, 127, 235, 0.25,              // BRAKE
    BACKARM, HOLD,                      // LIFT GOAL  
    DRIVEDIST, 127, 235, 10, 2,         // DRIVE BACK
    TURN, 0, 1,                         // SPIN GOAL OUT OF THE WAY
    TURN, 45, 1,                        // TURN TO START SPIN IN CORRECT DIRECTION
    DRIVEDIST, -127, 45, 24, 45,        // DRIVE AWAY FROM CENTER, DON'T DO ANYTHING ELSE IF WE ARE STUCK
    CHILLYEET,                          // TURN YEET OFF

    // DRIVE READY FOR ALLIANCE GOAL
    TIPBASE, OFF,                       // OPEN BACK GRAB
    DRIVETO, -127, -38, -36, 4,         // DRIVE TO LINE UP FOR GOAL
    DRIVE, 60, 45, 0.1,                 // BREAK
    TURN, 0, 2,                         // TURN READY FOR GOAL

    // GRAB ALLIANCE GOAL, BUMP INTO WALL TO GET IT UP
    DRIVEDIST, -70, 0, 30, 2,           // DRIVE INTO GOAL
    TIPBASE, ON,                        // GRAB GOAL
    DRIVE, 90, 0, 0.125,                // DRIVE AWAY FROM WALL
    DRIVE, -90, 0, 0.5,                 // DRIVE INTO WALL

    // LINE UP FOR ROW OF RINGS, GET THEM AND CLOSE YELLOW GOAL
    SIDEARM, READY,                     // LOWER SIDE ARM
    // DRIVETO, 127, -36, -48, 2,           // DRIVE TO ALIGN FOR RING ROW
    DRIVEDIST, 127, 0, 8, 2,            // DRIVE TO ALIGN FOR RING ROW
    DRIVE, -50, 0, 0.05,                 // BREAK
    TURN, 90, 2,                        // TURN READY FOR RINGS
    INTAKE, ON,                         // TURN INTAKE ON
    WOBBLE, ON,                         // TURN WOBBLE ON
    DRIVETO, 60, 5, -48, 6,             // DRIVE FOR RINGS
    SIDEARM, HOLD,                      // LIFT GOAL
    PAUSE, 0.75,                        // WAIT FOR GOAL LIFT

    // LINE UP FOR FLOWER RINGS, GET THEM AND RETURN
    DRIVETO, -127, -24, -36, 6,         // DRIVE TO LINE UP FOR FLOWERS
    TURN, 0, 2,                         // TURN TO FACE FLOWERS
    DRIVETO, 35, -24, 30, 20,           // DRIVE TO INTAKE RINGS
    TURN, 180, 2,                       // TURN AROUND
    DRIVETO, 90, -24, -36, 20,          // DRIVE BACK TO START

    // DROP OFF ALLIANCE GOAL, TURN AND GRAB IT WITH FRONT GRAB
    TURN, 45, 2,                        // TURN READY TO DROP OFF GOAL
    FRONTARM, READY,                    // LOWER ARM IN ANTICIPATION
    DRIVETO, -127, -40, -40, 4,         // DRIVE INTO CORNER
    TURN, 45, 2,                        // TURN READY TO DROP OFF GOAL
    INTAKE, OFF,                        // TURN INTAKE OFF
    WOBBLE, OFF,                        // TURN WOBBLE OFF
    TIPBASE, OFF,                       // DROP GOAL
    PAUSE, 0.75,                        // WAIT FOR DROP
    DRIVEDIST, 90, 45, 20, 3,           // DRIVE AWAY FROM GOAL
    TURN, 0, 0.5,                       // START TURN IN CORRECT DIRECTION
    TURN, 225, 2,                       // TURN TO AIM AT GOAL
    DRIVEDIST, 127, 225, 30, 2,         // DRIVE TO GRAB GOAL
    FRONTARM, HOLD,                     // LIFT GOAL

    // GO DOCK WITH OTHER BOT
    DRIVETO, -127, -40, -36, 5,         // DRIVE TO ALIGN FOR DOCK
    DRIVE, 50, 225, 0.05,               // BREAK
    BACKARM, READY,                     // LOWER BACK ARM
    TURN, 180, 2,                       // TURN READY FOR DOCK
    DRIVETO, -90, -49, 0, 10,           // GO DOCK

    END,
};

double left_auton[] = {
    -43, 49, 278,      // STARTING POS
    
    END,
};

double right_skills[] = {
    0, 0, 0,

    END,
};

double left_skills[] = {
    -60, 43, 0,

    DRIVEDIST, -60, 0, 3, 2,        // DRIVE BACKWARDS INTO GOAL
    TIPBASE, ON,                    // GRAB GOAL
    PAUSE, 0.25,                     // WAIT FOR GRAB
    TURN, 45, 0.5, 
    DRIVETO, 127, -48, 50, 2,        // DRIVE FORWARD

    FACE, 60, 44, 1,              // TURN READY FOR RING COLLECT

    INTAKE, ON,                     // TURN INTAKE ON
    WOBBLE, ON, 
    DRIVETO, 127, -7, 46, 3,        // DRIVE ALMOST TO ROW OF RINGS
    DRIVETO, 45, 10, 44, 10,        // DRIVE INTO RINGS
    DRIVETO, 45, 28, 44, 10,        // DRIVE INTO RINGS
    PAUSE, 0.5,

    TURN, 180, 1,                   // DRIVE SO BASE IS AGAINST WALL
    TIPBASE, OFF,
    INTAKE, OFF, 
    WOBBLE, OFF, 
    TURN, 180, 0.5, 
    DRIVEDIST, -70, 180, 5, 2, 
    DRIVEDIST, 70, 180, 1, 2, 
    TURN, 225, 1.5, 
    DRIVEDIST, -80, 225, 15, 2,

    FRONTARM, READY,  
    TIPBASE, ON, 
    DRIVEDIST, -80, 225, 1, 1,
    PAUSE, 0.25, 

    FACE, 0, 36, 1.5,
    DRIVETO, 127, 0, 30, 3,       // DRIVE TO GET GOAL
    FRONTARM, HOLD, 
    
    INTAKE, ON,                     // TURN INTAKE ON
    WOBBLE, ON, 
    FACE, 24, 15, 1,
    DRIVETO, 127, 26, 15, 4,        // DRIVE READY FOR NEXT RINGS

    TURN, 180, 1.5, 

    ////////////////////////////////////////////////////////////////////////
    // 26, 0, 180,                 // HACK FOR START HALF WAY THROUGH
    // TIPBASE, ON,
    // FRONTARM, HOLD,
    // INTAKE, ON, 
    // WOBBLE, ON, 
    ////////////////////////////////////////////////////////////////////////

    DRIVETO, 40, 26, -20, 16,       // DRIVE TO COLLECT NEXT RINGS
    PAUSE, 0.5,
    DRIVETO, 40, 26, -56, 16,       // DRIVE TO COLLECT NEXT RINGS
    PAUSE, 0.25, 
    TURN, 270, 1.5, 
    TIPBASE, OFF, 
    INTAKE, OFF, 
    WOBBLE, OFF, 
    DRIVEDIST, -50, 270, 30, 2,       // DRIVE INTO WALL
    DRIVEDIST, 80, 270, 10, 3,       // DRIVE AWAY FROM WALL
    BACKARM, READY,
    PAUSE, 2, 
    DRIVEDIST, -80, 270, 15, 2,       // DRIVE INTO WALL
    BACKARM, HOLD,                      // LIFT GOAL
    DRIVEDIST, 100, 270, 5, 1,          // DRIVE AWAY FROM WALL
    DRIVEDIST, -100, 270, 20, 1.5,         // DRIVE INTO WALL FULLY
    PAUSE, 0.5, 
    DRIVE, 70, 270, 0.2,         // DRIVE OUT OF WALL A LITTLE

    TURN, 190, 2,                       // TURN READY FOR RED GOAL
    DRIVEDIST, -60, 190, 9, 5,         // DRIVE INTO GOAL
    TIPBASE, ON,                    // GRAB GOAL
    PAUSE, 0.5,                     // WAIT FOR GOAL GRAB
    DRIVEDIST, 80, 225, 10, 2,      // DRIVE AWAY FROM GOAL

    SIDEARM, READY, 
    TURN, 315, 1, 
    INTAKE, ON, 
    WOBBLE, ON, 
    DRIVETO, 100, 24, -26, 2, 
    TURN, 270, 2, 

    DRIVETO, 100, -3, -26, 2, 
    SIDEARM, HOLD, 
    DRIVETO, 100, -23, -26, 2, 
    TURN, 0, 2,

    DRIVETO, 40, -23, 10, 16,       // DRIVE TO COLLECT NEXT RINGS
    PAUSE, 0.5,
    DRIVETO, 40, -23, 48, 16,       // DRIVE TO COLLECT NEXT RINGS

    FACE, -63, 43, 2, 
    DRIVETO, 90, -63, 43, 3,       // DRIVE READY FOR PARK
    TURN, 180, 2,                   // FACE PLATFORM
    DRIVEDIST, 100, 180, 4, 2,    // DRIVE INTO PLATFORM
    DRIVE, -50, 180, 0.1,    // BREAK
    FRONTARM, PARK,             // PULL PLATFORM DOWN
    PAUSE, 2,               // WAIT FOR IT TO COME
    DRIVEDIST, 127, 180, 12, 2,    // DRIVE ONTO PLATFORM
    DRIVEDIST, 127, 180, 24, 2,    // DRIVE ONTO PLATFORM

    END,
};


double* next_entry = NULL;
double last_auton_time = 0;


double process_entry() {
    double result = *next_entry;
    next_entry++;
    return result;
}


void autonomous() {
    double auton_start_time = millis();       // Start time of auton mode
    double command_start_time = millis();     // Start time of each command

    double command_time_out = -1;             // Time-out limit of each command
    bool command_timed_out = false;           // Flag for if current command reached time-out
    
    int wait_condition = -1;                 // Condition type to wait until
    double wait_parameter = -1;              // Parameter to decide when wait is done
    
    // Point to correct auton routine
    next_entry = auton_ptr[which_auton];
    cout << auton_names[which_auton] << " Auton Starting\n";

    // Starting position
    set_position(process_entry(),process_entry(),process_entry());

    bool next_command = true;
    bool yeeting = false;

    while (true) {

        // Hekkin make sure te encoder wheels are down
        ADIAnalogIn encoder_lift ( {{ 7 , 8 }});

        if (next_command) {
            last_auton_time = (millis() - auton_start_time) / 1000; // Record time
            controller.print(0,0,"T: %f", last_auton_time);
            
            next_command = false;             // We are not ready for next command any more
            command_start_time = millis();    // Record the time the command was started
            command_time_out = -1;            // Default to no time-out limit
            command_timed_out = false;        // This command has not yet timed out
            
            cout << ((millis() - auton_start_time) / 1000) << ": ";

            // can't declare vars in switch case (but should really just make new scope within)
            double x_pos;
            double y_pos;
            double dir;
            double pos;

            switch ((int) process_entry()) {

                case TURN:
                    // Robot will turn to face the specified angle
                    cout << "TURN" << endl;
                    turn_correct = false;
                    drive_mode = DM_TURN;
                    drive_speed_target = 0;
                    drive_turn_target = process_entry();        // Angle to turn to (degrees)
                    command_time_out = process_entry() * 1000;
                    break;

                case FACE:
                    // Robot will turn to face the specified point
                    cout << "FACE" << endl;
                    turn_correct = false;
                    drive_mode = DM_FACE;
                    drive_speed_target = 0;
                    drive_target_x = process_entry();   // X target (field coord in inches)
                    drive_target_y = process_entry();   // Y target (field coord in inches)
                    command_time_out = process_entry() * 1000;
                    break;

                case DRIVE:
                    // Robot will drive in a direction, at a speed, for a time
                    cout << "DRIVE" << endl;
                    drive_mode = DM_TIME;
                    drive_speed_target = process_entry();   // Speed to drive (-127, 127)
                    drive_turn_target = process_entry();    // Direction to face (degrees)
                    command_time_out = process_entry() * 1000;
                    break;

                case DRIVEDIST:
                    // Robot will drive in a direction, at a speed, for a distance
                    cout << "DRIVEDIST" << endl;
                    drive_mode = DM_DISTANCE;
                    drive_speed_target = process_entry();   // Speed to drive (-127, 127)
                    drive_turn_target = process_entry();    // Direction to face (degrees)
                    drive_distance_target = process_entry();    // Distance to drive (inches)
                
                    // Record starting position
                    drive_starting_x = robot_x;
                    drive_starting_y = robot_y;
                    
                    command_time_out = process_entry() * 1000;
                    break;

                case END:
                    // End of routine
                    cout << "END" << endl;
                    controller.print(0,0,"E");
                    // Just chill bruh
                    break;

                case PAUSE:
                    // Do nothhing for a time
                    cout << "PAUSE" << endl;
                    command_time_out = process_entry() * 1000;
                    break;

                case WAIT:
                    // Do nothing until condition is met, eg. WAIT, LIFTABOVE, 1000, 5,
                    cout << "WAIT" << endl;
                    wait_condition = (int)process_entry();  // What to wait for (keyword)
                    wait_parameter = process_entry();       // Value to wait until (number)
                    command_time_out = process_entry() * 1000;
                    break;

                case DEPLOY:
                    // Deploys the robot
                    cout << "DEPLOY" << endl;
                    yeet_state = 10;
                    next_command = true;
                    break;

                case CHILLYEET:
                    // Deactivates yeet release piston
                    cout << "CHILLYEET" << endl;
                    yeet_state = 11;
                    next_command = true;
                    break;

                case DRIVETO:
                    // Robot will drive to target position on field
                    cout << "DRIVETO" << endl;
                    drive_mode = DM_GOTO;
                    drive_speed_target = process_entry();   // How fast to drive (-127, 127)

                    drive_target_x = process_entry();   // X target (field coord in inches)
                    drive_target_y = process_entry();   // Y target (field coord in inches)

                    drive_starting_x = robot_x;
                    drive_starting_y = robot_y;

                    drive_distance_target = pythag(drive_starting_x, drive_starting_y, drive_target_x, drive_target_y);
                    
                    command_time_out = process_entry() * 1000;
                    break;

                case SETPOS:
                    // Sets the robot's position on the field
                    cout << "SETPOS" << endl;
                    x_pos = process_entry(); // X coord (field coord in inches)
                    y_pos = process_entry(); // Y coord (field coord in inches)
                    set_position(x_pos, y_pos);
                    next_command = true;
                    break;

                case SETDIR:
                    // Sets the robot's direction on the field
                    cout << "SETDIR" << endl;
                    dir = process_entry(); // Direction (degrees)
                    set_direction(dir);
                    next_command = true;
                    break;

                case YEET:
                    // Hekkin yeeeeet
                    cout << "YEET" << endl;
                    yeet_distance = process_entry();
                    yeet_state = 1;
                    command_time_out = process_entry() * 1000;
                    yeeting = true;
                    break;

                case RETRACTYEET:
                    // Retracts the yeet wheel such that drive wheels are on the ground
                    cout << "RETRACTYEET" << endl;
                    yeet_state = 10;
                    next_command = true;
                    break;

                case INTAKE:
                    // Turns intake on or off
                    cout << "INTAKE" << endl;
                    limit_current = false;
                    intake = process_entry();
                    next_command = true;
                    break;

                case WOBBLE:
                    // Turns wobble on or off
                    cout << "WOBBLE" << endl;
                    limit_current = false;
                    intake_wobble = process_entry();
                    next_command = true;
                    break;

                case FRONTARM:
                    // Move front arm to a positon
                    cout << "FRONTARM" << endl;
                    limit_current = false;
                    front_lift_state = process_entry();
                    next_command = true;
                    break;

                case BACKARM:
                    // Move back arm to a positon
                    cout << "BACKARM" << endl;
                    limit_current = false;
                    back_lift_state = process_entry();
                    next_command = true;
                    break;

                case TIPBASE:
                    // Turns back tip latch on or off
                    cout << "TIPBASE" << endl;
                    tip_latch = process_entry();
                    next_command = true;
                    break;

                case FRONTDROP:
                    // Turns front base latch on or off
                    cout << "FRONTDROP" << endl;
                    front_latch_on = process_entry();
                    next_command = true;
                    break;

                case BACKDROP:
                    // Turns back base latch on or off
                    cout << "BACKDROP" << endl;
                    back_latch_on = process_entry();
                    next_command = true;
                    break;

                case SIDEARM:
                    // Move side arm to a positon
                    cout << "SIDEARM" << endl;
                    limit_current = false;
                    pos = process_entry();
                    if (pos == HOLD)
                        side_lift_target = this_robot.SIDE_LIFT_HOLD_POS;
                    else
                        side_lift_target = this_robot.SIDE_LIFT_READY_POS;
                    next_command = true;
                    break;
                
                case BRAKE:
                    // Hold drive in current position
                    cout << "BRAKE" << endl;
                    drive_mode = DM_BRAKE;
                    command_time_out = process_entry() * 1000;
                    break;

                case ULTRABOOST:
                    // Give all power to drive
                    cout << "ULTRABOOST" << endl;
                    limit_current = true;
                    next_command = true;
                    break;

                case UNBOOST:
                    // Give all power to drive
                    cout << "UNBOOST" << endl;
                    limit_current = false;
                    next_command = true;
                    break;

                default:
                    // Command not recognised
                    cout << "BAD COMMAND" << endl;
                    break;

            }
        }

        // Check if the command we were doing has a time-out limit set
        if (command_time_out > 0) {
            // Check if this limit has been reached
            if (millis() - command_start_time > command_time_out) {
                command_time_out = -1;        // Reset time-out limit
                command_timed_out = true;     // Set flag to say we timed out
                next_command = true;         // Ready for next command
                cout << "Timed out\n";
            }
        }

        // If we are doing wait command
        if (wait_condition > 0) {
            bool finished_wait = false;
            
            // Check what we are waiting for
            switch (wait_condition) {
                    
                case TIME:          // Time since auton start to be above time
                    if (millis() - auton_start_time > 1000 * wait_parameter) {
                        finished_wait = true;
                        cout << "Wait time done\n";
                    }
                    break;
                    
            }
            
            // If condition is met the we are done
            if (finished_wait) {
                wait_condition = -1;     // Stop waiting
                wait_parameter = -1;     // Reset parameter
                next_command = true;     // Ready for next command
            }
        }

        bool finished_drive = false;
        // Check if we've driven far enough
        if (drive_mode == DM_DISTANCE) {
            // If we've moved at least as far as we wanted to
            if (drive_distance_target < pythag(drive_starting_x, drive_starting_y, robot_x, robot_y)) {
                finished_drive = true;
            }
        }
        if (drive_mode == DM_GOTO) {
            /////// HACK for mitigating violent turn
            if (drive_distance_target - this_robot.DRIVE_PRECISION < pythag(drive_starting_x, drive_starting_y, robot_x, robot_y)) {
                finished_drive = true;
            }
            // If we've moved at least as far as we wanted to
            if (pythag(drive_target_x, drive_target_y, robot_x, robot_y) <= this_robot.DRIVE_PRECISION) {     // Drive within tolerance
                finished_drive = true;
            }
        }
        if ((drive_mode == DM_FACE) || (drive_mode == DM_TURN)) {
            if (turn_correct) {
                finished_drive = true;
            }
        }

        if (yeeting) {
            if (yeet_state == 10) {
                next_command = true;
                yeeting = false;
                cout << "Yeet done\n";
            }
        }

        // If we timed out, make sure to stop whatever it was we were doing
        if (command_timed_out) {
            
            // If we were waiting for a condition, we are not anymore
            if (wait_condition != -1) {
                wait_condition = -1;
                wait_parameter = -1;
                cout << "Wait timed out\n";
            }
            
            // If we were driving
            if (drive_mode != DM_USER) {
                finished_drive = true;
            }

            // If we were yeeting
            if (yeet_state != 10) {
                yeeting = false;
                yeet_state = 10;
            }
        }

        if (finished_drive) {
            drive_turn_target = -1;
            drive_speed_target = 0;
            drive_mode = DM_USER;
            next_command = true;
        }

        if (!next_command)
            delay(10);
    }

}

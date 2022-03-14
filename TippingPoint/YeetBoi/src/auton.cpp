#include "main.h"


                            // FIRST LINE: <X>, <Y>, <DIRECTION>
// Auton commands
#define TURN 1              // TURN, <FACEDIR>, <TIMEOUT>
#define DRIVE 2             // DRIVE, <SPEED>, <DRIVEDIR>, <TIMEOUT>
#define DRIVEDIST 3         // DRIVEDIST, <SPEED>, <DRIVEDIR>, <DISTANCE> <TIMEOUT>
#define LIFTPOS 4           // LIFTPOS, <POSITION>
#define WRISTPOS 5          // WRISTPOS, <POSITION>
#define ARMPOS 6            // ARMPOS <POSITION>
#define END 7               // END
#define PAUSE 8             // PAUSE, <TIMEOUT>
#define WAIT 9              // WAIT, <CONDITION>, <PARAMETER>, <TIMEOUT>
#define DROP 13             // DROP, <TIME>,
#define DEPLOY 14           // DEPLOY
#define DRIVETO 15          // DRIVETO, <SPEED>, <X>, <Y>, <TIMOUT>
#define SETPOS 16           // SETPOS, <X>, <Y>

#define BELLYPOS 17         // BELLYPOS <STATE>
#define BASEPOS 18          // BASEPOS <STATE>

#define READYSPIKE 19       // READYSPIKE
#define YEET 20             // YEET, <TIMEOUT>
#define RETRACTYEET 21      // RETRACTYEET

#define COLLECTRING 22      // COLLECTRING, <TIMEOUT>
#define DEPOSITPOS 23       // DEPOSITPOS, <LOCATION>

#define SETDIR 24           // SETDIR, <DIRECTION>
#define FACE 25             // TURNTO, <X>, <Y>, <TIMEOUT>

#define CHILLYEET 26        // CHILLYEET

#define SPINOPTICAL 27     // SCORES 8 RINGS ON THE TOP GOAL

// Depost Locations
#define FORWARD 1
#define LOWER 2
#define UPPER 3

#define BASEREADY 1
#define BASEHOLD 2
#define BASEDROP 3
#define BASELOWLOW 119

#define BELLYCOAST 1
#define BELLYDOWN 2
#define BELLYUP 3

// Wait conditions
#define LIFTBELOW 1
#define LIFTABOVE 2
#define TIME 3


int which_auton = 0;
int num_autons = 4;
string auton_names[] = {"MID", "LEFT", "RING_P", "SPINNER"};//, "SK_LEFT", "SK_RGHT"};
double* auton_ptr[] = {&mid_auton[0], &left_auton[0], &ring_practice_auton[0], &spinner[0]};//, &right_skills[0], &left_skills[0]};

double spinner[] = {

    0,0,0, // first line should always be robot position
    
    BASEPOS, BASEREADY, //get ready to grab the base
    PAUSE, 5, //pause for 5seconds
    BASEPOS, BASEHOLD, // basepos is command, basehold is argument 

    SPINOPTICAL,

    BASEPOS, BASEDROP,

    END,

};

double mid_auton[] = {
    -52, -38, 234,      // STARTING POS
    DEPLOY, 
    BASEPOS, BASEREADY,
    WRISTPOS, this_robot.SPIKE_WRIST_STORE_POS,

    // FIRST YEET
    DRIVEDIST, -10000000, 234, 52, 3, 
    ARMPOS, this_robot.SPIKE_ARM_STORE_POS,
    BASEPOS, BASEHOLD,
    BELLYPOS, BELLYUP,
    DRIVETO, 127, -48, -36, 5, 
    CHILLYEET,

    // GET SECOND BASE
    TURN, 95, 0.5,
    BELLYPOS, BELLYDOWN,
    TURN, 95, 0.5,
    // DRIVETO, 127, -5, -36, 3, 
    DRIVEDIST, 127, 95, 16, 3, 
    BELLYPOS, BELLYUP,
    PAUSE, 0.5,
    DRIVETO, -127, -42, -38, 2,
    // PAUSE, 0.5,

    // PLACE SECOND BASE
    TURN, 0, 1,
    // DRIVETO, 100, -40, -12, 2,
    DRIVEDIST, 100, 0, 20, 3, 
    // BELLYPOS, BELLYCOAST,
    PAUSE, 0.5,
    BELLYPOS, BELLYDOWN,
    // PAUSE, 0.5,
    DRIVETO, -100, -45, -33, 2,
    DEPOSITPOS, FORWARD,
    PAUSE, 0.5,
    DROP, 0.1,
    
    // LINE UP FOR RINGS
    BELLYPOS, BELLYUP,
    DRIVETO, -100, -53, -49, 3,
    FACE, 100, -52.5, 1.5,
    BELLYPOS, BELLYDOWN,
    PAUSE, 0.25, 
    READYSPIKE, 
    PAUSE, 0.25, 

    // COLLECT RINGS
    DRIVEDIST, 80, 90, 7.5, 2,
    PAUSE, 0.2, 
    COLLECTRING, 0.5, 
    COLLECTRING, 1,               // RING #1 OF ROWS

    DRIVEDIST, 80, 90, 3.5, 1, 
    // PAUSE, 0.2,
    COLLECTRING, 1,               // 2

    DRIVEDIST, 80, 90, 3, 1, 
    // PAUSE, 0.2,
    COLLECTRING, 1,               // 3

    DRIVEDIST, 80, 90, 3, 1, 
    // PAUSE, 0.2,
    COLLECTRING, 1,               // 4

    DRIVEDIST, 80, 90, 3, 1, 
    // PAUSE, 0.2,
    COLLECTRING, 1,               // LAST OF THE ROW

    DRIVEDIST, 80, 90, 3, 1, 
    // PAUSE, 0.2,
    COLLECTRING, 1,               // SPARE FOR GOOD LUCK

    // LINE UP TO ALLIANCE BASE
    DEPOSITPOS, FORWARD, 
    DRIVETO, -100, -41, -36, 3, 
    TURN, 185, 1.5, 

    // GO TO ALLIANCE BASE, DROP RINGS
    DRIVEDIST, 100, 185, 24, 1.5, 
    DROP, 0.75, 
    PAUSE, 0.5, 
    DROP, 0.75, 
    PAUSE, 0.25, 
    DROP, 0.375, 
    PAUSE, 0.25, 

    ///////////////////////////
    // HACK FOR SKIPPING
    // BASEPOS, BASEHOLD,
    // DRIVETO, -127, -24, -24, 2, 
    ///////////////////////////

    // PUSH ALLIANCE BASE INTO CORNER
    DRIVETO, 127, -60, -60, 2, 
    DROP, -1, 
    DRIVEDIST, 127, 235, 5, 0.2, 
    DRIVEDIST, -80, 265, 8, 2, 
    READYSPIKE,

    TURN, 345, 1, 
    DRIVEDIST, -80, 345, 10, 2, 
    TURN, 315, 0.5, 

    // AS MANY MATCH LOADS AS POSSIBLE

    DRIVEDIST, 60, 315, 3, 2, 

    DRIVEDIST, 60, 315, 1, 2, 
    COLLECTRING, 0.5,               // 1
    PAUSE, 0.5, 

    DRIVEDIST, 70, 315, 0.5, 2, 
    COLLECTRING, 0.5,               // 2
    PAUSE, 0.5, 
    
    DRIVEDIST, 70, 315, 0.5, 2, 
    COLLECTRING, 0.5,               // 3
    PAUSE, 0.5, 

    DRIVEDIST, 70, 315, 0.5, 2, 
    COLLECTRING, 0.5,               // 4
    PAUSE, 0.5, 

    DRIVEDIST, 70, 315, 0.5, 2, 
    COLLECTRING, 0.5,               // 5
    PAUSE, 0.5, 

    DRIVEDIST, 70, 315, 0.5, 2, 
    COLLECTRING, 0.5,               // 6
    PAUSE, 0.5, 

    DRIVEDIST, 70, 315, 0.5, 2, 
    COLLECTRING, 0.5,               // 7
    PAUSE, 0.5, 

    DRIVEDIST, 70, 315, 0.5, 2, 
    COLLECTRING, 0.5,               // 8
    PAUSE, 0.5, 

    DRIVEDIST, 70, 315, 0.5, 2, 
    COLLECTRING, 0.5,               // 9
    PAUSE, 0.5, 

    DRIVEDIST, 70, 315, 0.5, 2, 
    COLLECTRING, 0.5,               // 10
    PAUSE, 0.5, 

    DRIVEDIST, 70, 315, 0.5, 2, 
    COLLECTRING, 0.5,               // 11
    PAUSE, 0.5, 

    DRIVEDIST, 70, 315, 0.5, 2, 
    COLLECTRING, 0.5,               // 12
    PAUSE, 0.5, 

    END,
};

double left_auton[] = {
    -43, 49, 278,      // STARTING POS
    DEPLOY, 
    BASEPOS, BASEREADY,
    WRISTPOS, this_robot.SPIKE_WRIST_STORE_POS,

    // FIRST YEET
    DRIVEDIST, -10000000, 276, 39, 3, 
    ARMPOS, this_robot.SPIKE_ARM_STORE_POS,
    BASEPOS, BASEHOLD,
    // BASEPOS, BASELOWLOW,
    BELLYPOS, BELLYUP,
    DRIVETO, 127, -18, 50, 50,  // TO STOP US CONTINUING IF STUCK
    BASEPOS, BASEHOLD,
    TURN, 180, 1, 
    DRIVETO, -127, -18, 60, 2,
    CHILLYEET,

    DROP, 0.1,
    BELLYPOS, BELLYDOWN,
    READYSPIKE,
    TURN, 180, 1.5, 

    DRIVEDIST, 60, 180, 11, 2, 
    COLLECTRING, 0.5,               // 1
    PAUSE, 0.2, 
    COLLECTRING, 0.5,
    PAUSE, 0.5, 


    TURN, 200, 0.25, 
    TURN, 180, 0.5, 
    DRIVEDIST, 50, 180, 3, 4, 
    COLLECTRING, 0.5,               // 2
    PAUSE, 0.2, 
    COLLECTRING, 0.5,
    PAUSE, 0.5, 

    TURN, 160, 0.25, 
    TURN, 180, 0.5, 
    DRIVEDIST, 50, 180, 4, 2, 
    COLLECTRING, 0.5,               // 3
    PAUSE, 0.2, 
    COLLECTRING, 0.5,
    PAUSE, 0.5, 

    TURN, 200, 0.25, 
    TURN, 180, 0.5, 
    DRIVEDIST, 50, 180, 4, 2, 
    COLLECTRING, 0.5,               // 4
    PAUSE, 0.2, 
    COLLECTRING, 0.5,
    PAUSE, 0.5, 

    TURN, 160, 0.25, 
    TURN, 180, 0.5, 
    DRIVEDIST, 50, 180, 4, 2, 
    COLLECTRING, 0.5,               // 5
    PAUSE, 0.2, 
    COLLECTRING, 0.5,
    PAUSE, 0.5, 

    TURN, 200, 0.25, 
    TURN, 180, 0.5, 
    DRIVEDIST, 50, 180, 4, 2, 
    COLLECTRING, 0.5,               // 6
    PAUSE, 0.2, 
    COLLECTRING, 0.5,
    PAUSE, 0.5,


    DRIVETO, -127, -20, 52, 3,
    TURN, 90, 1,                    // LINE UP FOR ROW OF RINGS
    DRIVEDIST, 60, 90, 12, 2,

    PAUSE, 0.2,
    COLLECTRING, 0.5,               // 1

    DRIVEDIST, 100, 90, 1.5, 1, 
    PAUSE, 0.2,
    COLLECTRING, 0.5,               // 2

    DRIVEDIST, 100, 90, 1.5, 1, 
    PAUSE, 0.2,
    COLLECTRING, 0.5,               // 4
    PAUSE, 0.2,
    COLLECTRING, 0.5,               // MAKE SURE NICE

    DEPOSITPOS, FORWARD, 
    DRIVETO, -127, -27, 52, 2, 
    FACE, -60, 34, 1.5, 
    DRIVETO, 80, -60, 34, 1.5, 
    TURN, 270, 1.5, 
    DRIVEDIST, -80, 270, 5, 1.5, 
    DRIVEDIST, 50, 270, 7, 1.5, 

    DROP, 0.75, 
    PAUSE, 0.5, 
    DROP, 0.75, 
    PAUSE, 0.5, 
    DROP, 0.75, 

    DRIVETO, 127, -60, 60, 2, 
    DRIVEDIST, -127, 315, 5, 1.5, 

    END,
};

// double right_skills[] = {
//     -60,-45,270,   
//     END,
// };

// double left_skills[] = {
//     -60,-45,270,   
//     END,
// };


double ring_practice_auton[] = {
    0, 0, 315,      // STARTING POS
    DEPLOY, 

    READYSPIKE, 
    PAUSE, 5, 
    CHILLYEET,

    // AS MANY MATCH LOADS AS POSSIBLE

    DRIVEDIST, 60, 315, 1, 2, 
    COLLECTRING, 0.75,               // 1
    PAUSE, 0.5, 

    DRIVEDIST, 70, 315, 0.5, 2, 
    COLLECTRING, 0.5,               // 2
    PAUSE, 0.5, 
    
    DRIVEDIST, 70, 315, 0.5, 2, 
    COLLECTRING, 0.5,               // 3
    PAUSE, 0.5, 

    DRIVEDIST, 70, 315, 0.5, 2, 
    COLLECTRING, 0.5,               // 4
    PAUSE, 0.5, 

    DRIVEDIST, 70, 315, 0.5, 2, 
    COLLECTRING, 0.5,               // 5
    PAUSE, 0.5, 

    DRIVEDIST, 70, 315, 0.5, 2, 
    COLLECTRING, 0.5,               // 6
    PAUSE, 0.5, 

    DRIVEDIST, 70, 315, 0.5, 2, 
    COLLECTRING, 0.5,               // 7
    PAUSE, 0.5, 

    DRIVEDIST, 70, 315, 0.5, 2, 
    COLLECTRING, 0.5,               // 8
    PAUSE, 0.5, 

    DRIVEDIST, 70, 315, 0.5, 2, 
    COLLECTRING, 0.5,               // 9
    PAUSE, 0.5, 

    DRIVEDIST, 70, 315, 0.5, 2, 
    COLLECTRING, 0.5,               // 10
    PAUSE, 0.5, 

    DRIVEDIST, 70, 315, 0.5, 2, 
    COLLECTRING, 0.5,               // 11
    PAUSE, 0.5, 

    DRIVEDIST, 70, 315, 0.5, 2, 
    COLLECTRING, 0.5,               // 12
    PAUSE, 0.5, 

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

    while (true) {

        if (next_command) {
            last_auton_time = (millis() - auton_start_time) / 1000; // Record time
            controller.print(0,0,"T: %f", last_auton_time);
            
            next_command = false;             // We are not ready for next command any more
            command_start_time = millis();    // Record the time the command was started
            command_time_out = -1;            // Default to no time-out limit
            command_timed_out = false;        // This command has not yet timed out
            
            cout << ((millis() - auton_start_time) / 1000) << ": ";

            switch ((int) process_entry()) {

                case TURN:
                    cout << "TURN" << endl;
                    drive_mode = DM_TURN;
                    drive_speed_target = 0;
                    drive_turn_target = process_entry();
                    command_time_out = process_entry() * 1000;
                    break;

                case FACE:
                    cout << "FACE" << endl;
                    drive_mode = DM_FACE;
                    drive_speed_target = 0;
                    drive_target_x = process_entry();
                    drive_target_y = process_entry();
                    command_time_out = process_entry() * 1000;
                    break;

                case DRIVE:
                    cout << "DRIVETIME" << endl;
                    drive_mode = DM_TIME;
                    drive_speed_target = process_entry();
                    drive_turn_target = process_entry();
                    command_time_out = process_entry() * 1000;
                    break;

                case DRIVEDIST:
                    cout << "DRIVEDIST" << endl;
                    drive_mode = DM_DISTANCE;
                    drive_speed_target = process_entry();
                    drive_turn_target = process_entry();
                    drive_distance_target = process_entry();
                
                    // Record starting position
                    drive_starting_x = robot_x;
                    drive_starting_y = robot_y;
                    
                    command_time_out = process_entry() * 1000;
                    break;

                case LIFTPOS:
                    cout << "LIFTPOS" << endl;
                    spike_arm_state = -1;
                    base_right_state = -1;
                    lift_state = -1;
                    lift_target = process_entry();
                    next_command = true;
                    break;

                case WRISTPOS:
                    cout << "WRISTPOS" << endl;
                    spike_arm_state = -1;
                    base_right_state = -1;
                    lift_state = -1;
                    spike_wrist_target = process_entry();
                    next_command = true;
                    break;

                case ARMPOS:
                    cout << "ARMPOS" << endl;
                    spike_arm_state = -1;
                    base_right_state = -1;
                    lift_state = -1;
                    spike_arm_target = process_entry();
                    next_command = true;
                    break;

                case END:
                    cout << "END" << endl;
                    controller.print(0,0,"E");
                    // Just chill bruh
                    break;

                case PAUSE:
                    cout << "PAUSE" << endl;
                    command_time_out = process_entry() * 1000;
                    break;

                case WAIT:
                    cout << "WAIT" << endl;
                    wait_condition = (int)process_entry();
                    wait_parameter = process_entry();
                    command_time_out = process_entry() * 1000;
                    break;

                case DROP:
                    cout << "DROP" << endl;
                    spike_drop = true;
                    command_time_out = process_entry() * 1000;
                    if (command_time_out < 0) {
                        next_command = true;
                    }
                    break;

                case DEPLOY:
                    cout << "DEPLOY" << endl;
                    yeet_state = 10;
                    belly_grab_state = BELLYDOWN;
                    spike_arm_target = 1600;
                    next_command = true;
                    break;

                case CHILLYEET:
                    cout << "CHILLYEET" << endl;
                    yeet_state = 11;
                    next_command = true;
                    break;

                case DRIVETO:
                    cout << "DRIVETO" << endl;
                    drive_mode = DM_GOTO;
                    drive_speed_target = process_entry();

                    drive_target_x = process_entry();
                    drive_target_y = process_entry();
                    
                    command_time_out = process_entry() * 1000;
                    break;

                case SETPOS:
                    cout << "STEPOS" << endl;
                    set_position(process_entry(), process_entry());
                    next_command = true;
                    break;

                case SETDIR:
                    cout << "STEDIR" << endl;
                    set_direction(process_entry());
                    next_command = true;
                    break;

                case BELLYPOS:
                    cout << "BELLYPOS" << endl;
                    belly_grab_state = (int)process_entry();
                    next_command = true;
                    break;

                case BASEPOS:
                    cout << "BASEPOS" << endl;
                    base_lift_state = (int)process_entry();
                    next_command = true;
                    break;

                case READYSPIKE:
                    cout << "READYSPIKE" << endl;
                    lift_state = -1;
                    base_right_state = -1;
                    spike_arm_state = 1;
                    next_command = true;
                    break;

                case YEET:
                    yeet_state = 1;
                    command_time_out = process_entry() * 1000;
                    break;

                case RETRACTYEET:
                    yeet_state = 10;
                    next_command = true;
                    break;

                case COLLECTRING:
                    cout << "COLLECTRING" << endl;
                    lift_state = -1;
                    base_right_state = -1;
                    spike_arm_state = 10;
                    command_time_out = process_entry() * 1000;
                    break;

                case DEPOSITPOS:
                    cout << "DEPOSITPOS" << endl;
                    lift_state = (int)process_entry();
                    base_right_state = -1;
                    spike_arm_state = -1;
                    next_command = true;
                    break;

                case SPINOPTICAL:
                    cout << "SPINOPTICAL" << endl;
                    seek_sticker = true;
                    break;

                default:        // Command not recognised
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
                    
                case LIFTABOVE:     // Lift to be above position
                    if (lift_pos >= wait_parameter) {
                        finished_wait = true;
                        cout << "Lift above done\n";
                    }
                    break;
                    
                case LIFTBELOW:     // Lift to be below position
                    if (lift_pos <= wait_parameter) {
                        finished_wait = true;
                        cout << "Lift below done\n";
                    }
                    break;
                    
                case TIME:          // Time since auton start to be above time
                    if (millis() - auton_start_time > 1000 * wait_parameter) {
                        finished_wait = true;
                        cout << "Wait time done\n";
                    }
                    break;
                    
            }
            
            // If condition is met then we are done
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
            // If we've moved at least as far as we wanted to
            if (pythag(drive_target_x, drive_target_y, robot_x, robot_y) <= this_robot.DRIVE_PRECISION) {     // Drive within tolerance
                finished_drive = true;
            }
        }

        // we just saw the sticker -- stop
        if (seek_sticker && has_base && is_sticker){
            seek_sticker = false;
            next_command = true;
            cout << "FINISHED FINDING STICKER" << endl;
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
            
            // If we were dropping rings
            if (spike_drop) {
                spike_drop = false;
            }

            // If we were yeeting
            if (yeet_state != 10) {
                yeet_state = 10;
            }

            // If we were grabbing ring
            if ((spike_arm_state == 2) || (spike_arm_state == 10)) {
                spike_arm_state = 1;
            }

        }

        if (finished_drive) {
            drive_turn_target = -1;
            drive_speed_target = 0;
            drive_mode = DM_USER;
            next_command = true;
        }

        delay(20);
    }

}

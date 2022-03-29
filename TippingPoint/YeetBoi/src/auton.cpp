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
#define SPIKEBACKWARDSCORE 28    // SPIKE_BACKWARDS_SCORE
#define SCOREHIGH 29


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
#define SPINCOMPLETE 4


int which_auton = 0;
int num_autons = 4;
stristring auton_names[] = {"MID", "LEFT", "SPINNER", "SK_LEFT", "SK_RGHT"};
double* auton_ptr[] = {&mid_auton[0], &left_auton[0], &spinner[0], &left_skills[0], &right_skills[0]};

// sample auton to test optical sensor
double spinner[] = {

    0,0,0, // first line should always be robot position

    // BASEPOS, BASEREADY, //get ready to grab the base
    // PAUSE, 5, //pause for 5seconds
    BASEPOS, BASEHOLD, // basepos is command, basehold is argument 
    
    PAUSE, 1, //pause
    DEPOSITPOS, UPPER, //drop dose rings
    SPINOPTICAL,
    WAIT, SPINCOMPLETE, -1, 300,  //wait requires an integer parameter that we won't use, I just put -1 arbitrarily
                                // waits 5 seconds for the bot to find the sticker and stop spinning the base
    PAUSE, 1,
    DROP, 1.5, 

    SPINOPTICAL,
    WAIT, SPINCOMPLETE, -1, 300,    
    PAUSE, 1,
    DROP, 1,
    PAUSE, 0.5,
    DROP, 1,

    READYSPIKE, //drop dose rings
    // BASEPOS, BASEDROP, //release the base (this is mostly to relax the motors for easy reset)

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
    DRIVETO, 127, -36, -36, 5, 
    CHILLYEET,

    // GET SECOND BASE
    FACE, -5, -36, 0.5,
    BELLYPOS, BELLYDOWN,
    FACE, -5, -36, 0.5,
    // DRIVETO, 127, -5, -36, 3, 
    DRIVEDIST, 127, 93, 14, 3, 
    BELLYPOS, BELLYUP,
    PAUSE, 0.5,
    DRIVETO, -127, -43, -38, 2,
    // PAUSE, 0.5,

    // PLACE SECOND BASE
    FACE, -43, -12, 1,
    // DRIVETO, 100, -40, -12, 2,
    DRIVEDIST, 100, 0, 20, 3, 
    // BELLYPOS, BELLYCOAST,
    PAUSE, 0.5,
    BELLYPOS, BELLYDOWN,
    // PAUSE, 0.5,
    DRIVETO, -100, -40, -36, 2,
    DEPOSITPOS, FORWARD,
    PAUSE, 0.5,
    DROP, 0.1,
    
    // LINE UP FOR RINGS
    BELLYPOS, BELLYUP,
    DRIVETO, -100, -46, -49, 3,
    FACE, 100, -52.5, 1.5,
    BELLYPOS, BELLYDOWN,
    PAUSE, 0.25, 
    READYSPIKE, 
    PAUSE, 0.25, 

    // COLLECT RINGS
    DRIVEDIST, 60, 90, 7, 2,
    PAUSE, 0.2, 
    COLLECTRING, 0.5, 
    COLLECTRING, 1,               // RING #1 OF ROWS

    DRIVEDIST, 60, 90, 3, 1, 
    // PAUSE, 0.2,
    COLLECTRING, 1,               // 2

    DRIVEDIST, 60, 90, 3, 1, 
    // PAUSE, 0.2,
    COLLECTRING, 1,               // 3

    DRIVEDIST, 60, 90, 3, 1, 
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
    DRIVETO, -100, -37, -36, 3, 
    TURN, 180, 1.5, 

    // GO TO ALLIANCE BASE, DROP RINGS
    DRIVEDIST, 100, 180, 20, 1.5, 
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
    DRIVEDIST, -80, 250, 8, 2, 
    READYSPIKE,

    TURN, 345, 1, 
    DRIVEDIST, -80, 345, 10, 2, 
    TURN, 315, 0.5, 

    // AS MANY MATCH LOADS AS POSSIBLE

    DRIVEDIST, 60, 315, 6, 2, 
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


    // TO ALLOW FOR NEW METHOD OF DEPLOY
    LIFTPOS, 200,


    // FIRST YEET
    DRIVEDIST, -10000000, 276, 41, 3, 
    ARMPOS, this_robot.SPIKE_ARM_STORE_POS,
    BASEPOS, BASEHOLD,
    // BASEPOS, BASELOWLOW,
    BELLYPOS, BELLYUP,


    // TO ALLOW FOR NEW METHOD OF DEPLOY
    LIFTPOS, 0,


    DRIVETO, 127, -18, 50, 50,  // TO STOP US CONTINUING IF STUCK
    BASEPOS, BASEHOLD,
    TURN, 180, 1, 
    DRIVETO, -127, -18, 60, 2,
    CHILLYEET,


    // TO ALLOW FOR NEW METHOD OF DEPLOY
    LIFTPOS, -1,


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
    DRIVETO, -127, -27, 60, 2, 
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

double right_skills[] = {
    0, 0, 0,
    DEPLOY,
    BASEPOS, BASEREADY,
    PAUSE,0.5, 

    SPIKEBACKWARDSCORE, 
    DRIVEDIST, -127, 0, 100, 2,
    DROP, 1, 
    DRIVEDIST, -127, 0, 100, 0.5,
    DROP, 2,
    DRIVEDIST, -127, 0, 100, 2,
    DROP, 10,

    END,
};

double left_skills[] = {
    -60, 48, 0,
    DEPLOY,
    BASEPOS, BASEREADY,
    PAUSE, 1, 

    DRIVEDIST, -127, 0, 10, 2, 
    DRIVEDIST, 127, 30, 10, 2,

    FACE, 0, 36, 2,

    DRIVETO, 127, 0, 36, 10,
    DRIVEDIST, 127, 90, 100, 2,

    END,
};


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

            // can't declare vars in switch case (but should really just make new scope within)
            double x_pos;
            double y_pos;
            double dir;

            switch ((int) process_entry()) {

                case TURN:
                    // Robot will turn to face the specified angle
                    cout << "TURN" << endl;
                    drive_mode = DM_TURN;
                    drive_speed_target = 0;
                    drive_turn_target = process_entry();        // Angle to turn to (degrees)
                    command_time_out = process_entry() * 1000;
                    break;

                case FACE:
                    // Robot will turn to face the specified point
                    cout << "FACE" << endl;
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

                case LIFTPOS:
                    // Move the four-bar lift to a speficied height
                    cout << "LIFTPOS" << endl;
                    spike_arm_state = -1;
                    base_right_state = -1;
                    lift_state = -1;
                    lift_target = process_entry();  // Height for the lift (encoder ticks)
                    next_command = true;
                    break;

                case WRISTPOS:
                    // Move the spike wrist to a specified angle
                    cout << "WRISTPOS" << endl;
                    spike_arm_state = -1;
                    base_right_state = -1;
                    lift_state = -1;
                    spike_wrist_target = process_entry();   // Angle for the wrist (degrees rel. to floor)
                    next_command = true;
                    break;

                case ARMPOS:
                    // Move the spike arm to a specified position
                    cout << "ARMPOS" << endl;
                    spike_arm_state = -1;
                    base_right_state = -1;
                    lift_state = -1;
                    spike_arm_target = process_entry(); // Position for the arm (encoder ticks)
                    next_command = true;
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

                case DROP:
                    // Drops rings from spike
                    cout << "DROP" << endl;
                    spike_drop = true;
                    command_time_out = process_entry() * 1000;
                    if (command_time_out < 0) {
                        next_command = true;
                    }
                    break;

                case DEPLOY:
                    // Deploys the robot
                    cout << "DEPLOY" << endl;
                    yeet_state = 10;
                    belly_grab_state = BELLYDOWN;
                    spike_arm_target = 1600;
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

                case BELLYPOS:
                    // Moves bellygrab to desired position
                    cout << "BELLYPOS" << endl;
                    belly_grab_state = (int)process_entry();    // Position (keyword)
                    next_command = true;
                    break;

                case BASEPOS:
                    // Moves base lift-turner to desired position
                    cout << "BASEPOS" << endl;
                    base_lift_state = (int)process_entry(); // Position (keyword)
                    next_command = true;
                    break;

                case READYSPIKE:
                    // Moves the spike to the ready position
                    cout << "READYSPIKE" << endl;
                    lift_state = -1;
                    base_right_state = -1;
                    spike_arm_state = 1;
                    next_command = true;
                    break;

                case YEET:
                    // Hekkin yeeeeet
                    cout << "YEET" << endl;
                    yeet_state = 1;
                    command_time_out = process_entry() * 1000;
                    break;

                case RETRACTYEET:
                    // Retracts the yeet wheel such that drive wheels are on the ground
                    cout << "RETRACTYEET" << endl;
                    yeet_state = 10;
                    next_command = true;
                    break;

                case COLLECTRING:
                    // Stamps down with the spike to pick up a ring
                    cout << "COLLECTRING" << endl;
                    lift_state = -1;
                    base_right_state = -1;
                    spike_arm_state = 10;
                    command_time_out = process_entry() * 1000;
                    break;

                case DEPOSITPOS:
                    // Moves the spike to the desired position for scoring
                    cout << "DEPOSITPOS" << endl;
                    lift_state = (int)process_entry();  // Scoring position (keyword)
                    base_right_state = -1;
                    spike_arm_state = -1;
                    next_command = true;
                    break;

                case SPIKEBACKWARDSCORE:
                    // Moves the spike to the position for backwards scoring on a alliance goal
                    cout << "SPIKEBACKWARDSCORE" << endl;
                    spike_arm_state = -1;
                    base_right_state = -1;
                    lift_state = -1;
                    spike_wrist_target = this_robot.ALLIANCE_HELD_WRIST_POS;
                    spike_arm_target = this_robot.ALLIANCE_HELD_ARM_POS;
                    lift_target = 0;
                    next_command = true;
                    break;

                case SPINOPTICAL:
                    cout << "SPINOPTICAL" << endl;
                    optical_state = LOOK_FOR_YELLOW;
                    next_command = true;
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
                case SPINCOMPLETE:
                    if (optical_state == DO_NOTHING){
                        finished_wait = true;
                        cout << "Sticker found\n";
                    }
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

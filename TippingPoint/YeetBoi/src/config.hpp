////////////////////////////////////////////////
// LITTLE BOT CONFIG
//

#ifndef __CONFIG_HPP__
#define __CONFIG_HPP__

#include <string>

////////////////////////////////////////////////////////////////
// Anything that we will use in multiple files
// must be declared as extern in this file.
// Must only be initialised w/o extern in one file.
// Then all files can view and use it
//

using namespace pros;
using namespace std;

#define DM_USER -1      // Default user control
#define DM_YEET_FORWARD 1
#define DM_YEET_BACKWARD 2
#define DM_DISTANCE 3   // Drive straight for distance
#define DM_TIME 4       // Drive straight for time
#define DM_GOTO 5      // Drive to a location
#define DM_TURN 6       // Turn on spot to angle
#define DM_FACE 7       // Turn on spot to face point

#define TURBO E_MOTOR_GEARSET_6
#define SPEED E_MOTOR_GEARSET_18
#define TORQUE E_MOTOR_GEARSET_36

//optical states
#define DO_NOTHING  -1
#define LOOK_FOR_STICKER 2
#define LOOK_FOR_YELLOW 3
#define FUCK_GO_BACK 4



extern double* auton_ptr[];
extern double mid_auton[];
extern double left_auton[];
extern double ring_practice_auton[];
extern double right_skills[];
extern double left_skills[];
extern double test_auton[];
extern double spinner[];

extern int which_robot;
extern string robot_names[];

extern int num_autons;
extern int which_auton;
extern string auton_names[];
extern double* auton_prt[];

// Main task prototypes
extern void run_drive(void* params);
extern void run_display(void* params);
extern void run_belly_grab(void* params);
extern void run_base_lift(void* params);
extern void run_lift(void* params);
extern void run_yeet(void* params);

//Optical sensor functions
extern void init_optical();
extern void run_optical(void* params);

// Global functions - Position tracking
extern void track_position();
extern void set_direction(double dir);
extern double pythag(double x1, double y1, double x2, double y2);
extern void set_position(double x, double y, double d);
extern void set_position(double x, double y);
extern void init_tracking();

// Robot Config Functions
extern void init_robot_white();
extern void init_robot_black();

// Controller
extern Controller controller;

// Motors
extern Motor drive_right_1;
extern Motor drive_right_2;
extern Motor drive_right_3;
extern Motor drive_left_1;
extern Motor drive_left_2;
extern Motor drive_left_3;

extern Motor belly_grab;
extern Motor base_lift;
extern Motor base_release;
extern Motor base_rotate;

extern Motor spike_wrist;
extern Motor spike_arm;
extern Motor spike;

extern Motor lift_left;
extern Motor lift_right;

// Global variables
extern double robot_theta;
extern double robot_x;
extern double robot_y;

extern double belly_grab_pos;
extern double belly_grab_target;

extern int base_lift_state;
extern double base_lift_pos;
extern double base_lift_target;
extern double base_release_pos;
extern double base_release_target;

extern double spike_arm_pos;
extern double spike_arm_target;
extern double spike_wrist_pos;
extern double spike_wrist_target;

extern double lift_pos;
extern double lift_target;

extern double spike_speed;

extern bool spike_drop;

extern int spike_arm_state;
extern int lift_state;
extern int base_right_state;

extern int yeet_state;
extern double yeet_distance;
extern int belly_grab_state;

extern int drive_mode;

extern double drive_speed_target;
extern double drive_turn_target;
extern double drive_target_x;
extern double drive_target_y;
extern double drive_starting_x;
extern double drive_starting_y;
extern double drive_distance_target;

extern double last_auton_time;

// Sensors
extern ADIEncoder left_encoder;
extern ADIEncoder middle_encoder;
extern ADIEncoder right_encoder;

extern double brightness;
extern double hue;
extern int32_t proximity;
extern double saturation;
extern pros::c::optical_rgb_s_t RGB_values;

extern int optical_state;
extern bool has_base;

extern bool is_black;
extern bool is_yellow;
extern bool is_red;

// Pistons
extern ADIDigitalOut yeet_release;
extern ADIDigitalOut yeet_retract;


struct RobotParams {
    string name;
    double BASE_LIFT_YEET_POS;

    double SPIKE_ARM_RATE;
    double SPIKE_WRIST_RATE;
    double SPIKE_SHAKE_AMPLITUDE;

    double BASE_RIGHT_WRIST_POS_1;
    double BASE_RIGHT_ARM_POS_1;
    double BASE_RIGHT_WRIST_POS_2;
    double BASE_RIGHT_ARM_POS_2;
    double BASE_RIGHT_WRIST_POS_3;
    double BASE_RIGHT_ARM_POS_3;

    double SPIKE_WRIST_GRAB_POS;
    double SPIKE_WRIST_READY_POS;
    double SPIKE_ARM_READY_POS;
    double SPIKE_ARM_GRAB_POS;
    double SPIKE_WRIST_RANGE;

    double SPIKE_ARM_CHASSIS_CLEAR;
    double SPIKE_WRIST_CHASSIS_CLEAR;
    double SPIKE_WRIST_CHASSIS_COLLIDE;

    double SPIKE_WRIST_STORE_POS;
    double SPIKE_ARM_STORE_POS;

    double ALLIANCE_HELD_ARM_POS;
    double ALLIANCE_HELD_WRIST_POS;

    double SPIKE_DROP_SPEED;
    double SPIKE_RETURN_SPEED ;

    double LIFT_RATE;

    double LEFT_LIFT_OFFSET;
    double SPIKE_WRIST_ALLIANCE_GOAL_POS;
    double SPIKE_ARM_ALLIANCE_GOAL_POS;
    double LIFT_ALLIANCE_GOAL_POS;

    double SPIKE_WRIST_LOW_GOAL_POS;
    double SPIKE_ARM_LOW_GOAL_POS;
    double LIFT_LOW_GOAL_POS;

    double SPIKE_WRIST_HIGH_GOAL_POS;
    double SPIKE_ARM_HIGH_GOAL_POS;
    double LIFT_HIGH_GOAL_POS;

    double BASE_LIFT_RATE;
    double BASE_RELEASE_RATE;

    double SPIKE_WRIST_ADJUST_AMOUNT;
    double SPIKE_ARM_ADJUST_AMOUNT;

    double BASE_LIFT_READY_POS;
    double BASE_LIFT_HOLD_POS;
    double BASE_LIFT_DROP_POS;
    double BASE_SPIN_OFFSET;
    double BASE_RELEASE_DROP_POS;
    double BASE_RELEASE_READY_POS;
    double BASE_RELEASE_HOLD_POS;


    double BELLY_GRAB_RATE_DOWN;
    double BELLY_GRAB_RATE_UP;

    double BELLY_GRAB_DOWN;
    double BELLY_GRAB_UP;

    double SLEW_STEP_FORWARD;
    double SLEW_STEP_TURN;

    // How many inches of travel does the robot move per tick of the encoder
    double INCHES_PER_TICK;

    // How far apart are the tracking wheels
    double TRACKING_DIAMETER;

    // Fudge Factors
    double FORWARD_FUDGE_FACTOR;
    double STRAFE_FUDGE_FACTOR;
    double ROTATIONAL_FUDGE_FACTOR;

    // Auton drive stuff
    double DRIVE_TURN_RATE;
    double DRIVE_DRIVE_TURN_RATE;
    double DRIVE_RATE;
    double MIN_DRIVE_SPEED = 40.0;
    double MAX_TURN_SPEED;
    double MIN_TURN_SPEED;
    int TURN_PULSE_ON;
    int TURN_PULSE_OFF;
    int TURN_PRECISION;
    double DRIVE_PRECISION;

    double SPIKE_WRIST_LOCK_BELLY;

    double SPIKE_STAMP_TIME;
};

extern RobotParams this_robot;

#endif




////////////////////////////////////////////////
// LITTLE BOT CONFIG
//

#ifndef __CONFIG_HPP__
#define __CONFIG_HPP__

////////////////////////////////////////////////////////////////
// Anything that we will use in multiple files
// must be declared as extern in this file.
// Must only be initialised w/o extern in one file.
// Then all files can view and use it
//


using namespace pros;

#define DM_USER -1
#define DM_YEET_FORWARD 1
#define DM_YEET_BACKWARD 2

#define TURBO E_MOTOR_GEARSET_6
#define SPEED E_MOTOR_GEARSET_18
#define TORQUE E_MOTOR_GEARSET_36

// Main task prototypes
extern void run_drive(void* params);
extern void run_display(void* params);
extern void run_belly_grab(void* params);
extern void run_base_lift(void* params);
extern void run_lift(void* params);
extern void run_yeet(void* params);

// Global functions - Position tracking
extern void track_position();
extern void set_direction(double dir);
extern double pythag(double x1, double y1, double x2, double y2);
extern void set_position(double x, double y, double d);
extern void set_position(double x, double y);
extern void init_tracking();

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

extern int yeet_state;
extern int belly_grab_state;

extern int drive_mode;


// Sensors
extern ADIEncoder left_encoder;
extern ADIEncoder middle_encoder;
extern ADIEncoder right_encoder;

// Pistons
extern ADIDigitalOut yeet_release;
extern ADIDigitalOut yeet_retract;


#endif




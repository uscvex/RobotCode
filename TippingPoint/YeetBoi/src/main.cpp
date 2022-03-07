#include "main.h"
#include <string>

using namespace pros;
using namespace std;

Controller controller(E_CONTROLLER_MASTER);

RobotParams this_robot;

int which_robot = 0;
int num_robots = 2;
string robot_names[] = {"WHT", "BLK"};

void calibrate_robot_params() {
    switch(which_robot) {
        default:
            which_robot = 0;
        case 0:
            init_robot_white();
            break;
        case 1:
            init_robot_black();
            break;
    }
}


void init_positions() {

    base_lift.move_voltage(-3000);
    spike_arm.move_voltage(-4000);
    spike_wrist.move_voltage(-4000);
    base_release.move_voltage(12000);
    lift_right.move_voltage(-2000);
    lift_left.move_voltage(-2000);
    delay(1000);
    while (controller.get_digital(DIGITAL_UP)) {
        delay(10);
    }
    lift_left.tare_position();
    lift_left.move_voltage(0);
    lift_right.tare_position();
    lift_right.move_voltage(0);
    base_lift.tare_position();
    base_lift.move_voltage(0);
    spike_arm.tare_position();
    spike_arm.move_voltage(0);
    spike_wrist.tare_position();
    spike_wrist.move_voltage(0);
    belly_grab.tare_position();
    base_release.tare_position();
    base_release.move_voltage(0);
}

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
    calibrate_robot_params();
    yeet_release.set_value(0);
    yeet_retract.set_value(0);
    init_tracking();
    init_positions();
    delay(200);
    pros::Task drive_task(run_drive, (void*)"PROS", TASK_PRIORITY_DEFAULT, TASK_STACK_DEPTH_DEFAULT, "Drive Task");
    pros::Task display_task(run_display, (void*)"PROS", TASK_PRIORITY_DEFAULT, TASK_STACK_DEPTH_DEFAULT, "Display Task");
    pros::Task belly_grab_task(run_belly_grab, (void*)"PROS", TASK_PRIORITY_DEFAULT, TASK_STACK_DEPTH_DEFAULT, "Belly Grab Task");
    pros::Task base_grab_task(run_base_lift, (void*)"PROS", TASK_PRIORITY_DEFAULT, TASK_STACK_DEPTH_DEFAULT, "Base Grab Task");
    pros::Task lift_task(run_lift, (void*)"PROS", TASK_PRIORITY_DEFAULT, TASK_STACK_DEPTH_DEFAULT, "Lift Task");
    pros::Task yeet_task(run_yeet, (void*)"PROS", TASK_PRIORITY_DEFAULT, TASK_STACK_DEPTH_DEFAULT, "Yeet Task");
    pros::Task optical_task(set_optical, (void*)"PROS", TASK_PRIORITY_DEFAULT, TASK_STACK_DEPTH_DEFAULT, "Optical Sensor Task");
}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {}

/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
void competition_initialize() {}


void opcontrol() {
    base_lift_state = 1;        // Base lift in ready-to-grab position
    belly_grab_state = 2;       // Bell grab in down state
    yeet_state = 0;             // Retract the piston from yeet
    drive_mode = DM_USER;       // Stop any unfinished drive move

    // If this is a skills run, deploy and put robot into good state
    if ((which_auton == 3) || (which_auton == 4)) {
        yeet_state = 10;            // Yeet release (auto-deploy for driver skills)
        lift_state = -1;            // Don't be scoring rings
        base_right_state = -1;      // Don't be righting bases
        spike_arm_state = 1;        // Do be ready to spike for match-loads
    }

    bool just_toggled_auton = false;
    while (true) {

        // Toggle between robots and autons
        if (controller.get_digital(DIGITAL_UP) && controller.get_digital(DIGITAL_X)) {
            if (!just_toggled_auton) {
                which_auton++;
                if (which_auton >= num_autons) {
                    which_auton = 0;
                    which_robot = (which_robot + 1) % num_robots;
                }
                calibrate_robot_params();
            }
            just_toggled_auton = true;
        }
        else {
            just_toggled_auton = false;
        }

        delay(50);
    }
}

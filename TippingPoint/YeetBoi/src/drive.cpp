#include "main.h"

using namespace pros;

#define SLEW_STEP_FORWARD 5
#define SLEW_STEP_TURN 5

Motor drive_right_1(17, SPEED, 0);
Motor drive_right_2(15, SPEED, 0);
Motor drive_right_3(14, SPEED, 0);

Motor drive_left_1(13, SPEED, 1);
Motor drive_left_2(12, SPEED, 1);
Motor drive_left_3(11, SPEED, 1);

int drive_mode = -1;


void run_drive(void* params) {

    double drive_slew_forward = 0;
    double drive_slew_turn = 0;

    while (true) {

        // Call track_position to update the odometry
        track_position();

        // Get user input for manual control of drive
        double input_forward = controller.get_analog(ANALOG_LEFT_Y);
        double input_turn = controller.get_analog(ANALOG_RIGHT_X);

        if (drive_slew_forward > input_forward + SLEW_STEP_FORWARD) {
            drive_slew_forward -= SLEW_STEP_FORWARD;
        } 
        else if (drive_slew_forward < input_forward - SLEW_STEP_FORWARD) {
            drive_slew_forward += SLEW_STEP_FORWARD;
        }
        else {
            drive_slew_forward = input_forward;
        }

        if (drive_slew_turn > input_turn + SLEW_STEP_TURN) {
            drive_slew_turn -= SLEW_STEP_TURN;
        } 
        else if (drive_slew_turn < input_turn - SLEW_STEP_TURN) {
            drive_slew_turn += SLEW_STEP_TURN;
        }
        else {
            drive_slew_turn = input_turn;
        }
        

        double forward_speed = drive_slew_forward;
        double turn_speed = drive_slew_turn;

        if (controller.get_digital(DIGITAL_UP)) {
            drive_mode = DM_USER;
        }


        if (drive_mode != DM_USER) {
            if (drive_mode == DM_YEET_FORWARD) {
                turn_speed = 0;
                forward_speed = -127;
            }
            else if (drive_mode == DM_YEET_BACKWARD) {
                turn_speed = 0;
                forward_speed = 127;
            }
        }

        // Sent values to drive motors
        drive_right_1.move_voltage(12000 * (forward_speed - turn_speed) / 127);
        drive_right_2.move_voltage(12000 * (forward_speed - turn_speed) / 127);
        drive_right_3.move_voltage(12000 * (forward_speed - turn_speed) / 127);
        drive_left_1.move_voltage(12000 * (forward_speed + turn_speed) / 127);
        drive_left_2.move_voltage(12000 * (forward_speed + turn_speed) / 127);
        drive_left_3.move_voltage(12000 * (forward_speed + turn_speed) / 127);

        delay(10);
    }
}

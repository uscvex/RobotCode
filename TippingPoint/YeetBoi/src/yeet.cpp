#include "main.h"

#define BASE_LIFT_YEET_POS 1100

ADIDigitalOut yeet_release(1, false);
ADIDigitalOut yeet_retract(2, false);

int yeet_state = 0;

void run_yeet(void* params) {

    bool just_toggled = false;

    double yeet_start_time = 0;
    double yeet_start_x = 0;
    double yeet_start_y = 0;
    double yeet_distance = 42;
    double yeet_last_time = millis();

    while (true) {

        bool next_yeet_state = false;
        if (controller.get_digital(DIGITAL_X) && !controller.get_digital(DIGITAL_UP)) {
            if (!just_toggled) {
                next_yeet_state = true;
            }
            just_toggled = true;
        }
        else {
            just_toggled = false;
        }

        switch (yeet_state) {
            case 0:
                yeet_release.set_value(0);
                yeet_retract.set_value(0);
                if (next_yeet_state) {
                    yeet_state = 1;
                }
                break;
            case 1: // Pre start
                yeet_last_time = millis();
                yeet_state = 2;
            case 2:
                belly_grab_target = 0;
                if (millis() - yeet_last_time > 100) {
                    yeet_state = 3;
                }
                break;
            case 3:
                belly_grab_target = -1234;
                break;
            // case 1:  // Pre-yeet
            //     yeet_start_time = millis();
            //     yeet_start_x = robot_x;
            //     yeet_start_y = robot_y;
            //     yeet_state = 2;
            //     yeet_release.set_value(1);
            //     yeet_retract.set_value(0);
            //     break;
            // case 2:
            //     belly_grab_state = -1;
            //     lift_target = 0;
            //     belly_grab_target = -1000;
            //     spike_arm_target = 1600;
            //     base_release_target = 200;
            //     base_lift_target = BASE_LIFT_YEET_POS;
            //     yeet_release.set_value(1);
            //     yeet_retract.set_value(0);
            //     drive_mode = DM_YEET_FORWARD;
            //     if ((next_yeet_state) || (pythag(robot_x, robot_y, yeet_start_x, yeet_start_y) >= yeet_distance)) {
            //         yeet_state = 3;
            //         yeet_last_time = millis();
            //         yeet_start_x = robot_x;
            //         yeet_start_y = robot_y;
            //         drive_mode = DM_USER;
            //     }
            //     break;
            // case 3:
            //     belly_grab_target = 0;
            //     base_lift_target = 3133;
            //     drive_mode = DM_YEET_BACKWARD;
            //     yeet_release.set_value(1);
            //     yeet_retract.set_value(1);
            //     if (next_yeet_state || (millis() - yeet_last_time > 3000) || (pythag(robot_x, robot_y, yeet_start_x, yeet_start_y) >= yeet_distance / 2)) {
            //         yeet_state = 0;
            //         drive_mode = DM_USER;
            //     }
            //     break;
            default:
                yeet_state = 0;
                break;
        }
        
        if (controller.get_digital(DIGITAL_UP)) {
            yeet_state = 0;
            if (controller.get_digital(DIGITAL_X)) {
                yeet_retract.set_value(1);
            }
        }

        delay(10);
    }
}

#include "main.h"


ADIDigitalOut yeet_release(1, false);
ADIDigitalOut yeet_retract(2, false);

int yeet_state = 0;
double yeet_distance = 45;

void run_yeet(void* params) {

    bool just_toggled = false;

    double yeet_start_time = 0;
    double yeet_start_x = 0;
    double yeet_start_y = 0;
    
    double yeet_last_time = millis();

    while (true) {

        bool next_yeet_state = false;
        // Disable manual yeet for now
        // if (controller.get_digital(DIGITAL_X) && !controller.get_digital(DIGITAL_UP)) {
        //     if (!just_toggled) {
        //         next_yeet_state = true;
        //     }
        //     just_toggled = true;
        // }
        // else {
        //     just_toggled = false;
        // }

        switch (yeet_state) {
            case 0:
                yeet_release.set_value(0);
                yeet_retract.set_value(0);
                if (next_yeet_state) {
                    yeet_state = 1;
                }
                break;
            case 1:  // Pre-yeet -- record current time & position
                yeet_start_time = millis();
                yeet_start_x = robot_x;
                yeet_start_y = robot_y;
                yeet_state = 2;
                // No break, just go to next state now
            case 2:
                cout << (millis() - yeet_start_time) << "\t" << pythag(robot_x, robot_y, yeet_start_x, yeet_start_y) << endl;
                yeet_release.set_value(0);
                yeet_retract.set_value(0);
                spike_arm_target = -2000;
                lift_target = 0;
                base_lift_target = this_robot.BASE_LIFT_YEET_POS;
                base_release_target = this_robot.BASE_RELEASE_READY_POS;
                spike_wrist_target = 65.0;
                if ((millis() - yeet_start_time) > (0.0)) {
                    yeet_state = 3;
                }
                break;
            case 3:
                cout << (millis() - yeet_start_time) << "\t" << pythag(robot_x, robot_y, yeet_start_x, yeet_start_y) << endl;
                yeet_release.set_value(1);
                yeet_retract.set_value(0);
                spike_arm_target = -2000;
                lift_target = 0;
                base_lift_target = this_robot.BASE_LIFT_YEET_POS;
                base_release_target = this_robot.BASE_RELEASE_READY_POS;
                spike_wrist_target = 65.0;
                if (yeet_distance <= pythag(robot_x, robot_y, yeet_start_x, yeet_start_y)) {
                    yeet_state = 4;
                }
                else if ((millis() - yeet_start_time) > 1500) {
                    yeet_state = 4;
                }
                else {
                    break;
                }
            case 4:
                spike_arm_target = 0;
                spike_wrist_target = this_robot.SPIKE_WRIST_STORE_POS;
                lift_target = 0;
                yeet_release.set_value(1);
                yeet_retract.set_value(1);
                break;

            case 10:    // DEPLOY
                yeet_release.set_value(1);
                yeet_retract.set_value(1);
                break;
            case 11:    // CHILL
                yeet_release.set_value(0);
                yeet_retract.set_value(1);
                break;

            default:
                yeet_state = 0;
                break;
        }
        
        // Retract piston on 'special'
        if (controller.get_digital(DIGITAL_LEFT)) {
            yeet_state = 0;
        }

        if (controller.get_digital(DIGITAL_UP)) {
            yeet_state = 0;
            if (controller.get_digital(DIGITAL_DOWN)) {
                yeet_retract.set_value(1);
                yeet_release.set_value(1);
            }
        }

        delay(10);
    }
}

#include "main.h"

Motor belly_grab(18, SPEED, 1);

double belly_grab_pos = 0;
double belly_grab_target = 0;
int belly_grab_state = 0;

void run_belly_grab(void* params) {

    bool just_toggled = false;

    while (true) {

        double belly_grab_speed = 0;

        belly_grab_pos = belly_grab.get_position();

        bool next_state = false;

        if (controller.get_digital(DIGITAL_R2)) {
            if (!just_toggled) {
                next_state = true;
            }
            just_toggled = true;
        }
        else {
            just_toggled = false;
        }
        
        switch (belly_grab_state) {
            case -1:
                if (next_state)
                    belly_grab_state = 1;
                break;
            case 1:
                belly_grab_target = -1;
                if (next_state)
                    belly_grab_state = 2;
                break;
            case 2:
                belly_grab_target = this_robot.BELLY_GRAB_DOWN;
                if (next_state) 
                    belly_grab_state = 3;
                break;
            case 3:
                belly_grab_target = this_robot.BELLY_GRAB_UP;
                if (next_state) 
                    belly_grab_state = 1;
                break;
            default:
                belly_grab_state = -1;
        }

        // P-controller
        if (belly_grab_target != -1) {
            if (belly_grab_target >= belly_grab_pos) {
                belly_grab_speed = (belly_grab_target - belly_grab_pos) * this_robot.BELLY_GRAB_RATE_UP;
            }
            else {
                belly_grab_speed = (belly_grab_target - belly_grab_pos) * this_robot.BELLY_GRAB_RATE_DOWN;
            }
        }
        if (belly_grab_target == -1234) {
            belly_grab_speed = -30;
        }

        // Abort button
        if (controller.get_digital(DIGITAL_UP)) {
            belly_grab_target = -1;
            belly_grab_speed = 0;
            belly_grab_state = -1;
        }

        // if (controller.get_digital(DIGITAL_UP)) {
        //     if (controller.get_digital(DIGITAL_DOWN)) {
        //         belly_grab_speed = -40;
        //     }
        // }

        belly_grab.move_voltage((12000 * belly_grab_speed) / 127);

        delay(20);
    }
}
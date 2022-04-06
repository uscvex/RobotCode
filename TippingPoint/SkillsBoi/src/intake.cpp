#include "main.h"

Motor intake_left(14, SPEED, 1);
Motor intake_right(15, SPEED, 0);
Motor intake_wobble_left(8, SPEED, 1);
Motor intake_wobble_right(18, SPEED, 0);

bool intake = false;
bool intake_wobble = false;

void run_intake(void* params) {

    bool just_toggled_intake = false;
    bool just_toggled_intake_wobble = false;
    double last_wobble_time = millis();
    double intake_stuck_start = millis();
    double intake_backwards_until = 0;
    
    while (true) {
        double intake_speed = 0;
        double intake_wobble_speed_left = 0;
        double intake_wobble_speed_right = 0;
        double intake_current_speed = (intake_left.get_actual_velocity() + intake_right.get_actual_velocity()) / 2.0;

        if (controller.get_digital(DIGITAL_LEFT)) {
            if (!just_toggled_intake) {
                intake = !intake;
                intake_wobble = intake;
            }
            just_toggled_intake = true;
        }
        else {
            just_toggled_intake = false;
        }

        if (controller.get_digital(DIGITAL_RIGHT)) {
            if (!just_toggled_intake_wobble) {
                intake_wobble = !intake_wobble;
            }
            just_toggled_intake_wobble = true;
        }
        else {
            just_toggled_intake_wobble = false;
        }

        if (controller.get_digital(DIGITAL_DOWN)) {
            intake_speed = -127;
            intake = false;
        }

        if (intake) {
            intake_speed = 127;
            if (millis() < intake_backwards_until) {
                intake_speed = -127;
                intake_stuck_start = millis();
            }
            else {
                if (intake_current_speed < this_robot.INTAKE_VOMIT_THRESHOLD) {
                    if (millis() - intake_stuck_start > this_robot.INTAKE_VOMIT_TIMEOUT) {
                        intake_backwards_until = millis() + this_robot.INTAKE_VOMIT_TIME;
                    }
                }
                else {
                    intake_stuck_start = millis();
                }
            }
        }
        else {
            intake_stuck_start = millis();
            intake_backwards_until = 0;
        }

        if (intake_wobble) {
            intake_wobble_speed_right = this_robot.INTAKE_WOBBLE_OUT;
            if ((millis() % this_robot.INTAKE_WOBBLE_PERIOD_RIGHT) <= (this_robot.INTAKE_WOBBLE_PERIOD_RIGHT / 2)) {
                intake_wobble_speed_right = this_robot.INTAKE_WOBBLE_IN;
            }

            intake_wobble_speed_left = this_robot.INTAKE_WOBBLE_OUT;
            if ((millis() % this_robot.INTAKE_WOBBLE_PERIOD_LEFT) <= (this_robot.INTAKE_WOBBLE_PERIOD_LEFT / 2)) {
                intake_wobble_speed_left = this_robot.INTAKE_WOBBLE_IN;
            }
            last_wobble_time = millis();
        }
        else {
            if ((millis() - last_wobble_time) < 750) {
                intake_wobble_speed_right = this_robot.INTAKE_WOBBLE_OUT / 2;
                intake_wobble_speed_left = this_robot.INTAKE_WOBBLE_OUT / 2;
            }
        }

        // Abort button
        if (controller.get_digital(DIGITAL_UP)) {
            intake = false;
            intake_wobble = false;
        }

        intake_wobble_left.move_voltage((12000 * intake_wobble_speed_left) / 127);
        intake_wobble_right.move_voltage((12000 * intake_wobble_speed_right) / 127);

        intake_left.move_voltage((12000 * intake_speed) / 127);
        intake_right.move_voltage((12000 * intake_speed) / 127);

        pros::delay(10);
    }
}

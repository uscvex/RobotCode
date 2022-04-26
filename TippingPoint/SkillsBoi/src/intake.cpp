#include "main.h"

Motor intake_left(14, SPEED, 1);
Motor intake_right(15, SPEED, 0);
Motor intake_wobble_left(8, SPEED, 1);
Motor intake_wobble_right(17, SPEED, 0);

ADIDigitalOut back_tip(2, false);

bool intake = false;
bool intake_wobble = false;
bool tip_latch = false;

void run_intake(void* params) {

    bool just_toggled_intake = false;
    bool just_toggled_intake_wobble = false;
    double last_wobble_time = millis();
    double intake_stuck_start = millis();
    double intake_backwards_until = 0;
    bool just_toggled_tip = false;
    double intake_start_time = -1;
    
    while (true) {
        double intake_speed = 0;
        double intake_wobble_speed_left = 0;
        double intake_wobble_speed_right = 0;
        double intake_current_speed = (intake_left.get_actual_velocity() + intake_right.get_actual_velocity()) / 2.0;

        if (controller.get_digital(DIGITAL_B)) {
            if (!just_toggled_tip) {
                tip_latch = !tip_latch;
                if (tip_latch) {
                    intake_start_time = millis() + 1500;
                }
                else {
                    intake = false;;
                    intake_wobble = false;
                    intake_start_time = -1;
                }
            }
            just_toggled_tip = true;
        }
        else {
            just_toggled_tip = false;
        }
        back_tip.set_value(tip_latch);

        if (intake_start_time != -1) {
            if (millis() > intake_start_time) {
                if (tip_latch) {
                    intake = true;
                    intake_wobble = true;
                }
                intake_start_time = -1;
            }
        }

        if (controller.get_digital(DIGITAL_LEFT)) {
            if (!just_toggled_intake) {
                intake = !intake;
                // if (!intake)
                intake_wobble = intake;
                if (intake)
                    limit_current = false;
            }
            just_toggled_intake = true;
        }
        else {
            just_toggled_intake = false;
        }

        if (controller.get_digital(DIGITAL_RIGHT)) {
            if (!just_toggled_intake_wobble) {
                intake_wobble = !intake_wobble;
                if (intake_wobble)
                    limit_current = false;
            }
            just_toggled_intake_wobble = true;
        }
        else {
            just_toggled_intake_wobble = false;
        }

        if (controller.get_digital(DIGITAL_DOWN)) {
            limit_current = false;
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
            limit_current = false;
            intake_start_time = -1;
        }

        intake_wobble_left.move_voltage((12000 * intake_wobble_speed_left) / 127);
        intake_wobble_right.move_voltage((12000 * intake_wobble_speed_right) / 127);

        intake_left.move_voltage((12000 * intake_speed) / 127);
        intake_right.move_voltage((12000 * intake_speed) / 127);

        pros::delay(10);
    }
}

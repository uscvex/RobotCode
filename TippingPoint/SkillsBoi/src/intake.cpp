#include "main.h"

Motor intake_left(14, SPEED, 1);
Motor intake_right(15, SPEED, 0);

bool intake = false;

void run_intake(void* params) {

    bool just_toggled_intake = false;
    
    while (true) {
        double intake_speed = 0;

        if (controller.get_digital(DIGITAL_LEFT)) {
            if (!just_toggled_intake) {
                intake = !intake;
            }
            just_toggled_intake = true;
        }
        else {
            just_toggled_intake = false;
        }
        if (controller.get_digital(DIGITAL_DOWN)) {
            intake_speed = -127;
            intake = false;
        }

        if (intake) {
            intake_speed = 127;
        }

        // Abort button
        if (controller.get_digital(DIGITAL_UP)) {
            intake = false;
        }

        intake_left.move_voltage((12000 * intake_speed) / 127);
        intake_right.move_voltage((12000 * intake_speed) / 127);

        pros::delay(20);
    }
}

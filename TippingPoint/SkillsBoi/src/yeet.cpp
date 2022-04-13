#include "main.h"


ADIDigitalOut yeet_release(8, false);
ADIDigitalOut yeet_retract(1, false);

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
                back_lift_state = -1;
                back_lift_target = -2000;
                tip_latch = true;
                cout << (millis() - yeet_start_time) << "\t" << pythag(robot_x, robot_y, yeet_start_x, yeet_start_y) << endl;
                yeet_release.set_value(1);
                yeet_retract.set_value(0);
                if (yeet_distance <= pythag(robot_x, robot_y, yeet_start_x, yeet_start_y)) {
                    yeet_state = 3;
                }
                else if ((millis() - yeet_start_time) > 2000) {
                    yeet_state = 3;
                }
                else {
                    break;
                }
            case 3:
                back_lift_state = READY;
                tip_latch = false;
                yeet_release.set_value(1);
                yeet_retract.set_value(1);
                yeet_state = 10;
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

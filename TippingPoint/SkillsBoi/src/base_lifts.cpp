#include "main.h"

Motor back_lift_right(9, SPEED, 1);
Motor back_lift_left(10, SPEED, 0);

Motor front_lift_right(19, SPEED, 1);
Motor front_lift_left(20, SPEED, 0);

Motor side_lift(17, SPEED, 0);

ADIDigitalOut front_latch(4, false);
ADIDigitalOut back_latch(3, false);
ADIDigitalOut back_tip(2, false);

int back_lift_state = -1;
double back_lift_pos = 0;
double back_lift_target = -1;

int front_lift_state = -1;
double front_lift_pos = 0;
double front_lift_target = -1;

double side_lift_pos = 0;
double side_lift_target = -1;

bool tip_latch = false;
bool wobble_front = false;

void run_base_lifts(void* params) {

    bool just_toggled_back = false;
    bool just_toggled_front = false;
    bool just_toggled_tip = false;
    bool just_toggled_back_wobble = false;
    bool just_toggled_side = false;
    
    while (true) {
        bool next_state_back = false;
        bool next_state_front = false;

        double back_lift_speed = 0;
        double front_lift_speed = 0;
        double side_lift_speed = 0;

        back_lift_pos = (back_lift_left.get_position() + back_lift_right.get_position()) / 2.0;
        front_lift_pos = (front_lift_left.get_position() + front_lift_right.get_position()) / 2.0;

        side_lift_pos = side_lift.get_position();

        if (controller.get_digital(DIGITAL_A)) {
            front_lift_state = 5;
        }

        if (controller.get_digital(DIGITAL_Y)) {
            limit_current = false;
            if (!just_toggled_side) {
                if (side_lift_target != this_robot.SIDE_LIFT_HOLD_POS) {
                    side_lift_target = this_robot.SIDE_LIFT_HOLD_POS;
                }
                else {
                    side_lift_target = this_robot.SIDE_LIFT_READY_POS;
                }
            }
            just_toggled_side = true;
        }
        else {
            just_toggled_side = false;
        }

        if (controller.get_digital(DIGITAL_B)) {
            if (!just_toggled_tip) {
                tip_latch = !tip_latch;
                intake = tip_latch;
                intake_wobble = tip_latch;
            }
            just_toggled_tip = true;
        }
        else {
            just_toggled_tip = false;
        }
        back_tip.set_value(tip_latch);

        if (controller.get_digital(DIGITAL_R1)) {
            limit_current = false;
            if (!just_toggled_back) {
                next_state_back = true;
            }
            just_toggled_back = true;
        }
        else {
            just_toggled_back = false;
        }

        if (controller.get_digital(DIGITAL_R2)) {
            back_latch.set_value(1);
        }
        else {
            back_latch.set_value(0);
        }

        if (controller.get_digital(DIGITAL_L1)) {
            limit_current = false;
            if (!just_toggled_front) {
                next_state_front = true;
            }
            just_toggled_front = true;
        }
        else {
            just_toggled_front = false;
        }

        if (controller.get_digital(DIGITAL_L2)) {
            front_latch.set_value(1);
        }
        else {
            front_latch.set_value(0);
        }

        switch (back_lift_state) {
            case -1:  // Do nothing (abort state)
                if (next_state_back)
                    back_lift_state = 1;
                break;
            case 1:  // Ready to grab state
                back_lift_target = this_robot.BACK_LIFT_READY_POS;
                if (controller.get_digital(DIGITAL_R2)) {
                    back_lift_target -= this_robot.BACK_LIFT_HEIGHT_DROP;
                }
                
                if (next_state_back)
                    back_lift_state = 2;
                break;
            case 2:  // Hold base state
                back_lift_target = this_robot.BACK_LIFT_HOLD_POS;
                if (next_state_back)
                    back_lift_state = 1;
                break;

            default:
                back_lift_state = -1;
        }

        switch (front_lift_state) {
            case -1:  // Do nothing (abort state)
                if (next_state_front)
                    front_lift_state = 1;
                break;
            case 1:  // Ready to grab state
                front_lift_target = this_robot.FRONT_LIFT_READY_POS;
                if (controller.get_digital(DIGITAL_L2)) {
                    front_lift_target -= this_robot.FRONT_LIFT_HEIGHT_DROP;
                }
                
                if (next_state_front)
                    front_lift_state = 2;
                break;
            case 2:  // Hold base state
                front_lift_target = this_robot.FRONT_LIFT_HOLD_POS;
                if (wobble_front) {
                    front_lift_target += this_robot.FRONT_WOBBLE_AMP * (sin(millis() / 100.0) - 1);
                }
                if (next_state_front)
                    front_lift_state = 1;
                break;

            case 5:  // Park state
                front_lift_target = this_robot.FRONT_LIFT_PARK_POS;
                if (next_state_front)
                    front_lift_state = 2;
                break;

            default:
                front_lift_state = -1;
        }

        // P-controller
        if (back_lift_target != -1) {
            back_lift_speed = (back_lift_target - back_lift_pos) * this_robot.BACK_LIFT_RATE;
        }
        if (front_lift_target != -1) {
            front_lift_speed = (front_lift_target - front_lift_pos) * this_robot.FRONT_LIFT_RATE;
        }
        if (side_lift_target != -1) {
            side_lift_speed = (side_lift_target - side_lift_pos) * this_robot.SIDE_LIFT_RATE;
        }

        // Abort button
        if (controller.get_digital(DIGITAL_UP)) {
            limit_current = false;
            back_lift_target = -1;
            front_lift_target = -1;
            back_lift_state = -1;
            front_lift_state = -1;
            back_latch.set_value(0);
            front_latch.set_value(0);
            tip_latch = false;
            back_tip.set_value(0);
            wobble_front = false;
            side_lift_target = -1;
        }

        side_lift.move_voltage((12000 * side_lift_speed) / 127);

        back_lift_right.move_voltage((12000 * back_lift_speed) / 127);
        back_lift_left.move_voltage((12000 * back_lift_speed) / 127);

        front_lift_right.move_voltage((12000 * front_lift_speed) / 127);
        front_lift_left.move_voltage((12000 * front_lift_speed) / 127);

        pros::delay(20);
    }
}

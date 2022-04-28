#include "main.h"
#include <math.h>
#include <iostream>


Motor spike_wrist(7, SPEED, 1);
Motor spike_arm(9, SPEED, 1);
Motor spike(3, SPEED, 0);

Motor lift_left(5, SPEED, 1);
Motor lift_right(4, SPEED, 0);

double spike_arm_pos = 0;
double spike_arm_target = -1;

double spike_wrist_pos = 0;
double spike_wrist_target = -1;

double spike_speed = 0;

double lift_pos = 0;
double lift_target = -1;

bool spike_drop = false;

int spike_arm_state = -1;
int lift_state = -1;
int base_right_state = -1;

void run_lift(void* params) {


    bool just_manual_dropped = false;
    bool just_toggled_collect = false;
    bool just_toggled_drop = false;
    bool just_toggled_right = false;
    bool just_toggled_lift = false;
    double just_dropped_time = millis();
    double last_state_time = millis();

    int collision_avoid_state = -1;
    double initial_wrist_pos = 0;

    bool just_toggled_spike_adj_up = false;
    bool just_toggled_spike_adj_down = false;


    while (true) {

        double spike_arm_speed = 0;
        double spike_wrist_speed = 0;
        double lift_speed = 0;
        double spike_wrist_target_offset = 0;

        spike_arm_pos = spike_arm.get_position();
        spike_wrist_pos = 10 + ((spike_wrist.get_position() * 2 / 5) + (spike_arm_pos / 7)) / 5;

        double lift_pos_right = lift_right.get_position();
        double lift_pos_left = lift_left.get_position();
        lift_pos = (lift_pos_right + lift_pos_left) / 2;

        bool next_right_state = false;
        if ((lift_state != 1) && (lift_state != 2) && (lift_state != 3)) {
            if (controller.get_digital(DIGITAL_B)) {
                lift_state = -1;
                spike_arm_state = -1;
                base_lift_state = 1;    // put base lift in ready-to-grab
                if (!just_toggled_right) {
                    next_right_state = true;
                }
                just_toggled_right = true;
            }
            else {
                just_toggled_right = false;
            }
        }

        bool next_collect_state = false;
        if (controller.get_digital(DIGITAL_L2)) {
        
            
            lift_state = -1;
            base_right_state = -1;
            if (!just_toggled_collect) {
                next_collect_state = true;
            }
            just_toggled_collect = true;
        }
        else {
            just_toggled_collect = false;
        }


        bool next_lift_state = false;
        if (controller.get_digital(DIGITAL_L1)) {
            spike_arm_state = -1;
            base_right_state = -1;
            if (!just_toggled_lift) {
                next_lift_state = true;
            }
            just_toggled_lift = true;
        }
        else {
            just_toggled_lift = false;
        }

        switch (base_right_state) {
            case -1:
                if (next_right_state) {
                    base_right_state = 1;
                }
                break;
            case 1:
                lift_target = 0;
                spike_arm_target = this_robot.BASE_RIGHT_ARM_POS_1;
                spike_wrist_target = this_robot.BASE_RIGHT_WRIST_POS_1;
                if (next_right_state) {
                    base_right_state = 2;
                }
                break;
            case 2:
                lift_target = 0;
                // spike_arm_target = this_robot.BASE_RIGHT_ARM_POS_2;
                // spike_wrist_target = this_robot.BASE_RIGHT_WRIST_POS_2;
                spike_arm_target = this_robot.SPIKE_ARM_READY_POS;
                spike_wrist_target = this_robot.SPIKE_WRIST_READY_POS;
                if (next_right_state) {
                    base_right_state = 3;
                }
                break;
            case 3:
                lift_target = 0;
                spike_arm_target = this_robot.BASE_RIGHT_ARM_POS_3;
                spike_wrist_target = this_robot.BASE_RIGHT_WRIST_POS_3;
                if (next_right_state) {
                    base_right_state = 1;
                }
                break;
            default:
                base_right_state = -1;
                break;
        }

        double spike_adjust = 0;
        if ((lift_state == 1) || (lift_state == 2) || (lift_state == 3)) {
            if (controller.get_digital(DIGITAL_B)) {
                if (!just_toggled_spike_adj_up) {
                    spike_adjust = -0.6;
                }
                just_toggled_spike_adj_down = true;
            }
            else {
                just_toggled_spike_adj_down = false;
            }
            if (controller.get_digital(DIGITAL_X)) {
                if (!just_toggled_spike_adj_up) {
                    spike_adjust = 1;
                }
                just_toggled_spike_adj_up = true;
            }
            else {
                just_toggled_spike_adj_up = false;
            }
        }

        switch (lift_state) {
            case -1:
                if (next_lift_state) {
                    lift_state = 1;
                }
                break;
            case 1:  // Move arm to score on forward alliance goals
                spike_wrist_target = this_robot.SPIKE_WRIST_ALLIANCE_GOAL_POS;
                spike_arm_target = this_robot.SPIKE_ARM_ALLIANCE_GOAL_POS;
                lift_target = this_robot.LIFT_ALLIANCE_GOAL_POS;
                if (spike_adjust != 0) {
                    this_robot.SPIKE_WRIST_ALLIANCE_GOAL_POS += spike_adjust * this_robot.SPIKE_WRIST_ADJUST_AMOUNT;
                }
                if (next_lift_state) {
                    lift_state = 2;
                }
                break;
            case 2:  // Move arm to score on low neutral goals
                spike_wrist_target = this_robot.SPIKE_WRIST_LOW_GOAL_POS;
                if ( (lift_pos < 0.8 * lift_target) || (spike_arm_pos < 0.8 * spike_arm_target) ) {
                    spike_wrist_target = this_robot.SPIKE_WRIST_CHASSIS_CLEAR;
                }
                if (spike_adjust != 0) {
                    this_robot.SPIKE_ARM_LOW_GOAL_POS += spike_adjust * this_robot.SPIKE_ARM_ADJUST_AMOUNT;
                }
                spike_arm_target = this_robot.SPIKE_ARM_LOW_GOAL_POS;
                lift_target = this_robot.LIFT_LOW_GOAL_POS;
                if (next_lift_state) {
                    lift_state = 3;
                }
                break;
            case 3:  // Move arm to score on high neutral goals
                spike_wrist_target = this_robot.SPIKE_WRIST_HIGH_GOAL_POS;
                if ( (lift_pos < 0.8 * lift_target) || (spike_arm_pos < 0.8 * spike_arm_target) ) {
                    // spike_wrist_target = this_robot.SPIKE_WRIST_CHASSIS_CLEAR;
                }
                if (spike_adjust != 0) {
                    this_robot.SPIKE_ARM_HIGH_GOAL_POS += spike_adjust * this_robot.SPIKE_ARM_ADJUST_AMOUNT;
                }
                spike_arm_target = this_robot.SPIKE_ARM_HIGH_GOAL_POS;
                lift_target = this_robot.LIFT_HIGH_GOAL_POS;
                if (next_lift_state) {
                    lift_state = 1;
                }
                break;
            default:
                lift_state = -1;
                break;
        }

        switch (spike_arm_state) {
            case -1:
                if (next_collect_state) {
                    spike_arm_state = 1;
                }
                break;
            case 1:  // Ready to pick up
                lift_target = 0;
                spike_arm_target = this_robot.SPIKE_ARM_READY_POS;
                spike_wrist_target = this_robot.SPIKE_WRIST_READY_POS;
                if (next_collect_state) {
                    spike_arm_state = 2;
                    last_state_time = millis();
                }
                break;
            case 10:
                last_state_time = millis();
                spike_arm_state = 2;
                break;

            case 2:  // Stamp down hard for a time
                lift_target = 0;
                spike_wrist_target = this_robot.SPIKE_WRIST_GRAB_POS;
                spike_arm_speed = -127;
                spike_arm_target = -1;
                if ((millis() - last_state_time > this_robot.SPIKE_STAMP_TIME) && (!just_toggled_collect)) {
                    spike_arm_state = 1;
                }
                if (next_collect_state) {
                    spike_arm_state = 1;
                }
                break;
            case 4:  // Hold down to pick up
                lift_target = 0;
                spike_arm_target = this_robot.SPIKE_ARM_GRAB_POS;
                spike_wrist_target = this_robot.SPIKE_WRIST_GRAB_POS;
                if (next_collect_state) {
                    spike_arm_state = 1;
                }
                break;
            default:
                spike_arm_state = -1;
                break;
        }

        if (controller.get_digital(DIGITAL_LEFT)) {
            spike_arm_state = -1;
            base_right_state = -1;
            lift_state = -1;
            if (spike_wrist_target != this_robot.SPIKE_WRIST_STORE_POS) {
                collision_avoid_state = -1;
            }
            spike_wrist_target = this_robot.SPIKE_WRIST_STORE_POS;
            spike_arm_target = this_robot.SPIKE_ARM_STORE_POS;
            lift_target = 0;
        }
        if (controller.get_digital(DIGITAL_RIGHT)) {
            spike_arm_state = -1;
            base_right_state = -1;
            lift_state = -1;
            if (spike_wrist_target != this_robot.ALLIANCE_HELD_WRIST_POS) {
                collision_avoid_state = -1;
            }
            spike_wrist_target = this_robot.ALLIANCE_HELD_WRIST_POS;
            spike_arm_target = this_robot.ALLIANCE_HELD_ARM_POS;
            lift_target = 0;
        }


        // Lock bellygrab
        if ((lift_state != 1) && (lift_state != 2) && (lift_state != 3)) {
            if (controller.get_digital(DIGITAL_X) && !controller.get_digital(DIGITAL_UP)) {
                spike_arm_state = -1;
                base_right_state = -1;
                lift_state = -1;
                spike_wrist_target = this_robot.SPIKE_WRIST_LOCK_BELLY;
                spike_arm_target = this_robot.SPIKE_ARM_LOCK_BELLY;;
                lift_target = -50;
                belly_grab_state = 1;
            }
        }


        if (!controller.get_digital(DIGITAL_UP) && controller.get_digital(DIGITAL_DOWN)) {
            spike_drop = true;
            just_manual_dropped = true;
        }
        else {
            if (just_manual_dropped) {
                spike_drop = false;
            }
            just_manual_dropped = false;
        }
        if (spike_drop) {
            if (spike_wrist_target != -1) {
                double skills_amplitude = 1.0;
                // if (which_auton == 4) {
                //     skills_amplitude = 1.35;
                // }
                spike_wrist_target_offset = skills_amplitude * this_robot.SPIKE_SHAKE_AMPLITUDE * (sin(millis() / 15) + sin(millis() / 18));
            }
            spike_speed = this_robot.SPIKE_DROP_SPEED;
            just_dropped_time = millis();
            just_toggled_drop = true;
        }
        else {
            if (just_toggled_drop) {
                if (millis() - just_dropped_time < 500) {
                    spike_speed = this_robot.SPIKE_RETURN_SPEED;
                }
                else {
                    just_toggled_drop = false;
                    spike_speed = 0;
                }
            }
        }

        // Prevent spike from crashing into chassis
        double new_arm_target = spike_arm_target;
        double new_wrist_target = spike_wrist_target;

        if (lift_pos < 800) {
            if (spike_wrist_target != -1) {
                if ((spike_arm_pos <= this_robot.SPIKE_ARM_CHASSIS_CLEAR) && (spike_wrist_pos <= this_robot.SPIKE_WRIST_CHASSIS_CLEAR) && (spike_wrist_target >= this_robot.SPIKE_WRIST_CHASSIS_COLLIDE) && collision_avoid_state == -1) {
                    collision_avoid_state = 1;
                    initial_wrist_pos = spike_wrist_pos;
                }
                if ((spike_arm_pos <= this_robot.SPIKE_ARM_CHASSIS_CLEAR) && (spike_wrist_pos >= this_robot.SPIKE_WRIST_CHASSIS_COLLIDE) && (spike_wrist_target <= this_robot.SPIKE_WRIST_CHASSIS_CLEAR) && collision_avoid_state == -1) {
                    collision_avoid_state = 9;
                    initial_wrist_pos = spike_wrist_pos;
                }
            }
        }

        if (lift_pos > 800) {
            collision_avoid_state = -1;
        }

        switch (collision_avoid_state) {
            case -1:    // Default chill state
                break;

            case 1:     // Move arm up while holding wrist still
                new_wrist_target = initial_wrist_pos;
                new_arm_target = this_robot.SPIKE_ARM_CHASSIS_CLEAR + 200;
                if (spike_arm_pos > this_robot.SPIKE_ARM_CHASSIS_CLEAR) {
                    
                    // If arm will keep moving up, we're done
                    if (spike_arm_target >= this_robot.SPIKE_ARM_CHASSIS_CLEAR + 200) {
                        collision_avoid_state = -1;
                    }
                    else {
                        collision_avoid_state = 2;
                    }
                }
                break;
            case 2:     // Hold arm and allow wrist to move
                new_arm_target = this_robot.SPIKE_ARM_CHASSIS_CLEAR + 200;
                if (spike_wrist_pos >= this_robot.SPIKE_WRIST_CHASSIS_CLEAR) {
                    collision_avoid_state = -1;
                }
                break;

            case 9:     // Move arm up while holding wrist still
                new_wrist_target = this_robot.SPIKE_WRIST_CHASSIS_CLEAR + 35;
                new_arm_target = this_robot.SPIKE_ARM_CHASSIS_CLEAR + 200;
                if (spike_arm_pos > this_robot.SPIKE_ARM_CHASSIS_CLEAR) {
                    // If arm will keep moving up, we're done
                    if (spike_arm_target >= this_robot.SPIKE_ARM_CHASSIS_CLEAR + 200) {
                        collision_avoid_state = -1;
                    }
                    else {
                        collision_avoid_state = 10;
                    }
                }
                break;
            case 10:     // Hold arm and allow wrist to move
                new_arm_target = this_robot.SPIKE_ARM_CHASSIS_CLEAR + 200;
                if (spike_wrist_pos <= this_robot.SPIKE_WRIST_CHASSIS_COLLIDE) {
                    collision_avoid_state = -1;
                }
                break;

            default:
                collision_avoid_state = -1;
                break;
        }

        if (new_arm_target != -1) {
            double spike_wrist_get_out_way_spin = 0;
            // If high scoring
            if ((lift_state == 3) || (lift_state == 2)) {
                // And spinning

                if (controller.get_digital(DIGITAL_A) || controller.get_digital(DIGITAL_Y) || (optical_state != DO_NOTHING)) {
                    spike_wrist_get_out_way_spin = -300;      // Spike move for goal spin
                }
            }
            spike_arm_speed = (spike_wrist_get_out_way_spin + new_arm_target - spike_arm_pos) * this_robot.SPIKE_ARM_RATE;
        }
        if (new_wrist_target != -1) {
            spike_wrist_speed = (new_wrist_target + spike_wrist_target_offset - spike_wrist_pos) * this_robot.SPIKE_WRIST_RATE;
        }


        double lift_speed_left = lift_speed;
        double lift_speed_right = lift_speed;
        if (lift_target != -1) {
            lift_speed_left = (lift_target - lift_pos_left) * this_robot.LIFT_RATE;
            // Try moving right lift to be the same as left is
            // lift_speed_right = (lift_pos_left - lift_pos_right) * LIFT_RATE;
            lift_speed_right = (lift_target - lift_pos_right + this_robot.LEFT_LIFT_OFFSET) * this_robot.LIFT_RATE;
        }

        if (controller.get_digital(DIGITAL_UP)) {
            spike_drop = false;
            base_right_state = -1;
            spike_arm_target = -1;
            lift_target = -1;
            spike_wrist_target = -1;
            spike_arm_state = -1;
            lift_state = -1;
            spike_speed = 0;
            collision_avoid_state = -1;
        }

        spike.move_voltage((12000 * spike_speed) / 127);
        spike_arm.move_voltage((12000 * spike_arm_speed) / 127);
        spike_wrist.move_voltage((12000 * spike_wrist_speed) / 127);

        lift_right.move_voltage((12000 * lift_speed_right) / 127);
        lift_left.move_voltage((12000 * lift_speed_left) / 127);
        
        delay(20);
    }
}
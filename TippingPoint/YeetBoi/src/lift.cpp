#include "main.h"
#include <math.h>
#include <iostream>

#define SPIKE_ARM_RATE 1.5
#define SPIKE_WRIST_RATE 10

#define SPIKE_WRIST_GRAB_POS 208
#define SPIKE_WRIST_READY_POS 215
#define SPIKE_ARM_READY_POS 1900
#define SPIKE_ARM_GRAB_POS 600
#define SPIKE_WRIST_RANGE 2740

#define SPIKE_ARM_CHASSIS_CLEAR 2700
#define SPIKE_WRIST_CHASSIS_CLEAR 200
#define SPIKE_WRIST_CHASSIS_COLLIDE 130

#define SPIKE_WRIST_STORE_POS 20
#define SPIKE_ARM_STORE_POS 600

#define SPIKE_DROP_SPEED 90
#define SPIKE_RETURN_SPEED -60

#define LIFT_RATE 1


#define SPIKE_WRIST_ALLIANCE_GOAL_POS 250
#define SPIKE_ARM_ALLIANCE_GOAL_POS 2860
#define LIFT_ALLIANCE_GOAL_POS 0

#define SPIKE_WRIST_LOW_GOAL_POS 136
#define SPIKE_ARM_LOW_GOAL_POS 3200
#define LIFT_LOW_GOAL_POS 1000

#define SPIKE_WRIST_HIGH_GOAL_POS 134
#define SPIKE_ARM_HIGH_GOAL_POS 4290
#define LIFT_HIGH_GOAL_POS 3700


Motor spike_wrist(7, SPEED, 1);
Motor spike_arm(8, SPEED, 1);
Motor spike(9, SPEED, 0);

Motor lift_left(5, SPEED, 1);
Motor lift_right(4, SPEED, 0);

double spike_arm_pos = 0;
double spike_arm_target = -1;

double spike_wrist_pos = 0;
double spike_wrist_target = -1;

double spike_speed = 0;

double lift_pos = 0;
double lift_target = -1;


void run_lift(void* params) {

    int spike_arm_state = -1;
    int lift_state = -1;

    bool just_toggled_collect = false;
    bool just_toggled_drop = false;
    bool just_toggled_lift = false;
    double just_dropped_time = millis();
    double last_state_time = millis();

    int collision_avoid_state = -1;
    double initial_wrist_pos = 0;

    while (true) {

        double spike_arm_speed = 0;
        double spike_wrist_speed = 0;
        double lift_speed = 0;

        spike_arm_pos = spike_arm.get_position();
        spike_wrist_pos = 10 + ((spike_wrist.get_position() * 2 / 5) + (spike_arm_pos / 7)) / 5;

        double lift_pos_right = lift_right.get_position();
        double lift_pos_left = lift_left.get_position();
        lift_pos = (lift_pos_right + lift_pos_left) / 2;

        bool next_collect_state = false;
        if (controller.get_digital(DIGITAL_L2)) {
            lift_state = -1;
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
            if (!just_toggled_lift) {
                next_lift_state = true;
            }
            just_toggled_lift = true;
        }
        else {
            just_toggled_lift = false;
        }

        switch (lift_state) {
            case -1:
                if (next_lift_state) {
                    lift_state = 1;
                }
                break;
            case 1:  // Move arm to score on forward alliance goals
                spike_wrist_target = SPIKE_WRIST_ALLIANCE_GOAL_POS;
                spike_arm_target = SPIKE_ARM_ALLIANCE_GOAL_POS;
                lift_target = LIFT_ALLIANCE_GOAL_POS;
                if (next_lift_state) {
                    lift_state = 2;
                }
                break;
            case 2:  // Move arm to score on low neutral goals
                spike_wrist_target = SPIKE_WRIST_LOW_GOAL_POS;
                if ( (lift_pos < 0.8 * lift_target) || (spike_arm_pos < 0.8 * spike_arm_target) ) {
                    spike_wrist_target = SPIKE_WRIST_CHASSIS_CLEAR;
                }
                spike_arm_target = SPIKE_ARM_LOW_GOAL_POS;
                lift_target = LIFT_LOW_GOAL_POS;
                if (next_lift_state) {
                    lift_state = 3;
                }
                break;
            case 3:  // Move arm to score on high neutral goals
                spike_wrist_target = SPIKE_WRIST_HIGH_GOAL_POS;
                if ( (lift_pos < 0.8 * lift_target) || (spike_arm_pos < 0.8 * spike_arm_target) ) {
                    spike_wrist_target = SPIKE_WRIST_CHASSIS_CLEAR;
                }
                spike_arm_target = SPIKE_ARM_HIGH_GOAL_POS;
                lift_target = LIFT_HIGH_GOAL_POS;
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
                spike_arm_target = SPIKE_ARM_READY_POS;
                spike_wrist_target = SPIKE_WRIST_READY_POS;
                if (next_collect_state) {
                    spike_arm_state = 2;
                    last_state_time = millis();
                }
                break;
            case 2:  // Stamp down hard for a time
                lift_target = 0;
                spike_wrist_target = SPIKE_WRIST_GRAB_POS;
                spike_arm_speed = -127;
                spike_arm_target = -1;
                if ((millis() - last_state_time > 500) && (!just_toggled_collect)) {
                    spike_arm_state = 1;
                }
                if (next_collect_state) {
                    spike_arm_state = 1;
                }
                break;
            case 4:  // Hold down to pick up
                lift_target = 0;
                spike_arm_target = SPIKE_ARM_GRAB_POS;
                spike_wrist_target = SPIKE_WRIST_GRAB_POS;
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
            lift_state = -1;
            spike_wrist_target = SPIKE_WRIST_STORE_POS;
            spike_arm_target = SPIKE_ARM_STORE_POS;
            lift_target = 0;
        }

        if (controller.get_digital(DIGITAL_DOWN)) {
            if (spike_wrist_target != -1) {
                spike_wrist_target += 5 * (sin(millis() / 15) + sin(millis() / 18));
            }
            spike_speed = SPIKE_DROP_SPEED;
            just_dropped_time = millis();
            just_toggled_drop = true;
        }
        else {
            if (just_toggled_drop) {
                if (millis() - just_dropped_time < 500) {
                    spike_speed = SPIKE_RETURN_SPEED;
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

        if (spike_wrist_target != -1) {
            if ((spike_arm_pos <= SPIKE_ARM_CHASSIS_CLEAR) && (spike_wrist_pos <= SPIKE_WRIST_CHASSIS_CLEAR) && (spike_wrist_target >= SPIKE_WRIST_CHASSIS_COLLIDE) && collision_avoid_state == -1) {
                collision_avoid_state = 1;
                initial_wrist_pos = spike_wrist_pos;
            }
            if ((spike_arm_pos <= SPIKE_ARM_CHASSIS_CLEAR) && (spike_wrist_pos >= SPIKE_WRIST_CHASSIS_COLLIDE) && (spike_wrist_target <= SPIKE_WRIST_CHASSIS_CLEAR) && collision_avoid_state == -1) {
                collision_avoid_state = 9;
                initial_wrist_pos = spike_wrist_pos;
            }
        }

        switch (collision_avoid_state) {
            case -1:    // Default chill state
                break;

            case 1:     // Move arm up while holding wrist still
                new_wrist_target = initial_wrist_pos;
                new_arm_target = SPIKE_ARM_CHASSIS_CLEAR + 200;
                if (spike_arm_pos > SPIKE_ARM_CHASSIS_CLEAR) {
                    
                    // If arm will keep moving up, we're done
                    if (spike_arm_target >= SPIKE_ARM_CHASSIS_CLEAR + 200) {
                        collision_avoid_state = -1;
                    }
                    else {
                        collision_avoid_state = 2;
                    }
                }
                break;
            case 2:     // Hold arm and allow wrist to move
                new_arm_target = SPIKE_ARM_CHASSIS_CLEAR + 200;
                if (spike_wrist_pos >= SPIKE_WRIST_CHASSIS_CLEAR) {
                    collision_avoid_state = -1;
                }
                break;

            case 9:     // Move arm up while holding wrist still
                new_wrist_target = SPIKE_WRIST_CHASSIS_CLEAR + 35;
                new_arm_target = SPIKE_ARM_CHASSIS_CLEAR + 200;
                if (spike_arm_pos > SPIKE_ARM_CHASSIS_CLEAR) {
                    // If arm will keep moving up, we're done
                    if (spike_arm_target >= SPIKE_ARM_CHASSIS_CLEAR + 200) {
                        collision_avoid_state = -1;
                    }
                    else {
                        collision_avoid_state = 10;
                    }
                }
                break;
            case 10:     // Hold arm and allow wrist to move
                new_arm_target = SPIKE_ARM_CHASSIS_CLEAR + 200;
                if (spike_wrist_pos <= SPIKE_WRIST_CHASSIS_COLLIDE) {
                    collision_avoid_state = -1;
                }
                break;
                break;

            default:
                collision_avoid_state = -1;
                break;
        }

        if (new_arm_target != -1) {
            spike_arm_speed = (new_arm_target - spike_arm_pos) * SPIKE_ARM_RATE;
        }
        if (new_wrist_target != -1) {
            spike_wrist_speed = (new_wrist_target - spike_wrist_pos) * SPIKE_WRIST_RATE;
        }


        double lift_speed_left = lift_speed;
        double lift_speed_right = lift_speed;
        if (lift_target != -1) {
            lift_speed_left = (lift_target - lift_pos_left) * LIFT_RATE;
            // Try moving right lift to be the same as left is
            lift_speed_right = (lift_pos_left - lift_pos_right) * LIFT_RATE;
        }

        if (controller.get_digital(DIGITAL_UP)) {
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
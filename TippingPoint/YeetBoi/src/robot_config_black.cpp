#include "main.h"


void init_robot_black() {
    this_robot.name = "B";

    this_robot.BASE_LIFT_YEET_POS = 1100;

    this_robot.SPIKE_ARM_RATE = 1.5;
    this_robot.SPIKE_WRIST_RATE = 10;
    this_robot.SPIKE_SHAKE_AMPLITUDE = 3;

    this_robot.SPIKE_STAMP_TIME = 250;

    // Base righting
    this_robot.BASE_RIGHT_WRIST_POS_1 = 265;
    this_robot.BASE_RIGHT_ARM_POS_1 = 2050;
    this_robot.BASE_RIGHT_WRIST_POS_2 = 204;
    this_robot.BASE_RIGHT_ARM_POS_2 = 1300;
    this_robot.BASE_RIGHT_WRIST_POS_3 = 204;
    this_robot.BASE_RIGHT_ARM_POS_3 = 2200;

    // When plunging down on ring
    this_robot.SPIKE_WRIST_GRAB_POS = 201;
    this_robot.SPIKE_ARM_GRAB_POS = 0;

    // Angle for when waiting to grab ring
    this_robot.SPIKE_WRIST_READY_POS = 201;
    this_robot.SPIKE_ARM_READY_POS = 1400;
    
    this_robot.SPIKE_WRIST_RANGE = 2740;

    // Collide protection
    this_robot.SPIKE_ARM_CHASSIS_CLEAR = 2500;
    this_robot.SPIKE_WRIST_CHASSIS_CLEAR = 190;
    this_robot.SPIKE_WRIST_CHASSIS_COLLIDE = 130;

    // Safety
    this_robot.SPIKE_WRIST_STORE_POS = 20;
    this_robot.SPIKE_ARM_STORE_POS = 400;

    // Back alliance goal
    this_robot.ALLIANCE_HELD_ARM_POS = 5364.0;
    this_robot.ALLIANCE_HELD_WRIST_POS = 187.34;

    this_robot.SPIKE_DROP_SPEED = 115;
    this_robot.SPIKE_RETURN_SPEED = -70;

    this_robot.LIFT_RATE = 1;

    this_robot.LEFT_LIFT_OFFSET = 80;

    // Alliance goal forwards
    this_robot.SPIKE_WRIST_ALLIANCE_GOAL_POS = 236;
    this_robot.SPIKE_ARM_ALLIANCE_GOAL_POS = 2300;
    this_robot.LIFT_ALLIANCE_GOAL_POS = 0;

    // Scoring for middle branches
    this_robot.SPIKE_WRIST_LOW_GOAL_POS = 131.97;
    this_robot.SPIKE_ARM_LOW_GOAL_POS = 2910.0;
    this_robot.LIFT_LOW_GOAL_POS = 1050.0;

    // Scoring for high branches
    this_robot.SPIKE_WRIST_HIGH_GOAL_POS = 129.0;
    this_robot.SPIKE_ARM_HIGH_GOAL_POS = 4030.0;
    this_robot.LIFT_HIGH_GOAL_POS = 3700.0;

    this_robot.BASE_LIFT_RATE = 1;
    this_robot.BASE_RELEASE_RATE = 1;

    this_robot.SPIKE_WRIST_ADJUST_AMOUNT = 1;
    this_robot.SPIKE_ARM_ADJUST_AMOUNT = 60;

    this_robot.BASE_LIFT_READY_POS = 1800;
    this_robot.BASE_LIFT_HOLD_POS = 3100;
    this_robot.BASE_LIFT_DROP_POS = 1800;
    this_robot.BASE_SPIN_OFFSET = 3100 - 1934;
    this_robot.BASE_RELEASE_DROP_POS = -800;
    this_robot.BASE_RELEASE_READY_POS = 250;
    this_robot.BASE_RELEASE_HOLD_POS = -1;


    this_robot.BELLY_GRAB_RATE_DOWN = 0.1;
    this_robot.BELLY_GRAB_RATE_UP = 0.5;

    this_robot.BELLY_GRAB_DOWN = 0;
    this_robot.BELLY_GRAB_UP = 2000;

    this_robot.SLEW_STEP_FORWARD = 15;
    this_robot.SLEW_STEP_TURN = 15;

    // How many inches of travel does the robot move per tick of the encoder
    this_robot.INCHES_PER_TICK = 0.02836160034;

    // How far apart are the tracking wheels
    this_robot.TRACKING_DIAMETER = 12;

    // Fudge Factors
    this_robot.FORWARD_FUDGE_FACTOR = -(24.0/23.5) / (3.25 / 2.75) * (24.0 / 24.25);
    this_robot.STRAFE_FUDGE_FACTOR = (24.0 / 23.3) * (24.0 / 24.4);
    this_robot.ROTATIONAL_FUDGE_FACTOR = (0.99699 / (3.25 / 2.75)) * (360.0/367.0) * (360.0/357.0) * (1800.0 / 1830.0) * (1800.0 / 1797.0) * (3600.0 / 3592.0);
    
    // Auton drive stuff
    this_robot.DRIVE_TURN_RATE = 3.0;
    this_robot.DRIVE_DRIVE_TURN_RATE = 10.0;
    this_robot.DRIVE_RATE = 8.0;
    this_robot.MIN_DRIVE_SPEED = 30.0;
    this_robot.MAX_TURN_SPEED = 127.0;
    this_robot.MIN_TURN_SPEED = 50.0;
    this_robot.TURN_PULSE_ON = 1;
    this_robot.TURN_PULSE_OFF = 10;
    this_robot.TURN_PRECISION = 1;  // +/- 1 degress precision
    this_robot.DRIVE_PRECISION = 3;  // +/- 3 inches precision

    this_robot.SPIKE_WRIST_LOCK_BELLY = 75;

}


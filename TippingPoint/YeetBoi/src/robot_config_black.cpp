#include "main.h"


void init_robot_black() {
    this_robot.name = "B";

    this_robot.BASE_LIFT_YEET_POS = 1100;

    this_robot.SPIKE_ARM_RATE = 1.5;
    this_robot.SPIKE_WRIST_RATE = 10;
    this_robot.SPIKE_SHAKE_AMPLITUDE = 3;

    this_robot.SPIKE_WRIST_GRAB_POS = 203;
    this_robot.SPIKE_WRIST_READY_POS = 208;
    this_robot.SPIKE_ARM_READY_POS = 1700;
    this_robot.SPIKE_ARM_GRAB_POS = 600;
    this_robot.SPIKE_WRIST_RANGE = 2740;

    this_robot.SPIKE_ARM_CHASSIS_CLEAR = 2300;
    this_robot.SPIKE_WRIST_CHASSIS_CLEAR = 190;
    this_robot.SPIKE_WRIST_CHASSIS_COLLIDE = 130;

    this_robot.SPIKE_WRIST_STORE_POS = 20;
    this_robot.SPIKE_ARM_STORE_POS = 400;

    this_robot.ALLIANCE_HELD_ARM_POS = 5600;
    this_robot.ALLIANCE_HELD_WRIST_POS = 190;

    this_robot.SPIKE_DROP_SPEED = 110;
    this_robot.SPIKE_RETURN_SPEED = -70;

    this_robot.LIFT_RATE = 1;

    this_robot.LEFT_LIFT_OFFSET = 80;
    this_robot.SPIKE_WRIST_ALLIANCE_GOAL_POS = 245;
    this_robot.SPIKE_ARM_ALLIANCE_GOAL_POS = 2750;
    this_robot.LIFT_ALLIANCE_GOAL_POS = 0;

    this_robot.SPIKE_WRIST_LOW_GOAL_POS = 135;
    this_robot.SPIKE_ARM_LOW_GOAL_POS = 3100;
    this_robot.LIFT_LOW_GOAL_POS = 900;

    this_robot.SPIKE_WRIST_HIGH_GOAL_POS = 138;
    this_robot.SPIKE_ARM_HIGH_GOAL_POS = 4410;
    this_robot.LIFT_HIGH_GOAL_POS = 3700;

    this_robot.BASE_LIFT_RATE = 1;
    this_robot.BASE_RELEASE_RATE = 1;

    this_robot.BASE_LIFT_READY_POS = 1800;
    this_robot.BASE_LIFT_HOLD_POS = 3100;
    this_robot.BASE_SPIN_OFFSET = 3100 - 1934;
    this_robot.BASE_RELEASE_DROP_POS = -740;
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
    this_robot.FORWARD_FUDGE_FACTOR = -(24.0/23.5) / (3.25 / 2.75);
    this_robot.STRAFE_FUDGE_FACTOR = (24.0/23.3);
    this_robot.ROTATIONAL_FUDGE_FACTOR = (0.99699 / (3.25 / 2.75)) * (360.0/367.0) * (360.0/357.0);

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
    this_robot.DRIVE_PRECISION = 2;  // +/- 1 inches precision

}


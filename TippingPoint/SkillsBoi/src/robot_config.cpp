#include "main.h"


void init_robot() {
    this_robot.name = "S";

    // How high to hold base grab during yeet (lower than normal grab due to extra wheels)
    this_robot.BASE_LIFT_YEET_POS = 1100;

    this_robot.BACK_LIFT_RATE = 1.0;
    this_robot.FRONT_LIFT_RATE = 1.0;
    this_robot.SIDE_LIFT_RATE = 2.0;

    this_robot.INTAKE_VOMIT_THRESHOLD = 50;
    this_robot.INTAKE_VOMIT_TIMEOUT = 100;
    this_robot.INTAKE_VOMIT_TIME = 1000;


    this_robot.INTAKE_WOBBLE_IN = -50;
    this_robot.INTAKE_WOBBLE_OUT = 50;
    this_robot.INTAKE_WOBBLE_PERIOD_RIGHT = 750;
    this_robot.INTAKE_WOBBLE_PERIOD_LEFT = (double(this_robot.INTAKE_WOBBLE_PERIOD_RIGHT) / 2); // WAS DIV 2

    this_robot.BACK_LIFT_READY_POS = -2500;
    this_robot.BACK_LIFT_HEIGHT_DROP = 200;
    this_robot.BACK_LIFT_HOLD_POS = 0;
    this_robot.FRONT_WOBBLE_AMP = 600;
    this_robot.FRONT_LIFT_READY_POS = -3450;
    this_robot.FRONT_LIFT_HOLD_POS = 0;
    this_robot.FRONT_LIFT_HEIGHT_DROP = 150;
    this_robot.FRONT_LIFT_HEIGHT_DROP_HIGH = 1200;
    this_robot.FRONT_LIFT_PARK_POS = -2450;

    this_robot.SIDE_LIFT_READY_POS = -1500;
    this_robot.SIDE_LIFT_HOLD_POS = 0;

    // Step size for driver control slew (burn-out protection)
    this_robot.SLEW_STEP_FORWARD = 5;
    this_robot.SLEW_STEP_TURN = 5;

    // How many inches of travel does the robot move per tick of the encoder
    this_robot.INCHES_PER_TICK = 0.02836160034;

    // How far apart are the tracking wheels
    this_robot.TRACKING_DIAMETER = 12;

    // Fudge Factors for odometry
    this_robot.FORWARD_FUDGE_FACTOR = 72.0 / 70.9;
    this_robot.STRAFE_FUDGE_FACTOR = 48.0 / 47.59;
    this_robot.ROTATIONAL_FUDGE_FACTOR = (5.0 * 360.0) / ((5.0 * 360.0) + 10.0);
    
    // Auton drive tuning
    this_robot.DRIVE_TURN_RATE = 3.0;
    this_robot.DRIVE_DRIVE_TURN_RATE = 5.0;
    this_robot.DRIVE_RATE = 0.075;
    this_robot.MIN_DRIVE_SPEED = 25.0;
    this_robot.MAX_TURN_SPEED = 127.0;
    this_robot.MIN_TURN_SPEED = 25.0;
    this_robot.TURN_PULSE_ON = 10;
    this_robot.TURN_PULSE_OFF = 7;
    this_robot.TURN_PRECISION = 2;  // +/- 2.5 degress precision
    this_robot.DRIVE_PRECISION = 2;  // +/- 3 inches precision

    this_robot.TURN_CORRECT_TIME = 10;

}


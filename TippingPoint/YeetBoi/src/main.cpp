#include "main.h"


using namespace pros;

Controller controller(E_CONTROLLER_MASTER);


void init_positions() {

    base_lift.move_voltage(-3000);
    spike_arm.move_voltage(-3000);
    spike_wrist.move_voltage(-3000);
    base_release.move_voltage(5000);
    lift_right.move_voltage(-2000);
    lift_left.move_voltage(-2000);
    delay(1000);
    lift_left.tare_position();
    lift_left.move_voltage(0);
    lift_right.tare_position();
    lift_right.move_voltage(0);
    base_lift.tare_position();
    base_lift.move_voltage(0);
    spike_arm.tare_position();
    spike_arm.move_voltage(0);
    spike_wrist.tare_position();
    spike_wrist.move_voltage(0);
    belly_grab.tare_position();
    base_release.tare_position();
    base_release.move_voltage(0);
}

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
    yeet_release.set_value(0);
    yeet_retract.set_value(0);
    init_tracking();
    init_positions();
    delay(200);
    pros::Task drive_task(run_drive, (void*)"PROS", TASK_PRIORITY_DEFAULT, TASK_STACK_DEPTH_DEFAULT, "Drive Task");
    pros::Task display_task(run_display, (void*)"PROS", TASK_PRIORITY_DEFAULT, TASK_STACK_DEPTH_DEFAULT, "Display Task");
    pros::Task belly_grab_task(run_belly_grab, (void*)"PROS", TASK_PRIORITY_DEFAULT, TASK_STACK_DEPTH_DEFAULT, "Belly Grab Task");
    pros::Task base_grab_task(run_base_lift, (void*)"PROS", TASK_PRIORITY_DEFAULT, TASK_STACK_DEPTH_DEFAULT, "Base Grab Task");
    pros::Task lift_task(run_lift, (void*)"PROS", TASK_PRIORITY_DEFAULT, TASK_STACK_DEPTH_DEFAULT, "Lift Task");
    pros::Task yeet_task(run_yeet, (void*)"PROS", TASK_PRIORITY_DEFAULT, TASK_STACK_DEPTH_DEFAULT, "Yeet Task");
}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {}

/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
void competition_initialize() {}


void autonomous() {}


void opcontrol() {
//
//    while (true) {
//        double spikeSpeed = 0;
//        double baseLiftSpeed = 0;
//        double baseTurnSpeed = 0;
//        double baseReleaseSpeed = 0;
//        double mainLiftSpeed = 0;
//        double spikeArmSpeed = 0;
//        double spikeWristSpeed = 0;
//        double bellyGrabSpeed = 0;
//
//
//        if (controller.get_digital(DIGITAL_L1)) {
//            baseLiftSpeed = 127;
//        }
//        if (controller.get_digital(DIGITAL_L2)) {
//            baseLiftSpeed = -127;
//        }
//        if (controller.get_digital(DIGITAL_Y)) {
//            mainLiftSpeed = 127;
//        }
//        if (controller.get_digital(DIGITAL_A)) {
//            mainLiftSpeed = -127;
//        }
//        if (controller.get_digital(DIGITAL_LEFT)) {
//            spikeArmSpeed = 127;
//        }
//        if (controller.get_digital(DIGITAL_RIGHT)) {
//            spikeArmSpeed = -127;
//        }
//        if (controller.get_digital(DIGITAL_R2)) {
//            bellyGrabSpeed = 127;
//        }
//        if (controller.get_digital(DIGITAL_R1)) {
//            bellyGrabSpeed = -127;
//        }
//        if (controller.get_digital(DIGITAL_X)) {
//            spikeWristSpeed = 127;
//        }
//        if (controller.get_digital(DIGITAL_B)) {
//            spikeWristSpeed = -127;
//        }
//
//        baseLift.move_voltage(12000 * baseLiftSpeed / 127);
//
//        bellyGrab.move_voltage(12000 * bellyGrabSpeed / 127);
//
//        mainLiftLeft.move_voltage(12000 * mainLiftSpeed / 127);
//        mainLiftRight.move_voltage(12000 * mainLiftSpeed / 127);
//
//        spikeArm.move_voltage(12000 * spikeArmSpeed / 127);
//
//        spikeWrist.move_voltage(12000 * spikeWristSpeed / 127);
//
////      // Arcade
//        double forwardSpeed = controller.get_analog(ANALOG_LEFT_Y);
//        double turnSpeed = controller.get_analog(ANALOG_RIGHT_X);
//
//        // Drive Motors
//        driveR1.move_voltage(12000 * (forwardSpeed - turnSpeed) / 127);
//        driveR2.move_voltage(12000 * (forwardSpeed - turnSpeed) / 127);
//        driveR3.move_voltage(12000 * (forwardSpeed - turnSpeed) / 127);
//        driveL1.move_voltage(12000 * (forwardSpeed + turnSpeed) / 127);
//        driveL2.move_voltage(12000 * (forwardSpeed + turnSpeed) / 127);
//        driveL3.move_voltage(12000 * (forwardSpeed + turnSpeed) / 127);
//
////        // Tank
////        double leftSpeed = controller.get_analog(ANALOG_LEFT_Y);
////        double rightSpeed = controller.get_analog(ANALOG_RIGHT_Y);
////
////        // Drive Motors
////        driveR1.move_voltage(12000 * (rightSpeed) / 127);
////        driveR2.move_voltage(12000 * (rightSpeed) / 127);
////        driveR3.move_voltage(12000 * (rightSpeed) / 127);
////        driveL1.move_voltage(12000 * (leftSpeed) / 127);
////        driveL2.move_voltage(12000 * (leftSpeed) / 127);
////        driveL3.move_voltage(12000 * (leftSpeed) / 127);
//
//        // Print info to the screen
//        double leftEncoderPos = leftEncoder.get_value();
//        double rightEncoderPos = rightEncoder.get_value();
//        double middleEncoderPos = middleEncoder.get_value();
//
//        lcd::print(0, "Left: %f\n", leftEncoderPos);
//        lcd::print(1, "Middle: %f\n", middleEncoderPos);
//        lcd::print(2, "Right: %f\n", rightEncoderPos);
//
//        pros::delay(20);
//    }
    while (true) {
        delay(100);
    }
}

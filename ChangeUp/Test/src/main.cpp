#include "main.h"

#define SPEED E_MOTOR_GEARSET_18

#define SPIKE_OPEN_SPEED 80
#define SPIKE_CLOSE_SPEED -45

using namespace pros;

Controller controller(E_CONTROLLER_MASTER);

// Spike motor declarations
Motor spike(20, SPEED, 0);

// Drive motor declarations
Motor driveR1(8,SPEED,1);
Motor driveR2(5,SPEED,0);
Motor driveR3(4,SPEED,1);
Motor driveR4(3,SPEED,0);

Motor driveL1(10,SPEED,0);
Motor driveL2(7,SPEED,1);
Motor driveL3(6,SPEED,0);
Motor driveL4(9,SPEED,1);


/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
    pros::lcd::initialize();
    spike.move_voltage(SPIKE_CLOSE_SPEED * 12000 / 127);
    delay(500);
    spike.move_voltage(0);
    delay(250);
    spike.tare_position();
    
    delay(50);
    spike.move_voltage(SPIKE_CLOSE_SPEED * 12000 / 127);
    delay(50);
    spike.move_voltage(0);
    delay(50);
    spike.move_voltage(-SPIKE_CLOSE_SPEED * 12000 / 127);
    delay(50);
    spike.move_voltage(0);
    delay(50);
    spike.move_voltage(SPIKE_CLOSE_SPEED * 12000 / 127);
    delay(50);
    spike.move_voltage(0);

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
    
    int spikeState = -1;
    double stateTime = 0;

	while (true) {
        double spikeSpeed = 0;
        
        double spikePosition = spike.get_position();
        
        pros::lcd::print(5, "Spike Position: %f", spikePosition);

        if (controller.get_digital(DIGITAL_L1)) {
            spikeState = 1;
        }
        if (controller.get_digital(DIGITAL_L2)) {
            spikeState = 4;
        }

        
        if (controller.get_digital(DIGITAL_LEFT)) {
            spikeSpeed = SPIKE_CLOSE_SPEED;
            spikeState = -1;
        }
        if (controller.get_digital(DIGITAL_RIGHT)) {
            spikeSpeed = SPIKE_OPEN_SPEED;
            spikeState = -1;
        }
        
        if (controller.get_digital(DIGITAL_UP)) {
            spikeSpeed = 0;
            spikeState = -1;
        }
        
        switch (spikeState) {
            case 1:  // Begin opening
                stateTime = millis();
                spikeState = 2;
            case 2: // Open until done
                spikeSpeed = SPIKE_OPEN_SPEED;
                if (((millis() - stateTime) > 500) or (spikePosition > 100)) {
                    stateTime = millis();
                    spikeState = 3;
                }
                break;
            case 3:  // Hold open
                spikeSpeed = SPIKE_OPEN_SPEED / 4;
                if ((millis() - stateTime) > 250) {
                    spikeState = -1;
                }
                break;


            case 4:  // Begin closing
                stateTime = millis();
                spikeState = 5;
            case 5: // Close until done
                spikeSpeed = SPIKE_CLOSE_SPEED;
                if (((millis() - stateTime) > 500) or (spikePosition < 40)) {
                    stateTime = millis();
                    spikeState = 6;
                }
                break;
            case 6:  // Hold close
                spikeSpeed = -SPIKE_OPEN_SPEED / 4;
                if ((millis() - stateTime) > 250) {
                    spikeState = -1;
                }
                break;

            default:
                spikeState = -1;
                break;
        }
        
        pros::lcd::print(6, "Spike State: %d", spikeState);
        pros::lcd::print(7, "Spike Speed: %f", spikeSpeed);
        spike.move_voltage(12000 * spikeSpeed / 127);

//        Arcade
        double forwardSpeed = controller.get_analog(ANALOG_LEFT_Y);
        double turnSpeed = controller.get_analog(ANALOG_RIGHT_X);

        // Drive Motors
        driveR1.move_voltage(12000 * (forwardSpeed - turnSpeed) / 127);
        driveR2.move_voltage(12000 * (forwardSpeed - turnSpeed) / 127);
        driveR3.move_voltage(12000 * (forwardSpeed - turnSpeed) / 127);
        driveR4.move_voltage(12000 * (forwardSpeed - turnSpeed) / 127);
        driveL1.move_voltage(12000 * (forwardSpeed + turnSpeed) / 127);
        driveL2.move_voltage(12000 * (forwardSpeed + turnSpeed) / 127);
        driveL3.move_voltage(12000 * (forwardSpeed + turnSpeed) / 127);
        driveL4.move_voltage(12000 * (forwardSpeed + turnSpeed) / 127);
        
//        Tank
//        double leftSpeed = controller.get_analog(ANALOG_LEFT_Y);
//        double rightSpeed = controller.get_analog(ANALOG_RIGHT_Y);
//
//        // Drive Motors
//        driveR1.move_voltage(12000 * (rightSpeed) / 127);
//        driveR2.move_voltage(12000 * (rightSpeed) / 127);
//        driveR3.move_voltage(12000 * (rightSpeed) / 127);
//        driveR4.move_voltage(12000 * (rightSpeed) / 127);
//        driveL1.move_voltage(12000 * (leftSpeed) / 127);
//        driveL2.move_voltage(12000 * (leftSpeed) / 127);
//        driveL3.move_voltage(12000 * (leftSpeed) / 127);
//        driveL4.move_voltage(12000 * (leftSpeed) / 127);
        
        
		pros::delay(20);
	}
}

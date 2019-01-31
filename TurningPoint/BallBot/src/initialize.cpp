#include "main.h"

/*
 #define BTN_TOGGLE DIGITAL_DOWN
 #define BTN_ABORT DIGITAL_UP
 
 // Flywheel
 #define BTN_FIRE_HIGH DIGITAL_L1
 #define BTN_FIRE_LOW DIGITAL_L2
 #define BTN_INTAKE_IN DIGITAL_R1
 #define BTN_INTAKE_OUT DIGITAL_R2
 #define BTN_FIRE_BOTH DIGITAL_B
 
 
 // Arm
 #define BTN_WRIST_UP DIGITAL_RIGHT
 #define BTN_WRIST_DOWN DIGITAL_LEFT
 #define BTN_ARM_UP DIGITAL_X
 #define BTN_ARM_DOWN DIGITAL_B
 #define BTN_FLIPPER_LEFT DIGITAL_Y
 #define BTN_FLIPPER_RIGHT DIGITAL_A
 #define BTN_FLIP DIGITAL_R1
 #define BTN_WRIST DIGITAL_R2
 #define BTN_ARM_HIGH DIGITAL_L1
 #define BTN_ARM_LOW DIGITAL_L2
 */

void on_center_button() {
    // Do something if we like
}

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
    pros::lcd::initialize();
    pros::lcd::set_text(0, "USC VEX MULTI-BOT");
    
    pros::lcd::register_btn1_cb(on_center_button);
    
    initAll();  // My function
    
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

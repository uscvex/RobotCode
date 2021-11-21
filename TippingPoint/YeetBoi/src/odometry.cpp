#include "main.h"

using namespace pros;

// Define encoder ports and direction
ADIEncoder left_encoder(3, 4, false);
ADIEncoder middle_encoder(5, 6, false);
ADIEncoder right_encoder(7, 8, false);

// Global variables to track the position of the robot
double robot_theta = 0;
double robot_x = 0;
double robot_y = 0;

// Variables to track the previous values of the encoders
double last_right_encoder = 0;
double last_left_encoder = 0;
double last_middle_encoder = 0;

void init_tracking() {
    robot_theta = 0;
    robot_x = 0;
    robot_y = 0;
    right_encoder.reset();
    left_encoder.reset();
    middle_encoder.reset();
    last_right_encoder = 0;
    last_left_encoder = 0;
    last_middle_encoder = 0;
}

void set_direction(double new_dir) {
    robot_theta = new_dir;
    if (robot_theta > 360) {
        robot_theta -= 360;
    }
}

void set_position(double x, double y, double d) {
    robot_x = x;
    robot_y = y;
    robot_theta = d;
}

void set_position(double x, double y) {
    robot_x = x;
    robot_y = y;
}

double pythag(double x1, double y1, double x2, double y2) {
    return sqrt( ((x1-x2) * (x1-x2)) + ((y1-y2) * (y1-y2)) );
}

void track_position() {

    // Get values from encoders
    double right_value = right_encoder.get_value();
    double left_value = left_encoder.get_value();
    double middle_value = middle_encoder.get_value();
    
    // Calculate change in encoder values
    double right_change = right_value - last_right_encoder;
    double left_change = left_value - last_left_encoder;
    double middle_change = middle_value - last_middle_encoder;
    
    // Update last values
    last_right_encoder = right_value;
    last_left_encoder = left_value;
    last_middle_encoder = middle_value;
    
    // Now find average change in encoders (ticks)
    double angle_change = right_change + left_change;
    //angle_change /= 2;
    
    // Take out the rotational component from strafe wheel
    middle_change -= angle_change / 2;
    
    // Convert ticks to inches
    angle_change *= this_robot.INCHES_PER_TICK;
    
    // Find actual angle change (rad) based on distance between encoders
    angle_change /= this_robot.TRACKING_DIAMETER;
    
    // Convert to degrees
    angle_change = (180 * angle_change) / (double)M_PI;
    
    // Calculate new global direction
    robot_theta += angle_change * this_robot.ROTATIONAL_FUDGE_FACTOR;
    
    // Wrap to [0,360]
    if (robot_theta > 360) robot_theta -= 360;
    if (robot_theta < 0) robot_theta += 360;
    
    // Get new global direction in degrees
    double robot_theta_rad = ((double)M_PI * robot_theta) / 180;
    
    // Find change in forward position and sideways position
    double forward_change = this_robot.FORWARD_FUDGE_FACTOR * this_robot.INCHES_PER_TICK * (left_change - right_change) / 2;
    double strafe_change = this_robot.STRAFE_FUDGE_FACTOR * middle_change * this_robot.INCHES_PER_TICK;
    
    // Calculate global change based on current angle
    double change_x = forward_change * sin(robot_theta_rad) + strafe_change * cos(robot_theta_rad);
    double change_y = strafe_change * sin(robot_theta_rad) - forward_change * cos(robot_theta_rad);
    
    // Calculate new global position
    robot_x += change_x;
    robot_y += change_y;

    if (controller.get_digital(DIGITAL_UP)) {
        set_position(0, 0, 0);
    }

}

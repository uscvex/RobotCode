#include "main.h"

#include <vector>

using namespace pros;

// Vision Sensor Stuff
#define DEFAULT 0                       // Choose colour of flag based on auton mode
#define MAX_FLAG_WIDTH 150              // Widest object camera will recognise
#define MAX_FLAG_HEIGHT 500             // Tallest object camera will recognise
#define MIN_FLAG_Y -200                 // Lowest camera will recognise object
#define AIM_ACCEPT 5                    // Stop auto-aiming within x
#define FLAG_OFFSET 1                   // Value to add/subtract to angle to hit flag closer to edge
#define FLYWHEEL_AIM_RANGE 5            // fire ball when within x degrees of flag
#define VISION_SEEK_RATE 3              // How fast to turn to aim, bigger = slower

#define BLUE_FLAG 1
#define RED_FLAG 2
#define GREEN_FLAG 3

#define CENTER 0
#define LEFT 1
#define RIGHT 2

#define BLUE_CODE_ID 11
#define RED_CODE_ID 19

#define REDAUTON 0
#define BLUEAUTON 1
#define SKILLSAUTON 2
#define REDBACKAUTON 3
#define BLUEBACKAUTON 4

int autonSelect = REDAUTON;

Controller controller(E_CONTROLLER_MASTER);     // Controller object
Vision camera(16);

pros::vision_signature_s_t GREEN_SIG;
pros::vision_signature_s_t RED_SIG;
pros::vision_signature_s_t BLUE_SIG;

pros::vision_color_code_t BLUE_CODE;
pros::vision_color_code_t RED_CODE;

void calibrateVision() {
    
    // Calibration for Blue Bot
    if (false){//autonSelect == BLUEBACKAUTON || autonSelect == REDBACKAUTON) {
        camera.set_exposure(70);
    }
    
    // Calibration for Red Bot
    else {
        
        // Create signatures for calibration
        BLUE_SIG =
        pros::Vision::signature_from_utility(BLUE_FLAG, -2473, -955, -1714, 9031, 12743, 10887, 1.7, 1);
        
        RED_SIG =
        pros::Vision::signature_from_utility(RED_FLAG, 9427, 11033, 10230, -1413, -773, -1093, 1.9, 1);
        
        GREEN_SIG =
        pros::Vision::signature_from_utility(GREEN_FLAG, -2197, -1177, -1687, -5471, -4937, -5204, 6.8, 1);
        
        
        camera.set_signature(BLUE_FLAG, &BLUE_SIG);
        camera.set_signature(RED_FLAG, &RED_SIG);
        camera.set_signature(GREEN_FLAG, &GREEN_SIG);
        
        BLUE_CODE = camera.create_color_code(BLUE_FLAG,GREEN_FLAG);
        RED_CODE = camera.create_color_code(RED_FLAG,GREEN_FLAG);
        
        camera.set_exposure(60);
        
    }
    
    camera.set_zero_point(pros::E_VISION_ZERO_TOPLEFT);
    

    
}

// Read vision sensor to get angle needed to turn
// Returns angle to desired target
// Or 0 if error
double getRelativeAngle(int location = CENTER, int target = DEFAULT) {
    
    int lookingFor = BLUE_FLAG;     // default to red-team
    if (autonSelect == BLUEAUTON || autonSelect == BLUEBACKAUTON)
        lookingFor = RED_FLAG;      // but change to blue if needed
    
    if (target != DEFAULT)
        lookingFor = target;
    
    // Containers for the things we'll see
    std::vector<vision_object_s_t> blueThings;
    std::vector<vision_object_s_t> redThings;
    
    // Find number of objects visable
    int noObjs = camera.get_object_count();
    
    
    if (noObjs > 100)      // Camera error, so don't aim
        return 0;
    
    // Got through all objects seen
    for (int i = 0; i < noObjs; i++) {
        vision_object_s_t thisThing = camera.get_by_size(i);
        // Print their info
        
        // If object is a colour code
        if (thisThing.type == 1) {
            if (thisThing.signature == RED_CODE_ID) {
                redThings.push_back(thisThing);
            }
            if (thisThing.signature == BLUE_CODE_ID) {
                blueThings.push_back(thisThing);
            }
        }
    }
    
    std::vector<vision_object_s_t> *theseThings;
    
    if (lookingFor == BLUE_FLAG)
        theseThings = &blueThings;
    if (lookingFor == RED_FLAG)
        theseThings = &redThings;
    
    // Find which object is closest to left/middle/right
    double closestDist;
    if (location == CENTER) {
        closestDist = 10000;
        for (int i = 0; i < (*theseThings).size(); i++) {
            if (abs((*theseThings)[i].x_middle_coord - (VISION_FOV_WIDTH/2)) < closestDist) {
                closestDist = (*theseThings)[i].x_middle_coord;
            }
        }
    }
    if (location == LEFT) {
        closestDist = 10000;
        for (int i = 0; i < (*theseThings).size(); i++) {
            if ((*theseThings)[i].x_middle_coord < closestDist) {
                closestDist = (*theseThings)[i].x_middle_coord;
            }
        }
    }
    if (location == RIGHT) {
        closestDist = -10000;
        for (int i = 0; i < (*theseThings).size(); i++) {
            if ((*theseThings)[i].x_middle_coord > closestDist) {
                closestDist = (*theseThings)[i].x_middle_coord;
            }
        }
    }
    
    // Aim at the edge of the flag for better chance of toggleing
    if (lookingFor == RED_FLAG) closestDist += FLAG_OFFSET;
    if (lookingFor == BLUE_FLAG) closestDist -= FLAG_OFFSET;
    
    closestDist = closestDist - (VISION_FOV_WIDTH/2);
    
    if ((-closestDist/VISION_SEEK_RATE) == 0)
        return 0.001;
    
    return -closestDist/VISION_SEEK_RATE;
    
}




void opcontrol() {
    
    calibrateVision();
    
	while (true) {
		
        double angle = getRelativeAngle();
        std::cout << "\n\nAngle is: " << angle << std::endl;
        
		pros::delay(20);
	}
}

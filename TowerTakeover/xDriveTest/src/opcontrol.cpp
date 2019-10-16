#include "main.h"



ADIDigitalOut solenoid( 7, false );

void opcontrol() {

    // Call init function at start of opcontrol
    init();
    
    bool pistonOn = false;
    bool justToggled = false;
    
	while (true) {
        
        // Just chill out
        
        if (controller.get_digital(DIGITAL_B)) {
            if (!justToggled) {
                pistonOn = !pistonOn;
                solenoid.set_value(pistonOn);
            }
            justToggled = true;
        }
        else {
            justToggled = false;
        }
        
		delay(20);
	}
}

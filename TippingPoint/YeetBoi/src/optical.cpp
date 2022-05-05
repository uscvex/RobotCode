
// to do: two modes: one for auton where we don't know we have the base.
// the other is for driver control when we do know we have it
#define PROXIMITY_THRESHOLD 244


#include "main.h"
// #include <opencv2/highgui/highgui.hpp>
// #include <fstream>
// #include <stdio.h>

Optical optical(2);
double brightness = 0;
double hue = 0;
int32_t proximity = 0;
double saturation = 0;
pros::c::optical_rgb_s_t RGB_values;

bool has_base = false;
int optical_state = DO_NOTHING;
bool is_black = false;
bool is_yellow = false;
bool is_red = false;

// optical sensor readings and state changes
void init_optical(){
    // optical.disable_gesture();
    optical.set_led_pwm(100);
    cout << "optical init\n";
}

void run_optical(void* params){

    while(true){
        double base_rotate_speed = 0;
        has_base = false;
        brightness = optical.get_brightness();
        hue = optical.get_hue();
        saturation = optical.get_saturation();
        proximity = optical.get_proximity();
        RGB_values = optical.get_rgb();

        is_yellow = false;
        is_black = false;
        is_red = false;

        char buffer [50];
        printf(buffer, "R: %f \n", RGB_values.red);
        printf(buffer, "G: %f \n", RGB_values.green);
        printf(buffer, "B: %f \n", RGB_values.blue);
        printf(buffer, "Brightness: %f \n", brightness);
        printf(buffer, "hue: %f \n", hue);
        printf(buffer, "saturation: %f \n", saturation);


        if (proximity > PROXIMITY_THRESHOLD) has_base = true;
        // if saturation > threshold, it's looking for red
        // if saturation < threshold it's looking for black (the stripes)

        #define SATURATION_THRESHOLD 0.40 //0.27
        #define HUE_THRESHOLD 50

        #define MIN_HUE_THRESHOLD 20
        #define BRIGHTNESS_THRESHOLD 0.105
        #define MIN_BRIGHTNESS_THRESHOLD 0.45 //between 0.3 and 0.6 was the reasoning

        // is_yellow = ((hue < HUE_THRESHOLD && hue > MIN_HUE_THRESHOLD) && saturation > SATURATION_THRESHOLD) && brightness > BRIGHTNESS_THRESHOLD;
        is_yellow = ((hue < HUE_THRESHOLD && hue > MIN_HUE_THRESHOLD) && saturation > SATURATION_THRESHOLD);
        // is_red = ((hue > HUE_THRESHOLD || hue < MIN_HUE_THRESHOLD) && saturation > SATURATION_THRESHOLD) && (brightness > MIN_BRIGHTNESS_THRESHOLD);
        is_red = ((hue > HUE_THRESHOLD || hue < MIN_HUE_THRESHOLD) && saturation > SATURATION_THRESHOLD);
        is_black = (saturation <= SATURATION_THRESHOLD);// && (brightness <= BRIGHTNESS_THRESHOLD);
        
        // yellow hue 40 sat 0.52 brighht 0.P
        // red 357 .54 .18
        // red 0.3 0.6 0.13
        // red 1.7 0.6 0.12
        // red 0.9 0.56 0.19
        // black 29 0.29 0.07
        // black 33 0.31 0.07
        // black 25 0.23 0.08
        // black 37 0.3 0.06
        // black 37 0.27 0.103
        // black 33 0.22 0.115
        // yellow 37 0.51 0.22
        // yellow 39 0.52 0.26
        // yellow 39 0.51 0.29

        // we're prioritizing red cuz this is when the spinner stops initially- overrides all others
        if (is_red){
            is_black = false;
            is_yellow = false;
        }
        else if (is_black){
            is_yellow = false;
        }

        switch (optical_state){
            case DO_NOTHING: // default, does nothing
                optical.set_led_pwm(100);
                break;
            case LOOK_FOR_YELLOW: // look for the yellowness of the base
                // if it's yellow specifically
                optical.set_led_pwm(100);
                if (is_yellow) {
                    cout << "found yellow " << millis() << endl;
                    optical_state = LOOK_FOR_STICKER;
                }
                break;
            case LOOK_FOR_STICKER: //look for the sticker
                optical.set_led_pwm(100);
                if (is_red){
                    cout << "found sticker " << millis() << endl;
                    optical_state = FUCK_GO_BACK; // mission complete
                }
                break;
            case FUCK_GO_BACK:
                optical.set_led_pwm(100);
                // if it's not red (could be black or yellow)
                if (is_black){
                    cout << "went back " << millis() << endl;
                    optical_state = DO_NOTHING;
                }
                break;
            case FORCE_SPIN:
                break;
        }

        if (controller.get_digital(DIGITAL_UP)) {
            optical_state = DO_NOTHING;
        }

        delay(10);
    }
}


// to do: two modes: one for auton where we don't know we have the base.
// the other is for driver control when we do know we have it


#define PROXIMITY_THRESHOLD 244



#include "main.h"
// #include <opencv2/highgui/highgui.hpp>
#include <fstream>
#include <stdio.h>



Optical optical(12);
double brightness = 0;
double hue = 0;
int32_t proximity = 0;
double saturation = 0;
pros::c::optical_rgb_s_t RGB_values;

bool has_base = false;
bool is_sticker = false;

bool seek_sticker = false;
bool finished_autoscore = false;

int optical_state = DO_NOTHING;


bool is_black = false;
bool is_yellow = false;
bool is_red = false;

ofstream o;

// optical sensor readings and state changes
void init_optical(void* params){

    optical.disable_gesture();
    optical.set_led_pwm(100);
    o.open("../bin/colors.txt");
    cout << "optical init\n";

}

void set_optical(void* params){

    while(true){
        double base_rotate_speed = 0;
        is_sticker = false;
        has_base = false;

        brightness = optical.get_brightness();
        hue = optical.get_hue();
        saturation = optical.get_saturation();
        proximity = optical.get_proximity();
        RGB_values = optical.get_rgb();

        // char buffer [50];
        // snprintf(buffer, "R: %f \n", RGB_values.red);
        // o << buffer << endl;
        // snprintf(buffer, "G: %f \n", RGB_values.green);
        // o << buffer << endl;
        // snprintf(buffer, "B: %f \n", RGB_values.blue);
        // o << buffer << endl;

        char buffer [50];
        printf(buffer, "R: %f \n", RGB_values.red);
        printf(buffer, "G: %f \n", RGB_values.green);
        printf(buffer, "B: %f \n", RGB_values.blue);
        printf(buffer, "Brightness: %f \n", brightness);
        printf(buffer, "hue: %f \n", hue);
        printf(buffer, "saturation: %f \n", saturation);

        // o << "Red value: %f \n", RGB_values.red
        if (proximity > PROXIMITY_THRESHOLD) has_base = true;
        // if saturation > threshold, it's looking for red
        // if saturation < threshold it's looking for black (the stripes)
        is_black = (saturation < SATURATION_THRESHOLD);
        is_yellow = ((hue < HUE_THRESHOLD && hue > 20) && saturation > SATURATION_THRESHOLD);
        is_red = ((hue > HUE_THRESHOLD || hue < 20) && saturation > SATURATION_THRESHOLD);

        switch (optical_state){
            case DO_NOTHING: // default, does nothing
                break;
            case LOOK_FOR_YELLOW: // look for the yellowness of the base
                // if it's yellow specifically
                if (is_yellow) {
                    cout << "found yellow " << millis() << endl;
                    optical_state = LOOK_FOR_STICKER;
                }
                break;
            case LOOK_FOR_STICKER: //look for the sticker
                if (is_red){
                    cout << "found sticker " << millis() << endl;
                    optical_state = FUCK_GO_BACK; // mission complete
                }
                break;
            case FUCK_GO_BACK:
                // if it's not red (could be black or yellow)
                if (is_black){
                    cout << "went back " << millis() << endl;
                    optical_state = DO_NOTHING;
                }
        }

        delay(10);
    }
}

// to do: two modes: one for auton where we don't know we have the base.
// the other is for driver control when we do know we have it


#define PROXIMITY_THRESHOLD 244
#define STICKER_THRESHOLD 41
#include "main.h"
// #include <opencv2/highgui/highgui.hpp>

Optical optical(13);
double brightness = 0;
double hue = 0;
int32_t proximity = 0;
double saturation = 0;
pros::c::optical_rgb_s_t RGB_values;

bool has_base = false;
bool is_sticker = false;

// optical sensor readings and state changes
void set_optical(void* params){

    optical.disable_gesture();
    optical.set_led_pwm(100);

    while(true){

        brightness = optical.get_brightness();
        hue = optical.get_hue();
        saturation = optical.get_saturation();
        proximity = optical.get_proximity();
        RGB_values = optical.get_rgb();

        printf("Red value: %f \n", RGB_values.red);
        printf("Green value: %f \n", RGB_values.green);
        printf("Blue value: %f \n", RGB_values.blue);
        printf("Brightness value: %f \n", RGB_values.brightness);
        if (proximity > PROXIMITY_THRESHOLD) has_base = true;
        if (hue <=STICKER_THRESHOLD){
            is_sticker = false;
        }
        else{
            is_sticker = true;
        }
        delay(20);
    }
}
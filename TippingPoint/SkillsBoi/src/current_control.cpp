#include "main.h"

bool limit_current = false;
double average_temp = 0;
double average_current = 0;

void run_current_control(void* params) {

    int counter = 0;
    while (true) {
        counter++;

        if (counter % 10 == 0) {
            // cout << "\n\nML1: " << drive_left_1.get_current_draw() << "    " << drive_left_1.get_temperature() << endl;
            // cout << "ML2: " << drive_left_2.get_current_draw() << "    " << drive_left_2.get_temperature() << endl;
            // cout << "ML3: " << drive_left_3.get_current_draw() << "    " << drive_left_3.get_temperature() << endl;
            // cout << "ML4: " << drive_left_4.get_current_draw() << "    " << drive_left_4.get_temperature() << endl;

            // cout << "\nMR1: " << drive_right_1.get_current_draw() << "    " << drive_right_1.get_temperature() << endl;
            // cout << "MR2: " << drive_right_2.get_current_draw() << "    " << drive_right_2.get_temperature() << endl;
            // cout << "MR3: " << drive_right_3.get_current_draw() << "    " << drive_right_3.get_temperature() << endl;
            // cout << "MR4: " << drive_right_4.get_current_draw() << "    " << drive_right_4.get_temperature() << endl;
            
            average_current = (drive_left_1.get_current_draw() +
                                     drive_left_2.get_current_draw() + 
                                     drive_left_3.get_current_draw() + 
                                     drive_left_4.get_current_draw() + 
                                     drive_right_1.get_current_draw() + 
                                     drive_right_2.get_current_draw() + 
                                     drive_right_3.get_current_draw() + 
                                     drive_right_4.get_current_draw()); 

            average_temp = (drive_left_1.get_temperature() +
                                     drive_left_2.get_temperature() + 
                                     drive_left_3.get_temperature() + 
                                     drive_left_4.get_temperature() + 
                                     drive_right_1.get_temperature() + 
                                     drive_right_2.get_temperature() + 
                                     drive_right_3.get_temperature() + 
                                     drive_right_4.get_temperature()) / 8.0; 
            // cout << "\nAvg: " << average_current <<  "    " << average_temp << endl;

        }

        drive_right_1.set_current_limit(2500);        
        drive_right_2.set_current_limit(2500);
        drive_right_3.set_current_limit(2500);
        drive_right_4.set_current_limit(2500);
        
        drive_left_1.set_current_limit(2500);
        drive_left_2.set_current_limit(2500);
        drive_left_3.set_current_limit(2500);
        drive_left_4.set_current_limit(2500);

        if (limit_current) {
            back_lift_right.set_current_limit(100);
            back_lift_left.set_current_limit(100);
            front_lift_right.set_current_limit(100);
            front_lift_left.set_current_limit(100);

            side_lift.set_current_limit(100);

            intake_right.set_current_limit(100);
            intake_left.set_current_limit(100);
            intake_wobble_left.set_current_limit(100);
            intake_wobble_right.set_current_limit(100);
        }
        else {
            back_lift_right.set_current_limit(2500);
            back_lift_left.set_current_limit(2500);
            front_lift_right.set_current_limit(2500);
            front_lift_left.set_current_limit(2500);

            side_lift.set_current_limit(2500);

            intake_right.set_current_limit(2500);
            intake_left.set_current_limit(2500);
            intake_wobble_left.set_current_limit(2500);
            intake_wobble_right.set_current_limit(2500);
        }
        
        pros::delay(100);
    }
}

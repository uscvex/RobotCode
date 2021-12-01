#include "main.h"


double* next_entry = NULL;


double process_entry() {
    double result = *next_entry;
    next_entry++;
    return result;
}


void autonomous() {
    bool next_command = true;

    while (true) {



        delay(20);
    }

}
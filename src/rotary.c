// https://scienceprog.com/interfacing-rotary-encoder-to-avr-microcontroller/
#include "rotary.h"

uint8_t rotary_status;
uint8_t rotary_counter;

void init_rotary(void) {
    // ROTDDR &= ~(_BV(ROTPA)) & ~(_BV(ROTPB)) & ~(_BV(ROTBUTTON)); // Set pins as input
    ROTDDR &= ~(_BV(ROTPA) | _BV(ROTPB) | _BV(ROTBUTTON)); // Set pins as input
    ROTPORT |= _BV(ROTPA) | _BV(ROTPB) | _BV(ROTBUTTON); // pull-up pin
}

void rotary_check_status(void) {
    if (ROTA & (!ROTB)) {
        loop_until_bit_is_set(ROTPIN, ROTPA);
        if (ROTB) {
            rotary_counter--;
        }
    } else if (ROTB & (!ROTA)) {
        loop_until_bit_is_set(ROTPIN, ROTPB);
        if (ROTA) {
            rotary_counter++;
        }
    } else if (ROTA & ROTB) {
        loop_until_bit_is_set(ROTPIN, ROTPA);
        if (ROTB) {
            rotary_counter--;
        } else {
            rotary_counter++;
        }
    }
    if (ROTCLICK) {
        loop_until_bit_is_set(ROTPIN, ROTBUTTON);
        rotary_status = 3;
    }

}

uint8_t rotary_get_status(void) {
    return rotary_status;
}

uint8_t rotary_get_counter(void) {
    return rotary_counter;
}

void rotary_reset_status(void) {
    rotary_status = 0;
}

void rotary_reset_counter(void) {
    rotary_counter = 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <math.h>

#include "uart.h"
#include "defines.h"
#include "rotary.h"
#include "max7219.h"

FILE uart_str = FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW);

uint8_t rotary_status;

void Timer0_Start(void)
{
    TCCR0B |= _BV(CS02); // prescaler 256 ~122 interrupts/s
    TIMSK0 |= _BV(TOIE0); // Enable Timer0 Overflow interrupts

    sei(); // Set global interrupts
}

void button_init(void) {
    DDRD &= ~(_BV(PD3)); // set as input
    PORTD |= _BV(PD3); // pullup
}

void calculate_fstops(double seconds, double interval, double *array) {
    // interval = -3;
    // for(int i = 0; i < size; ++i) {
    //     array[i] = seconds * pow(2, (interval + i) * interval
    // }
    array[0] = seconds * pow(2, -3 * interval);
    array[1] = seconds * pow(2, -2 * interval);
    array[2] = seconds * pow(2, -1 * interval);
    array[3] = seconds * pow(2, 0 * interval);
    array[4] = seconds * pow(2, 1 * interval);
    array[5] = seconds * pow(2, 2 * interval);
    array[6] = seconds * pow(2, 3 * interval);
}


void counter_start(int seconds)
{   
    int num = seconds;
    seconds *= 10;
    while (seconds >= 0) {
        MAX7219_displayNumber(seconds);
        _delay_ms(100);
        seconds--;
    }

    MAX7219_displayNumber(num * 10);
}

int main()
{
    init_rotary();
    Timer0_Start();
    uart_init(MYUBRR);
    rotary_reset_status();
    spiMasterInit();
    button_init();

    // Decode mode to "Font Code-B"
    MAX7219_writeData(MAX7219_MODE_DECODE, 0xFF);

    // Scan limit runs from 0.
    MAX7219_writeData(MAX7219_MODE_SCAN_LIMIT, DIGITS_IN_USE - 1);
    MAX7219_writeData(MAX7219_MODE_INTENSITY, 4);
    MAX7219_writeData(MAX7219_MODE_POWER, ON);

    uint8_t rotary_counter, rotary_counter_last = rotary_get_counter();
    uint8_t rotary_status = rotary_get_status();

    stdout = &uart_str;

    fprintf(stdout, "Hello World!\n");

    double arr[6] ={0.0};
    calculate_fstops(5, 0.5, arr);

    MAX7219_displayNumber(arr[5] * 10);

    while (1)
    {
        rotary_counter = rotary_get_counter();
        rotary_status = rotary_get_status();


        if (rotary_counter != rotary_counter_last) { 
            fprintf(stdout, "Counter: %d | Status: %d\n", rotary_counter, rotary_status);
            MAX7219_displayNumber((int) rotary_counter * 10);
            // MAX7219_displayNumber(rotary_counter);
            rotary_counter_last = rotary_counter;
        }

        if (rotary_status == 3) {
            fprintf(stdout, "BUTTON CLICKED!\n");
            counter_start(rotary_counter);
            rotary_reset_status();
        }

        if (!(PIND & _BV(PIND3))) {
            counter_start(rotary_counter);
        }
        
    }

    return 0;
}

ISR(TIMER0_OVF_vect)
{
    // reading rotary and button
    rotary_check_status();
}

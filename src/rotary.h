#include <avr/io.h>

#define ROTPORT PORTD
#define ROTDDR DDRD
#define ROTPIN PIND

#define ROTPA PD7
#define ROTPB PD6
#define ROTBUTTON PD5

#define ROTA !(ROTPIN & _BV(ROTPA))
#define ROTB !(ROTPIN & _BV(ROTPB))
#define ROTCLICK !(ROTPIN & _BV(ROTBUTTON))

void init_rotary(void);
void rotary_check_status(void);
uint8_t rotary_get_status(void);
uint8_t rotary_get_counter(void);
void rotary_reset_status(void);
void rotary_reset_counter(void);

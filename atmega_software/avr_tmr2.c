/*
avr_tmr2.c

Provide functions to set up timer 2 hardware.

Copyright (C) 2021 Henrik Bjorkman www.eit.se/hb.

This file is free software; you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License version 2.1.

Removing this comment or the history section is not allowed.
If you modify this code make a note about it in the history
section below. That is required!

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

History

2021-04-20 created using code from avr_tmr0.c Henrik Bjorkman

*/

/*
When reading 16bit reg remember to read low byte first then high byte
when writing write high byte first then low byte but hopefully the C compiler does this.
For a list of available signals see:
http://www.nongnu.org/avr-libc/user-manual/index.html
*/


#include <avr/interrupt.h>
//#include <avr/signal.h>
#include <avr/pgmspace.h>

#include "avr_cfg.h"
#include "avr_tmr2.h"


/*
Clock select
0 No clock (stop)
1 No prescaling
2 clk/8
3 clk/32
4 clk/64
5 clk/128
6 clk/256
7 clk/1024.
*/
#define TMR2_PRESCALER_Stop 0
#define TMR2_PRESCALER_1 1
#define TMR2_PRESCALER_8 2
#define TMR2_PRESCALER_32 3
#define TMR2_PRESCALER_64 4
#define TMR2_PRESCALER_128 5
#define TMR2_PRESCALER_256 6
#define TMR2_PRESCALER_1024 7


#ifdef AVR_SYS_USE_TMR2


// local variables and defines, use 64 bit here to avoid issues with wrapping counters.
volatile int64_t timer2count;

// SIG_OVERFLOW0 renamed to TIMER0_OVF_vect according to:
// http://www.nongnu.org/avr-libc/user-manual/group__avr__interrupts.html

SIGNAL (TIMER2_OVF_vect)
{
  timer2count++;
}

SIGNAL (TIMER2_COMPA_vect)
{
	  timer2count++;
}

int64_t avr_tmr2_get_tick_64(void)
{
	int64_t tmp;
	TIMSK2&=(unsigned char)~(1<<TOIE2); // disable interrupt, I hope
	tmp=timer2count;
	TIMSK2|=(1<<TOIE2); // enable interrupt
	return tmp;
}

// If only 16 bits are needed use this one instead of avr_tmr0_get_tick_64, it will be faster.
int16_t avr_tmr2_get_tick_16(void)
{
	int16_t tmp;
	TIMSK2&=(unsigned char)~(1<<TOIE2); // disable interrupt, I hope
	tmp=timer2count;
	TIMSK2|=(1<<TOIE2); // enable interrupt
	return tmp;
}

// set up hardware (port directions, registers etc.)
void avr_tmr2_init(void)
{
	// set up timer 2

	// No output from this timer so COM2A and COM2B shall remain zero.

	#if (AVR_TMR2_DIVIDER == (1024L*256L))

	// Waveform Generation Mode 0 (default)
	TCCR2B |= (TMR2_PRESCALER_1024<<CS20);

	#else

	// Wanted do do it like this but it does not work.
	// Waveform Generation Mode
	// Clear Timer on Compare Match (CTC) mode 2
	//const uint8_t wgm = 2;
	//TCCR2A |= ((wgm&0x3) << WGM20);
	//TCCR2B |= (((wgm>>2)&1)<<WGM22);

	TCCR2A=(1<<WGM21); // set CTC mode (2)


	#if (AVR_TMR2_DIVIDER == (1024L*125L))
	// This will give 125 ticks per second with 16 MHz clock
	OCR2A=124;
	TCCR2B = (TMR2_PRESCALER_1024<<CS20);
	#elif (AVR_TMR2_DIVIDER == (256L*125L))
	// This will give 500 ticks per second with 16 MHz clock
	OCR2A=124;
	TCCR2B = (TMR2_PRESCALER_256<<CS20);
    #elif (AVR_TMR2_DIVIDER == (128L*125L))
	// This will give 1000 ticks per second with 16 MHz clock
	OCR2A=124;
	TCCR2B=(TMR2_PRESCALER_128<<CS20);
    #elif (AVR_TMR2_DIVIDER == (64L*125L))
	// This will give 1000 ticks per second with 8 MHz clock
	OCR2A=124;
	TCCR2B = (TMR2_PRESCALER_64<<CS20);
	#else
	#error DIVIDER combo not yet implemented.
	#endif
	#endif



	//TIMSK2|=(1<<TOIE2); // enable interrupt

	TIMSK2|=(1<<OCIE2A); // Timer/Counter2 Output Compare Match Interrupt Enable
}



#else


void avr_tmr2_pwm_on(void)
{
    /*
    Clock select
    0 No clock (stop)
    1 No prescaling
    2 clk/8
    3 clk/32
    4 clk/64
    5 clk/128
    6 clk/256
    7 clk/1024.
    */
    // set prescaler 8 and start

    uint8_t cs = 2;
    TCCR2B |= (cs << CS20);
}

void avr_tmr2_pwm_off(void)
{
	// stop
	TCCR2B &= ~(7 << CS20);
}

// set up hardware (port directions, registers etc.)
// set up timer 1
void avr_tmr2_init(void)
{
    // Set up timer1 so it can give 1 KHz or 2 on PD5 & PD6
    // Current settings below gave 7.81 KHz

    // set pins OC2A & OC2B to output
    DDRB |= (1 << DDB3);
    DDRD |= (1 << DDD3);

    // 50%
    OCR2A = 128;

    // 25%
    OCR2B = 64;

    /*
    Compare Output Mode, Fast PWM Mode
    0  Normal port operation, OC0A disconnected.
    1  Toggle...
    2  Clear OC2A on Compare Match, set OC2A at BOTTOM,(non-inverting mode).
    3  Set OC2A on Compare Match, clear OC2A at BOTTOM,(inverting mode).
    */
    uint8_t com_a = 2; // not inverting mode
    uint8_t com_b = 3; // inverting mode
    // Waveform Generation Mode: Fast PWM mode
    uint8_t wgm = 3;
    TCCR2A |= (com_a << COM2A0) | (com_b << COM2B0) | (wgm << WGM20);

    avr_tmr2_pwm_on();
}

#endif

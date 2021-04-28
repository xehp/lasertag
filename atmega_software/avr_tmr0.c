/*
avr_tmr0.c 

Provide functions to set up timer 0 hardware

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

2005-07-02 created using code from avr_misc.c. /Henrik Bjorkman

*/

/*
When reading 16bit reg remember to read low byte first then high byte
when writing write high byte first then low byte but hopefully the C compiler does this.
For a list ov available signals see:
http://www.nongnu.org/avr-libc/user-manual/index.html
*/


// includes

#include <avr/interrupt.h>
//#include <avr/signal.h>
#include <avr/pgmspace.h>
#include <stdint.h>

#include "avr_cfg.h"
#include "avr_tmr0.h"

#define TMR2_PRESCALER_Stop 0
#define TMR2_PRESCALER_1 1
#define TMR2_PRESCALER_8 2
#define TMR2_PRESCALER_64 3
#define TMR2_PRESCALER_256 4
#define TMR2_PRESCALER_1024 5


#ifdef AVR_SYS_USE_TMR0

// local variables and defines, use 64 bit here to avoid issues with wrapping counters.
volatile int64_t timer0count;

// SIG_OVERFLOW0 renamed to TIMER0_OVF_vect according to:
// http://www.nongnu.org/avr-libc/user-manual/group__avr__interrupts.html

SIGNAL (TIMER0_OVF_vect)
{
  timer0count++;
}

int64_t avr_tmr0_get_tick_64(void)
{
	int64_t tmp;
	TIMSK0&=(unsigned char)~(1<<TOIE0); // disable interrupt, I hope
	tmp=timer0count;
	TIMSK0|=(1<<TOIE0); // enable interrupt
	return tmp;
}

// If only 16 bits are needed use this one instead of avr_tmr0_get_tick_64, it will be faster.
int16_t avr_tmr0_get_tick_16(void)
{
	int16_t tmp;
	TIMSK0&=(unsigned char)~(1<<TOIE0); // disable interrupt, I hope
	tmp=timer0count;
	TIMSK0|=(1<<TOIE0); // enable interrupt
	return tmp;
}

// set up hardware (port directions, registers etc.)
void avr_tmr0_init(void) 
{
	// set up timer 0
	#if AVR_TMR0_DIVIDER == (1024L*256L)
	TCCR0B=0x05; // internal clock with prescaler to 1024 gives interrupt frequency=AVR_FOSC/(1024*256)
	TIMSK0|=(1<<TOIE0); // enable interrupt
	#else
	#error
	#endif
}


#elif defined BEEP_USE_TMR0


// tone:
// 4 : 244 Hz
// 5 : 976 Hz
void avr_tmr0_pwm_on(uint8_t tone)
{
    // Set up timer1 so it can give 1 KHz or 2 on PD5 & PD6
    // Current settings below gave 7.81 KHz

    // First turn it off (might not be needed but anyway).
    avr_tmr0_pwm_off();

    // 50%
    OCR0A = 128;
    OCR0B = 128;

    /*
    Compare Output Mode, Fast PWM Mode
    0  Normal port operation, OC0A/B disconnected.
    1  -
    2  Clear OC0A/B on Compare Match, set OC0A/B at BOTTOM,(non-inverting mode).
    3  Set OC0A/B on Compare Match, clear OC0A/B at BOTTOM,(inverting mode).
    */
    const uint8_t com_a = 2; // not inverting mode
    const uint8_t com_b = 3; // inverting mode

    // Waveform Generation Mode: Fast PWM mode
	const uint8_t wgm = 3;

	// Set output mode for OC0A & OC0B
    TCCR0A |= (com_a << COM0A0) | (com_b << COM0B0) | ((wgm&0x3) << WGM00);

	// set prescaler 256 and start
	TCCR0B &= ~(7 << CS00);
    TCCR0B |= (((wgm>>2)&1)<<WGM02) | (tone << CS20);
}

void avr_tmr0_pwm_off(void)
{

    /*
    Compare Output Mode, Fast PWM Mode
    0  Normal port operation, OC0A/B disconnected.
    1  -
    2  Clear OC0A/B on Compare Match, set OC0A/B at BOTTOM,(non-inverting mode).
    3  Set OC0A/B on Compare Match, clear OC0A/B at BOTTOM,(inverting mode).
    */
	// Disable output from OC0A and OC0B
    TCCR0A &= ~((3 << COM0A0) | (3 << COM0B0));

	// stop
	TCCR0B &= ~(7 << CS00);
}

// set up hardware (port directions, registers etc.)
// set up timer 1
void avr_tmr0_init(void)
{
    // set pins OC0A & OC0B to outputs PD6 & PD5 respectively
	// Those are pins 12 and 11 on 28 pin ATMEGAS
	// These outputs are active when they differ so using high for both as inactive.
    PORTD |= (1 << DDD6) | (1 << DDD5);
    DDRD |= (1 << DDD6) | (1 << DDD5);
}

// TODO Adjust frequency.
// We want something from 300 to 5000 Hz.
// f= FOSC / (prescaler * (top+1))
// currently using 0xFF as top.
// So we should be getting 16000000/(256*256) -> 244 Hz

#elif defined IR_OUTPUT_USE_TMR0

void avr_tmr0_pwm_off(void)
{

    /*
    Compare Output Mode, Fast PWM Mode
    0  Normal port operation, OC0A/B disconnected.
    1  -
    2  Clear OC0A/B on Compare Match, set OC0A/B at BOTTOM,(non-inverting mode).
    3  Set OC0A/B on Compare Match, clear OC0A/B at BOTTOM,(inverting mode).
    */
	// Disable output from OC0A (and OC0B while at it)
    TCCR0A &= ~((3 << COM0A0) | (3 << COM0B0));

	// stop
	TCCR0B &= ~(7 << CS00);

    PORTD &= ~(1 << PD6);
}

// tone:
// 4 : 244 Hz
// 5 : 976 Hz
void avr_tmr0_pwm_on(uint8_t tone)
{
    // Set up timer1 so it can give 1 KHz or 2 on PD5 & PD6
    // Current settings below gave 7.81 KHz

	// First turn it off (might not be needed but anyway).
	avr_tmr0_pwm_off();


    // 50%
    OCR0A = 128;
    OCR0B = 128;

    /*
    Compare Output Mode, Fast PWM Mode
    0  Normal port operation, OC0A/B disconnected.
    1  -
    2  Clear OC0A/B on Compare Match, set OC0A/B at BOTTOM,(non-inverting mode).
    3  Set OC0A/B on Compare Match, clear OC0A/B at BOTTOM,(inverting mode).
    */
    const uint8_t com_a = 2; // not inverting mode
    const uint8_t com_b = 0; // not used for now

    // Waveform Generation Mode: Fast PWM mode
	const uint8_t wgm = 3;

	// Set output mode for OC0A & OC0B
    TCCR0A |= (com_a << COM0A0) | (com_b << COM0B0) | ((wgm&0x3) << WGM00);

	// set prescaler 256 and start
	TCCR0B &= ~(7 << CS00);
    TCCR0B |= (((wgm>>2)&1)<<WGM02) | (tone << CS20);
}


// set up hardware (port directions, registers etc.)
// set up timer 1
void avr_tmr0_init(void)
{
    // set pin OC0A to output PD6
	// That is pin 12 on 28 pin ATMEGAS
	// The output is active high so set it to zero.
    PORTD &= ~(1 << PD6);
    DDRD |= (1 << DDD6);
}





#else
// Timer 0 is not used
#endif



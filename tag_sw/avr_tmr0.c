/*
avr_tmr0.c 

Provide functions to set up timer 0 hardware

Copyright (C) 2021 Henrik Bjorkman www.eit.se/hb.

This file is free software; you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License version 2.1.

Removing this comment or the history section is not allowed. Even if only
a few lines from this file is actually used. If you modify this code make
a note about it in the history section below. That is required!

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
#include "avr_uart.h"
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

	// clear and set output mode for OC0A & OC0B
    TCCR0A &= ~((3 << COM0A0) | (3 << COM0B0) | (3 << WGM00));
    TCCR0A |= (com_a << COM0A0) | (com_b << COM0B0) | ((wgm&0x3) << WGM00);

	// clear and set prescaler 256 and start
	TCCR0B &= ~(1<<WGM02 | (7 << CS00));
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

	TCNT0 = 0;
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

    // 50%
    OCR0A = 128;
    OCR0B = 128;

}

// TODO Adjust frequency.
// We want something from 300 to 5000 Hz.
// f= FOSC / (prescaler * (top+1))
// currently using 0xFF as top.
// So we should be getting 16000000/(256*256) -> 244 Hz

#elif defined IR_OUTPUT_USE_TMR0

#ifdef IR_USE_OC0A_PD6

void avr_tmr0_pwm_off(void)
{
	// Output is active high so set it to low (zero/inactive)
	// This is only needed if we thing someone else also use PD5.
    //PORTD &= ~(1 << PD5);

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


	TCNT0 = 0;
}

// tone:
// 4 : 244 Hz
// 5 : 976 Hz
void avr_tmr0_pwm_on()
{
    // Set up timer1 so it can give 1 KHz or 2 on PD5 & PD6
    // Current settings below gave 7.81 KHz

    /*
    Compare Output Mode, Fast PWM Mode
    0  Normal port operation, OC0A/B disconnected.
    1  -
    2  Clear OC0A/B on Compare Match, set OC0A/B at BOTTOM,(non-inverting mode).
    3  Set OC0A/B on Compare Match, clear OC0A/B at BOTTOM,(inverting mode).
    */
    const uint8_t com_a = 1; // toggle OC0A on Compare Match
    const uint8_t com_b = 0; // not used in this case

    // Waveform Generation Mode: CTC
	const uint8_t wgm = 2;

	// No prescaling
	const uint8_t cs = 1;

	// CLear and set output mode for OC0A (& OC0B)
    TCCR0A &= ~((3 << COM0A0) | (3 << COM0B0) | (3 << WGM00));
    TCCR0A |= (com_a << COM0A0) | (com_b << COM0B0) | ((wgm&0x3) << WGM00);

	// Clear and set prescaler and start
	TCCR0B &= ~(1<<WGM02 | (7 << CS00));
    TCCR0B |= (((wgm>>2)&1)<<WGM02) | (cs << CS20);
}


// set up hardware (port directions, registers etc.)
// set up timer 1
void avr_tmr0_init(void)
{
	// Ref [1] 15.5.3 "The setup of the OC0x should be performed before setting the Data Direction Register for the port pin to output."

	// To get 38 KHz from 16 MHz
	// Since we use toggle we need double frequency, so 76 KHz
    OCR0A = 210; // Should give 38.1 KHz
    OCR0B = OCR0A/2; // Not used probably.

	// set pin OC0A to output PD6
	// That is pin 12 on 28 pin ATMEGAS
	// Output is active high so set it to low (zero/inactive)
    PORTD &= ~(1 << PD6);
    DDRD |= (1 << DDD6);
}



#elif defined IR_USE_OC0B_PD5

// Define one of these (not both)
//#define FAST_PWM
#define PHASE_PWM

void avr_tmr0_pwm_off(void)
{
	// Output is active high so set it to low (zero/inactive)
	// This is only needed if we think someone else also use PD5.
    //PORTD &= ~(1 << PD5);

    /*
    Compare Output Mode, Fast PWM Mode
    0  Normal port operation, OC0A/B disconnected.
    1  -
    2  Clear OC0A/B on Compare Match, set OC0A/B at BOTTOM,(non-inverting mode).
    3  Set OC0A/B on Compare Match, clear OC0A/B at BOTTOM,(inverting mode).
    */
	// Disable output from OC0B (and OC0A while at it)
    TCCR0A &= ~((3 << COM0A0) | (3 << COM0B0));

	// stop
	TCCR0B &= ~(7 << CS00);

	// reset counter also.
	TCNT0 = 0;
}

// tone:
// 4 : 244 Hz
// 5 : 976 Hz
void avr_tmr0_pwm_on()
{
    // Set up timer1 so can our 38 KHz on PD5.

    /*
    Compare Output Mode, Fast PWM Mode
    0  Normal port operation, OC0A/B disconnected.
    1  -
    2  Clear OC0A/B on Compare Match, set OC0A/B at BOTTOM,(non-inverting mode).
    3  Set OC0A/B on Compare Match, clear OC0A/B at BOTTOM,(inverting mode).
    */
    const uint8_t com_a = 0; // not used in this case
    const uint8_t com_b = 2; // non inverting

	#ifdef FAST_PWM
	const uint8_t wgm = 2; // Waveform Generation Mode: Fast PWM
	const uint8_t cs = 2; // 1/8 prescaling
	#elif defined PHASE_PWM
	const uint8_t wgm = 5; // Waveform Generation Mode: Phase correct PWM
	const uint8_t cs = 1; // no prescaling
	#else
	#error
	#endif


	// CLear and set output mode for OC0A & OC0B
    TCCR0A &= ~((3 << COM0A0) | (3 << COM0B0) | (3 << WGM00));
    TCCR0A |= (com_a << COM0A0) | (com_b << COM0B0) | ((wgm&0x3) << WGM00);

	// Clear and set prescaler and start
	TCCR0B &= ~(1<<WGM02 | (7 << CS00));
    TCCR0B |= (((wgm>>2)&1)<<WGM02) | (cs << CS20);
}

void avr_tmr0_set_signal_strength(uint8_t signal_strength_percent)
{
	OCR0B = (((uint16_t)signal_strength_percent)*((uint16_t)OCR0A))/200;
}

// set up hardware (port directions, registers etc.)
// set up timer 1
void avr_tmr0_init(uint8_t signal_strength_percent)
{
	// Ref [1] 15.5.3 "The setup of the OC0x should be performed before setting the Data Direction Register for the port pin to output."

	// To get 38 KHz from 16 MHz
	#ifdef FAST_PWM
		#if AVR_FOSC == 16000000L
		OCR0A = 52; // Should give 38.5 KHz with prescaler 8.
		#else
		#error
		#endif
	#elif defined PHASE_PWM
		// Fpwm = Fosc / ((OCR0A-1) * 2)
		#if AVR_FOSC == 16000000L
		OCR0A = 211; // Should give 38.1 KHz with prescaler 1 and 16 MHz.
		#elif AVR_FOSC == 12000000L
		// With 12 MHz OCR0A=159 should give 38.0 KHz out
		OCR0A = 159; // should give 38.0 KHz
		#elif AVR_FOSC == 8000000L
		// With 8 MHz OCR0A=106 should give 38.1 KHz out
		OCR0A = 106; // should give 38.1 KHz
		#elif AVR_FOSC == 7370000L
		// With 8 MHz OCR0A=106 should give 38.1 KHz out
		OCR0A = 98; // should give 38.0 KHz
		#else
		#error
		#endif
	#else
	#error
	#endif


	avr_tmr0_set_signal_strength(signal_strength_percent);

	// set pin OC0B to output PD5
	// That is pin 11 on 28 pin ATMEGAS
	#if HW_VERSION == 2
	// For HW 2021-05-27 use HW_VERSION 2 (wrong transistor was used)
	// Output is active low so set it to high (one)
    PORTD |= (1 << PD5);
    DDRD |= (1 << DDD5);
	#else
	// Output is active high so set it to low (zero/inactive)
    PORTD &= ~(1 << PD5);
    DDRD |= (1 << DDD5);
	#endif

	uart_print_P(PSTR("tmr0 "));
	uart_print_hex8(signal_strength_percent);
	uart_putchar(' ');
	uart_print_hex8(OCR0A);
	uart_print_P(PSTR("\r\n"));
}

#else
#error
#endif

#else
// Timer 0 is not used
#endif

uint8_t tmr0_get_OCR0B(void)
{
	return OCR0B;
}


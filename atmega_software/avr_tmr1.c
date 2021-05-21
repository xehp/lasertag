/*
avr_tmr1.c

Provide functions to set up timer 1 hardware.

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

2021-04-20 created using code from avr_tmr0.h Henrik Bjorkman

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
#include "avr_uart.h"
#include "avr_tmr1.h"

#ifdef IR_OUTPUT_USE_TMR1


// local variables and defines, use 64 bit here to avoid issues with wrapping counters.
volatile int64_t timer0count;


void avr_tmr1_pwm_on(void)
{
    // not inverting mode
    // Fast PWM mode using ICR1 as TOP
	/*
    Clock select
    0 No clock (stop)
    1 No prescaling
    2 clk/8
    3 clk/64
    4 clk/256
    5 clk/1024
    6 External source on falling edge.
    7 External source on rising edge.
    */
    //TCCR1A |= (1 << COM1A1) | (1 << COM1B1) | (1 << WGM11);
    TCCR1A |= (1 << COM1A1) | (1 << WGM11);
    TCCR1B |= (1 << WGM12) | (1 << WGM13) | (1 << CS10);
}

void avr_tmr1_pwm_off(void)
{
    // Stop timer and release PB1
    TCCR1A = 0;
	TCCR1B = 0;
	TCNT1 = 0;
}

// set up hardware (port directions, registers etc.)
// set up timer 1
void avr_tmr1_init(void)
{
	uart_print_P(PSTR("tmr1\r\n"));

	// Set up timer1 so it can give 38 KHz on OC1A (AKA PB1 pin 15 of 28)

	// Signal is active high so set PB1 low and then as output.
    PORTB &= ~(1 << DDB1);
    DDRB |= (1 << DDB1);

    // top
    // TODO this needs to be calculated to get the wanted 38 KHz.
    // First try (ICR1 = 0xFFFF) gave 244.1 Hz, 421 was spot on.
    // That is with 16 MHz FOSC. This needs to be adjusted if its not 16 MHz.
    ICR1 = 421;

    // 25%
    OCR1A = ICR1/4;

    // 75%
    OCR1B = (ICR1/4)*3;
}



#elif defined BEEP_USE_TMR1



void avr_tmr1_pwm_off(void)
{
	// this output is active high so set PB1 & PB2 to zero
    PORTB &= ~((1 << PB2) | (1 << PB1));

    // Stop timer and release PB1 & PB2
    TCCR1A = 0;
	TCCR1B = 0;
}

void avr_tmr1_pwm_on(uint16_t tone)
{
	// Set up timer1 so it can give 38 KHz on OC1A (AKA PB1 pin 15)

	// First clear TCCR1A & TCCR1B (turns it off)
    TCCR1A = 0;
	TCCR1B = 0;

	if (tone != 0)
	{
		// A not inverted, B inverted
		const uint8_t com1a = 2;
		const uint8_t com1b = 3;

		// Fast PWM mode using ICR1 as TOP
		const uint8_t wgm1 = 14;

		/*
		Clock select
		0 No clock (stop)
		1 No prescaling
		2 clk/8
		3 clk/64
		4 clk/256
		5 clk/1024
		6 External source on falling edge.
		7 External source on rising edge.
		*/
		const uint8_t cs = 1;

		// top
		ICR1 = tone;

		// PB1 to 50%
		OCR1A = ICR1/2;

		// PB2 to 50%
		OCR1B = ICR1/2;

		TCCR1A |= (com1a << COM1A0) | (com1b << COM1B0) | ((wgm1 & 3) << WGM10);
		TCCR1B |= (((wgm1 >> 2) & 3) << WGM12) | (cs << CS10);
	}
}


// set up hardware (port directions, registers etc.)
void avr_tmr1_init(void)
{
	uart_print_P(PSTR("tmr1\r\n"));

	// PB1 & PB2 low and as output
    PORTB &= ~((1 << PB2) | (1 << PB1));
    DDRB |= ((1 << DDB2) | (1 << DDB1));
}

#endif

/*
avr_misc.c 

provide functions to set up the general basics of the system.

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

For a list ov available signals see:
http://www.nongnu.org/avr-libc/user-manual/index.html

History

2005-02-16 Created. Henrik Bjorkman
2005-02-20 Set up timer0 interrupt /Henrik Bjorkman
2005-02-22 Using timer1 to generate 38 MHz. Will need that to send IR later. /Henrik Bjorkman
2005-02-22 Using timer2 to get a timer with a 1 second resolution Henrik Bjorkman
2005-02-22 Will try to use timer2 interrupt to make a software uart together with timer1
2005-07-01 cleanup Henrik
2005-07-02 Moved timer drivers to own files. Henrik

*/

// When reading 16bit reg remember to read low byte first then high byte
// when writing write high byte first then low byte but hopefully the C compiler does this.


// includes

#include <avr/interrupt.h>
//#include <avr/signal.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include "avr_cfg.h"
#include "avr_ports.h"
#include "avr_uart.h"
#ifdef AVR_SYS_USE_TMR0
#include "avr_tmr0.h"
#endif
#ifdef AVR_SYS_USE_TMR2
#include "avr_tmr2.h"
#endif
#include "avr_sys.h"

#if ((defined(__AVR_ATmega88__)) || (defined(__AVR_ATmega168__)) || (defined(__AVR_ATmega328P__)))
// OK
#else
#warning // It should work with other atmega mcus to but that is not tested.
#endif


#if defined AVR_SYS_USE_TMR0 && defined AVR_SYS_USE_TMR2
#error
#endif


int16_t power_wtd = 0;



// Flash n times with the LED, just to see that CPU is running before doing anything else.
// This may be called before debug_init has been called.
int avr_blink_debug_led(uint8_t n)
{
#ifdef DEBUG_LED_PORT
	avr_delay_ms_16(200);

	/* INITIALIZE */

	/* set output to off */
	DEBUG_LED_OFF();

	/* enable PC5 as output */
	DEBUG_LED_ENABLE();

	/* BLINK, BLINK ... */

	while (n>0)
	{
		/* set LED on */
		DEBUG_LED_ON();

		avr_delay_ms_16(200);

		/* set LED off */
		DEBUG_LED_OFF();

		avr_delay_ms_16(400);

		n--;
	}

	/* disable debug LED as output */
	DEBUG_LED_DISABLE();

	avr_delay_ms_16(800);
#endif
	return 0;
}



// This function will set CPU in idle mode, saving energy.
// Make sure timer 2 (or timer 0) interrupts are started before this is called.
void avr_wtd_reset_and_idle(void)
{
	// Only reset HW watchdog if the power watchdog is not timed out.
	// That way if power stop calling avr_wtd_reset_power we get a reset.
	if (power_wtd != 0)
	{
		wdt_reset();
		power_wtd--;
	}
	else
	{
		RELAY_OFF();
	}


	// enable sleep
	SMCR|=(1<<SE);

	// if needed, reduce master clock speed here
	// using XDIV register

	// if needed, turn of current for analog comparator here

	// set sleep mode to idle
	SMCR&=~(7<<SM0);
	//MCUCR|=(0<<SM0);

	// Here we go to sleep
	asm ("sleep");

	// we continue here after being wakened up by an interrupt.

	// reset clock speed to normal value
	// using XDIV register

	// turn on current for analog comparator here (if we turned it off)

	// disable sleep
	SMCR&=~(1<<SE);
}

// Only power (battery charging supervision) is allowed to call this reset.
void avr_wtd_reset_power(void)
{
	power_wtd=-1;
	avr_wtd_reset_and_idle();
}


void avr_delay_ms_16(int delay_ms)
{
#ifdef AVR_SYS_USE_TMR0
	// Translate into timer0 ticks
	const int16_t ticks_to_delay = AVR_TMR0_TRANSLATE_MS_TO_TICKS_16(delay_ms);
	const int16_t tick_target=avr_tmr0_get_tick_16()+ticks_to_delay;
	while((tick_target-avr_tmr0_get_tick_16())>=0)
	{
		// wait
		avr_wtd_reset();
		avr_idle();
	}
#elif (defined AVR_SYS_USE_TMR2) && (AVR_TMR2_TICKS_PER_SEC == 1000)
	// Translate into timer2 ticks
	const int16_t ticks_to_delay = delay_ms;
	const int16_t tick_target = avr_tmr2_get_tick_16() + ticks_to_delay;
	while((tick_target-avr_tmr2_get_tick_16())>=0)
	{
		// wait
		avr_wtd_reset_and_idle();
	}
#elif defined AVR_SYS_USE_TMR2
	// Translate into timer2 ticks
	const int16_t ticks_to_delay = AVR_TMR2_TRANSLATE_MS_TO_TICKS_16(delay_ms);
	const int16_t tick_target=avr_tmr2_get_tick_16()+ticks_to_delay;
	while((tick_target-avr_tmr2_get_tick_16())>=0)
	{
		// wait
		avr_wtd_reset_and_idle();
	}
#elif 1
void avr_delay_ms_16(int delay_ms)
{
	int i;
	while(delay_ms>0)
	{
		for (i=0; i<(AVR_FOSC/6000L); i++)
		{
			avr_wtd_reset_and_idle(); // If wdt_reset isn't wanted, put a no OP here instead: asm ("nop");
		}
		delay_ms--;
	}
}
#else
// alternative function using the avr_delay_us below
void avr_delay_ms_16(int delay_ms)
{
	while(delay_ms>0)
	{
		AVR_DELAY_US(1000);
		delay_ms--;
	}
}
#endif
}


/**
 * Subtract about 10 us just for the time calling this will take.
 * For better precision if making really short pulses disable interrupt when using this.
 * cli(); // disable global interrupts
 * sei(); // enable global interrupts
 */
void avr_delay_us(int delay_us)
{
	if (delay_us==0)
	{
		return;
	}
	else
	{
		delay_us--;
		while(delay_us>0)
		{
			#if (AVR_FOSC<6000000L)
			#elif (AVR_FOSC<9000000L)
			// for 8 MHz
			asm ("nop");
			#elif AVR_FOSC<11000000L
			// for 10 MHz
			asm ("nop");
			asm ("nop");
			asm ("nop");
			#elif AVR_FOSC<14000000L
			// for 12 MHz
			asm ("nop");
			asm ("nop");
			asm ("nop");
			asm ("nop");
			asm ("nop");
			#elif AVR_FOSC<18000000L
			// for 16 MHz
			asm ("nop");
			asm ("nop");
			asm ("nop");
			asm ("nop");
			asm ("nop");
			asm ("nop");
			asm ("nop");
			asm ("nop");
			asm ("nop");
			#elif AVR_FOSC==20000000L
			// for 20 MHz
			asm ("nop");
			asm ("nop");
			asm ("nop");
			asm ("nop");
			asm ("nop");
			asm ("nop");
			asm ("nop");
			asm ("nop");
			asm ("nop");
			asm ("nop");
			asm ("nop");
			asm ("nop");
			asm ("nop");
			#else
			#error selected AVR_FOSC not (yet) supported by delay_us
			#endif
			delay_us--;
		}
	}
}

void avr_error_handler_P(const char *pgm_addr, uint16_t errorCode)
{
	// Remember to turn things off that must be turned of in case of error.
	// That is application specific code may have to be entered here.

	for(;;)
	{
		uart_print_crlf();
		uart_print_P(pgm_addr);
		uart_putchar(' ');
		uart_print_hex16(errorCode);
		uart_print_crlf();
		avr_blink_debug_led(8);
		avr_delay_ms_16(1000);
		RELAY_OFF();
	}
}


// set up hardware (port directions, registers etc.)
void avr_init() 
{
	wdt_enable(WDTO_2S);

	// Analog comparator
	ACSR|=1<<ACD; // disable analog comparator to save power

	// enable global interrupts
	sei();

#ifdef AVR_SYS_USE_TMR0
	avr_tmr0_init();
#elif defined AVR_SYS_USE_TMR2
	avr_tmr2_init();
#else
#error
#endif

	RELAY_ON();
	RELAY_ENABLE();
	power_wtd=-1;
	avr_wtd_reset_and_idle();
}

int64_t avr_systime_ms_64(void)
{
#ifdef AVR_SYS_USE_TMR0
	return AVR_TMR0_TRANSLATE_TICKS_TO_MS_64(avr_tmr0_get_tick_64());
#elif (defined AVR_SYS_USE_TMR2) && (AVR_TMR2_TICKS_PER_SEC == 1000)
	return avr_tmr2_get_tick_64();
#elif defined AVR_SYS_USE_TMR2
	return AVR_TMR2_TRANSLATE_TICKS_TO_MS_64(avr_tmr2_get_tick_64());
#else
#error
#endif
}

int16_t avr_systime_ms_16(void)
{
#if (defined AVR_SYS_USE_TMR0) && (AVR_TMR0_TICKS_PER_SEC == 1000)
	return avr_tmr0_get_tick_16();
#elif (defined AVR_SYS_USE_TMR2) && (AVR_TMR2_TICKS_PER_SEC == 1000)
	return avr_tmr2_get_tick_16();
#elif defined AVR_SYS_USE_TMR0
	return AVR_TMR0_TRANSLATE_TICKS_TO_MS_16(avr_tmr0_get_tick_64());
#elif defined AVR_SYS_USE_TMR2
	return AVR_TMR2_TRANSLATE_TICKS_TO_MS_16(avr_tmr2_get_tick_64());
#else
#error
#endif
}

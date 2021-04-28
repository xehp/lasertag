/*
main.c

Henriks AVR application

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

2005-02-14 want to use serial port. Have adapted some code I found at http://www.lka.ch/projects/avr/ Henrik Bjorkman
2005-02-20 Will try to interpret some commands Henrik

*/


#include <avr/wdt.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "avr_cfg.h"
#include "avr_sys.h"
#include "avr_uart.h"
#include "avr_ports.h"
#include "avr_tmr0.h"
#include "avr_tmr1.h"
#include "game.h"
#include "power.h"
#include "beep.h"
#include "radio.h"
#include "version.h"


const uint32_t ignore_mask = 0x70;

int16_t main_timer_ms=0;
int32_t main_counter_s=0;
int8_t main_state=0;
uint32_t log_pin = 0;



int main( void ) 
{
	wdt_enable(WDTO_2S);

	// Initiate system timer. The one that give us milli seconds.
	avr_init();
	wdt_reset();

	// Initiate USART (serial port).
	uart_init();
	UART_PRINT_P("\r\n" VERSION_STRING "\r\n");
	avr_delay_ms_16(200);
	wdt_reset();

	// Initiate all sub tasks here.

	#ifdef DEBUG_LED_PORT
	avr_blink(3); // just so we see that it started
	wdt_reset();
	#endif

	avr_tmr0_init();
	avr_tmr1_init();

	HIT_LEDS_ENABLE();
	LASER_ENABLE();
	VIB_ENABLE();
	TRIGGER_INIT();
	EXTERNAL_IR_INIT();
	INTERNAL_IR_INIT();


	game_init();
	avr_delay_ms_16(200);
	wdt_reset();

	power_init();
	avr_delay_ms_16(200);
	wdt_reset();


	beep_init();
	avr_delay_ms_16(200);
	wdt_reset();


	radio_init();
	avr_delay_ms_16(200);
	wdt_reset();


	// All init is done, get ready to run main loop.
	UART_PRINT_P("main\r\n");
	main_timer_ms = avr_systime_ms_16() + 1000;

	// The main loop, this will run until power off.
	for(;;)
	{
		// Put all urgent tasks here. Things that need to be called more than once per second.

		int ch = uart_getchar();
		if (ch >= 0)
		{
			// Just echo input. Change small letters to capital just to make
			// it obvious that this feature works.
			if (ch == 0x1b)
			{
				// esc, do eternal loop until we get reset by WTD.
				for(;;);
			}
			else if ((ch>='a') && (ch<='z'))
			{
				ch -= 'a' - 'A';
			}
			uart_putchar(ch);
		}

		radio_process();

		game_process();

		switch(main_state)
		{
			case 0:
			{
				// Check if we reached next second.
				const int16_t t = avr_systime_ms_16();
				const int16_t d = t - main_timer_ms;
				if (d >= 0)
				{
					// Start a new sequence of calls to second ticks.
					// Not doing all at once since that might disturb more urgent tasks.
					main_timer_ms += 1000;
					++main_state;
				}
				break;
			}

			// Put tasks to be made once per second or less here.
			// Just as cases as needed. Just don't forget to do "++main_state".
			case 1:
			{
				// log digital inputs
				uint32_t b = PINB;
				uint32_t c = PINC;
				uint32_t d = PIND;
				uint32_t p = (b << 16) | (c << 8) | (d);
				if ((log_pin | ignore_mask) != (p | ignore_mask))
				{
					UART_PRINT_P("pin ");
					uart_print_hex32(p);
					UART_PRINT_P("\r\n");
					log_pin = p;
				}
				++main_state;
				break;
			}
			case 2:
				power_tick_s();
				++main_state;
				break;
			case 3:
				beep_tick_s();
				++main_state;
				break;
			case 4:
				game_tick_s();
				++main_state;
				break;
			default:
			    // All tasks to do once per second are done for this time.
				++main_counter_s;
				main_state=0;
				break;
		}

		wdt_reset();

		// Set CPU in idle mode to save energy, it will wake up next time there is an interrupt
		avr_idle();
	}



	return(0);
} // end main()




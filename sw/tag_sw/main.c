/*
main.c

Henriks AVR application

Copyright (C) 2021 Henrik Bjorkman www.eit.se/hb.

This file is free software; you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License version 2.1.

Removing this comment or the history section is not allowed. Even if only
a few lines from this file is actually used. If you modify this code make
a note about it in the history section below. That is required!

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
General Public License for more details.

History

2005-02-14 want to use serial port. Have adapted some code I found at http://www.lka.ch/projects/avr/ Henrik Bjorkman
2005-02-20 Will try to interpret some commands Henrik

*/


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
#include "avr_eeprom.h"
#include "game.h"
#include "power.h"
#include "beep.h"
#include "radio.h"
#include "cmd.h"
#include "version.h"


//static const uint32_t ignore_mask = 0x00070077;

int16_t main_timer_ms=0;
int32_t main_counter_s=0;
int8_t main_state=0;
//static uint32_t log_pin = 0;

/*static void log_ports(void)
{
	const uint32_t b = PINB;
	const uint32_t c = PINC;
	const uint32_t d = PIND;
	const uint32_t p = (b << 16) | (c << 8) | (d);
	if ((log_pin | ignore_mask) != (p | ignore_mask))
	{
		UART_PRINT_P("pin ");
		uart_print_hex32(p);
		UART_PRINT_P("\r\n");
		log_pin = p;
	}
}*/

int main( void ) 
{
	// Initiate system timer. The one that give us milliseconds.
	avr_init();


	// Initiate USART (serial port).
	uart_init();
	uart_print_crlf();
	UART_PRINT_PL(VERSION_STRING);

	avr_delay_ms_16(100);

	// Initiate all sub tasks here.

	eepromLoad();
	avr_delay_ms_16(100);

	#ifdef DEBUG_LED_PORT
	avr_blink(3); // just so we see that it started
	avr_wtd_reset_and_sleep();
	#endif

	#if HW_VERSION == 0
	avr_tmr0_init();
	avr_tmr1_init();
	#else
	avr_tmr0_init(ee.signal_strength_percent);
	avr_tmr1_init();
	#endif
	avr_delay_ms_16(100);

	// To help debugging, print a message if the SW was not compiled
	// for same HW as configured.
	if (ee.expected_swhw != HW_VERSION)
	{
		UART_PRINT_P("HW_VERSION not as expected ");
		uart_print_hex8(ee.expected_swhw);
		uart_putchar(' ');
		uart_print_hex8(HW_VERSION);
		uart_print_crlf();
	}

	HIT_LED0_ENABLE();
	HIT_LED1_ENABLE();
	LASER_ENABLE();
	VIB_ENABLE();
	TRIGGER_INIT();
	EXTERNAL_IR_INIT();
	INTERNAL_IR_INIT();

	avr_delay_ms_16(100);

	beep_init();
	avr_delay_ms_16(100);

	power_init();
	avr_delay_ms_16(100);

	radio_init();
	avr_delay_ms_16(100);

	game_init();
	avr_delay_ms_16(100);

	cmd_init();
	avr_delay_ms_16(100);

	// All init is done, get ready to run main loop.
	UART_PRINT_PL("main");
	main_timer_ms = avr_systime_ms_16() + 1000;

	// The main loop, this will run until power off.
	for(;;)
	{
		// Put all urgent tasks here. Things that need to be called more than once per second.

		cmd_process();

		radio_process();

		game_process();

		beep_process();

		//log_ports();

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
				//log_ports();
				#ifdef RADIO_MODEM_ONLY
				cmd_init_tick_s();
				#endif
				++main_state;
				break;
			}
			case 2:
				power_tick_s();
				++main_state;
				break;
			case 3:
				radio_tick();
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


		// Set CPU in idle mode to save energy, it will wake up next time there is an interrupt
		avr_wtd_reset_and_sleep();
	}

	return(0);
} // end main()




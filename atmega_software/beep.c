/*
beep.c

Provide functions to do audio beeps.

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

2021-04-22 Created. /Henrik
*/

#include <avr/pgmspace.h>
#include <stdint.h>
#include "avr_cfg.h"
#include "avr_sys.h"
#include "avr_uart.h"
#include "avr_tmr0.h"
#include "avr_ports.h"
#include "beep.h"

#ifdef BEEP_USE_TMR0
#if defined AVR_SYS_USE_TMR0 || defined IR_OUTPUT_USE_TMR0
#error
#endif
#else
#error
#endif

enum {
	BEEP_IDLE_STATE,
	BEEP_LOW_STATE,
	BEEP_HIGH_STATE,
	BEEP_FAIL_STATE,
};

int8_t beep_state = 0;
int16_t beep_counter = 0;

void beep_init(void)
{
	UART_PRINT_P("beep\r\n");
	BEEP_ON(LOW_TONE);
}

void beep_tick_s(void)
{
	/*
    switch (beep_state)
    {
		default:
		case BEEP_IDLE_STATE:
		{
			BEEP_ON(LOW_TONE);
			beep_state = BEEP_LOW_STATE;
			break;
		}
		case BEEP_LOW_STATE:
		{
			BEEP_ON(HIGH_TONE);
			beep_state = BEEP_HIGH_STATE;
			break;
		}
		case BEEP_HIGH_STATE:
		{
			BEEP_OFF();
			break;
		}
		case BEEP_FAIL_STATE:
		{
			// TODO
		    break;
		}
    }
    */
}


/*
beep.c

Provide functions to do audio beeps.

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

2021-04-22 Created. /Henrik
*/

#include <avr/pgmspace.h>
#include <stdint.h>
#include "avr_cfg.h"
#include "avr_sys.h"
#include "avr_uart.h"
#include "avr_tmr0.h"
#include "avr_tmr1.h"
#include "avr_ports.h"
#include "beep.h"

#ifdef BEEP_USE_TMR0
#if defined AVR_SYS_USE_TMR0 || defined IR_OUTPUT_USE_TMR0
#error
#endif
#elif defined BEEP_USE_TMR1
// OK
#else
#error
#endif

enum {
	BEEP_IDLE_STATE,
	BEEP_ACTIVE_STATE,
};

static int8_t beep_state = 0;
//int16_t beep_counter = 0;
static int16_t beep_timer_ms = 0;
static int8_t log_state = 0;


// Size must be power of 2. So 2, 4, 8, 16...
#define BEEP_FIFO_SIZE 16

#define BEEP_FIFO_MASK (BEEP_FIFO_SIZE-1)

struct fifo_entry {
	uint16_t tone;
	uint16_t duration_ms;
};


volatile uint8_t beep_fifo_head;
volatile uint8_t beep_fifo_tail;
volatile struct fifo_entry beep_fifo_buff[BEEP_FIFO_SIZE];

#define BEEP_FIFO_INC(index) ((index+1)&BEEP_FIFO_MASK)

void beep_fifo_init(void)
{
	beep_fifo_head = 0;
	beep_fifo_tail = 0;
}

static int8_t beep_fifo_is_full(void)
{
  return ((BEEP_FIFO_INC(beep_fifo_head)) == beep_fifo_tail);
}

static int8_t beep_fifo_is_empty(void)
{
  return (beep_fifo_head == beep_fifo_tail);
}

void beep_fifo_put(uint16_t tone, uint16_t duration_ms)
{
	if (beep_fifo_is_full())
	{
		beep_fifo_tail = BEEP_FIFO_INC(beep_fifo_tail);
	}
	beep_fifo_buff[beep_fifo_head].tone=tone;
	beep_fifo_buff[beep_fifo_head].duration_ms=duration_ms;
	beep_fifo_head = BEEP_FIFO_INC(beep_fifo_head);
}

static volatile struct fifo_entry* beep_fifo_take(void)
{
	/*if (beep_fifo_is_empty())
	{
		return NULL;
	}*/
	volatile struct fifo_entry* d = &(beep_fifo_buff[beep_fifo_tail]);
	beep_fifo_tail = BEEP_FIFO_INC(beep_fifo_tail);
	return d;
}




static uint8_t beep_get_entries_in_queue(void)
{
    return ( beep_fifo_tail - beep_fifo_head ) & BEEP_FIFO_MASK;
}



uint8_t beep_get_free_space_in_queue()
{
	return((BEEP_FIFO_SIZE-1)-beep_get_entries_in_queue());
}


static void beep_on(uint16_t tone)
{
	if (tone)
	{
		BEEP_ON(tone);
	}
	else
	{
		BEEP_OFF();
	}
}




void beep_init()
{
	//UART_PRINT_P("beep\r\n");
	//BEEP_ON(LOW_TONE);
}

void beep_process()
{
    switch (beep_state)
    {
		default:
		case BEEP_IDLE_STATE:
		{
			if (!beep_fifo_is_empty())
			{
				volatile struct fifo_entry* d = beep_fifo_take();
				beep_timer_ms = avr_systime_ms_16() + d->duration_ms;
				beep_on(d->tone);
				beep_state = BEEP_ACTIVE_STATE;
			}
			break;
		}
		case BEEP_ACTIVE_STATE:
		{
			const int16_t t = avr_systime_ms_16();
			const int16_t d = t - beep_timer_ms;
			if (d>=0)
			{
				if (!beep_fifo_is_empty())
				{
					// Just change tone
					volatile struct fifo_entry* d = beep_fifo_take();
					beep_timer_ms = avr_systime_ms_16() + d->duration_ms;
					beep_on(d->tone);
				}
				else
				{
					BEEP_OFF();
					beep_state = BEEP_IDLE_STATE;
				}
			}
			break;
		}
    }
	if (beep_state != log_state)
	{
		UART_PRINT_P("bs ");
		uart_print_hex4(beep_state);
		uart_print_crlf();
		log_state = beep_state;
	}

}


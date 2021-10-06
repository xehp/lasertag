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
	BEEP_CONTINUOUS_STATE,
};

enum{
	led_idle_state,
	led_active_state,
};

static uint16_t beep_led = 0;
static int8_t beep_state_led = 0;
static int8_t beep_state_audio = 0;
//int16_t beep_counter = 0;
static int16_t beep_timer_ms = 0;
//static int8_t log_state = 0;
static int16_t beep_led_timer_ms = 0;

// Size must be power of 2. So 2, 4, 8, 16...
#define BEEP_FIFO_SIZE 16

#define BEEP_FIFO_MASK (BEEP_FIFO_SIZE-1)

struct fifo_entry {
	uint16_t tone;
	uint16_t duration_ms;
	uint8_t volume_percent;
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

void beep_fifo_put(uint16_t tone, uint16_t duration_ms, uint8_t volume_in_percent)
{
	if (beep_fifo_is_full())
	{
		beep_fifo_tail = BEEP_FIFO_INC(beep_fifo_tail);
	}
	beep_fifo_buff[beep_fifo_head].tone=tone;
	beep_fifo_buff[beep_fifo_head].duration_ms=duration_ms;
	beep_fifo_buff[beep_fifo_head].volume_percent=volume_in_percent;
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


uint8_t beep_get_free_space_in_fifo()
{
	return((BEEP_FIFO_SIZE-1)-beep_get_entries_in_queue());
}

void beep_fifo_clear(void)
{
	beep_fifo_tail = beep_fifo_head;
}


// Frequency will be FOSC/(tone+1)
// Example, if Fosc is 16 Mhz then:
// tone 15999 give 1 kHz.
// tone 1 give 8 Mhz
static void beep_on(uint16_t tone, uint8_t volume)
{
	if (tone)
	{
		BEEP_ON(tone, volume);
	}
	else
	{
		BEEP_OFF();
	}
}

static void dec_beep_time(int16_t time_passed_ms)
{
	if (beep_led>time_passed_ms)
	{
		beep_led-=time_passed_ms;
	}
	else
	{
		beep_led = 0;
	}
}

void beep_init()
{
	//UART_PRINT_P("beep\r\n");
	//BEEP_ON(LOW_TONE);
}

void beep_process()
{
	const int16_t t = avr_systime_ms_16();

	switch (beep_state_audio)
    {
		default:
		case BEEP_IDLE_STATE:
		{
			if (!beep_fifo_is_empty())
			{
				volatile struct fifo_entry* d = beep_fifo_take();
				beep_on(d->tone, d->volume_percent);
				if (d->duration_ms != 0)
				{
					beep_timer_ms = avr_systime_ms_16() + d->duration_ms;
					beep_state_audio = BEEP_ACTIVE_STATE;
				}
				else
				{
					beep_state_audio = BEEP_CONTINUOUS_STATE;
				}
			}
			break;
		}
		case BEEP_ACTIVE_STATE:
		{
			const int16_t dt = t - beep_timer_ms;
			if (dt>=0)
			{
				if (!beep_fifo_is_empty())
				{
					// Just change tone
					volatile struct fifo_entry* e = beep_fifo_take();
					beep_timer_ms = avr_systime_ms_16() + e->duration_ms;
					beep_on(e->tone, e->volume_percent);
				}
				else
				{
					BEEP_OFF();
					beep_state_audio = BEEP_IDLE_STATE;
				}
			}
			break;
		}
		case BEEP_CONTINUOUS_STATE:
		{
			if (!beep_fifo_is_empty())
			{
				beep_state_audio = BEEP_IDLE_STATE;
			}
			break;
		}
    }

	/*if (beep_state_audio != log_state)
	{
		UART_PRINT_P("bs ");
		uart_print_hex4(beep_state_audio);
		uart_print_crlf();
		log_state = beep_state_audio;
	}*/

	const int16_t time_passed_ms = t - beep_led_timer_ms;
	switch(beep_state_led)
	{
		case led_idle_state:
			if (beep_led>0)
			{
				HIT_LEDS_ON();
				dec_beep_time(time_passed_ms);
				beep_state_led = led_active_state;
			}
			break;
		default:
		case led_active_state:
		{
			HIT_LEDS_OFF();
			dec_beep_time(time_passed_ms);
			beep_state_led = led_idle_state;
			break;
		}
	}
	beep_led_timer_ms = t;
}

void beep_led_on(uint16_t duration_ms)
{
	/*UART_PRINT_P("b l ");
	uart_print_hex16(duration_ms);
	uart_print_crlf();*/
	beep_led = duration_ms;
}

void beep_led_off(void)
{
	//UART_PRINT_PL("b l -");
	beep_led = 0;
}

// f = FOSC/(tone+1)
// so tone = Fosc/f -1
uint16_t beep_tonecode_from_frequency(uint16_t frequency_Hz)
{
	const uint32_t f = frequency_Hz;
	return (AVR_FOSC / f) - 1L;
}

/*
game.c

The main state of this program.

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
#include "avr_tmr1.h"
#include "avr_ports.h"
#include "radio.h"
#include "game.h"


static uint8_t game_state = 0;
static int16_t game_timer_ms = 0;
static int16_t game_counter = 0;
static uint8_t internal_ir_state = 0;
static uint8_t external_ir_state = 0;


void game_init(void)
{
	UART_PRINT_P("game\r\n");
	game_timer_ms = avr_systime_ms_16();
}

void game_process(void)
{
	const uint16_t t = avr_systime_ms_16();

	uint8_t tmp[RADIO_PAYLOAD_SIZE] = {0};
	const uint8_t n = radio_receive_data(tmp, sizeof(tmp));

	if (n>0)
	{
		UART_PRINT_P("radio ");
		for (uint8_t i=0; i<sizeof(tmp); ++i)
		{
			uart_print_hex8(tmp[i]);
		}
		UART_PRINT_P("\r\n");
	}

	switch(internal_ir_state)
	{
	default:
	case 0:
		if (INTERNAL_IR_READ())
		{
			UART_PRINT_P("vib on\r\n");
			VIB_ON();
			internal_ir_state = 1;
		}
		break;
	case 1:
		if (!INTERNAL_IR_READ())
		{
			UART_PRINT_P("vib off\r\n");
			VIB_OFF();
			internal_ir_state = 0;
		}
		break;
	}

	switch(external_ir_state)
	{
	default:
	case 0:
		if (EXTERNAL_IR_READ())
		{
			UART_PRINT_P("hit on\r\n");
			HIT_LEDS_ON();
			external_ir_state = 1;
		}
		break;
	case 1:
		if (!EXTERNAL_IR_READ())
		{
			UART_PRINT_P("hit off\r\n");
			HIT_LEDS_OFF();
			external_ir_state = 0;
		}
		break;
	}


	switch (game_state)
	{
		default:
		case 0:
			if (TRIGGER_READ())
			{
				UART_PRINT_P("on\r\n");
				IR_OUTPUT_ON();
				LASER_ON();
				game_state = 1;
				game_timer_ms = t+10;
			}
			break;
		case 1:
		{
			if ((t-game_timer_ms) >= 0)
			{
				UART_PRINT_P("off\r\n");
				IR_OUTPUT_OFF();
				game_timer_ms = t+200;
				game_state = 2;
			}
			break;
		}
		case 2:
			if ((t-game_timer_ms) >= 0)
			{
				LASER_OFF();
				if (!TRIGGER_READ())
				{
					UART_PRINT_P("ready\r\n");
					game_state = 0;
				}
			}
			break;
	}
}

void game_tick_s()
{
	if (game_counter>30)
	{
		const uint8_t data[RADIO_PAYLOAD_SIZE] = {'H', 'e', 'l','l','o'};
		const int8_t r = radio_transmit_data(data, sizeof(data));
		if (r!=0)
		{
			UART_PRINT_P("fail\r\n");
		}
	}
	else
	{
		++game_counter;
	}
}

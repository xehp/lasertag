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
#include "avr_tmr2.h"
#include "avr_eeprom.h"
#include "radio.h"
#include "game.h"


static uint8_t game_state = 0;
static int16_t game_timer_ms = 0;
static int16_t game_counter = 0;
//static uint8_t internal_ir_state = 0;
//static uint8_t external_ir_state = 0;
static uint32_t msg_counter = 0;

void game_init(void)
{
	UART_PRINT_P("game\r\n");
	game_timer_ms = avr_systime_ms_16();
}

void game_process(void)
{
	const uint16_t t = avr_systime_ms_16();

	// This was just while debugging. Can be removed later.
	if (uart_get_free_space_in_write_buffer() < (UART_TX_BUFFER_SIZE/2))
	{
		return;
	}


	// This is how we check for new messages from radio.
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

	// Checking for codes received on IR.
	if (!rx1_fifo_is_empty())
	{
		const int8_t ch = rx1_fifo_take();
		UART_PRINT_P("rx1 ");
		uart_print_hex8(ch);
		UART_PRINT_P("\r\n");
	}

	if (!rx2_fifo_is_empty())
	{
		const int8_t ch = rx2_fifo_take();
		UART_PRINT_P("rx2 ");
		uart_print_hex8(ch);
		UART_PRINT_P("\r\n");
	}

    // Here we do things like check trigger, turning laser on or off.
	switch (game_state)
	{
		default:
		case 0:
			if (TRIGGER_READ())
			{
				UART_PRINT_P("on\r\n");
				LASER_ON();
				VIB_ON();
				HIT_LEDS_ON();
				game_state = 1;
				game_timer_ms = t+10;
			}
			break;
		case 1:
		{
			if ((t-game_timer_ms) >= 0)
			{
				UART_PRINT_P("off\r\n");
				HIT_LEDS_OFF();
				VIB_OFF();
				game_timer_ms = t+200;
				game_state = 2;
			}
			break;
		}
		case 2:
			if ((t-game_timer_ms) >= 0)
			{
				if (!TRIGGER_READ())
				{
					UART_PRINT_P("ready\r\n");
					LASER_OFF();
					game_state = 0;
				}
			}
			break;
	}
}

int8_t is_on = 0;

void game_tick_s()
{
	// Here is how a code is sent over IR
	tx1_fifo_put(ee.ID);

	/*if (is_on)
	{
		IR_OUTPUT_OFF();
		is_on = 0;
	}
	else
	{
		IR_OUTPUT_ON();
		is_on = 1;
	}*/

	if (game_counter>10)
	{
		// Here is an example on how to send a message over radio.
		uint8_t data[RADIO_PAYLOAD_SIZE];
		data[0] = 0;
		data[1] = (msg_counter >> 24) & 0xFF;
		data[2] = (msg_counter >> 16) & 0xFF;
		data[3] = (msg_counter >> 8) & 0xFF;
		data[4] = msg_counter & 0xFF;

		const int8_t r = radio_transmit_data(data, sizeof(data));
		if (r!=0)
		{
			UART_PRINT_P("fail\r\n");
		}
		msg_counter++;
		game_counter = 0;
	}
	else
	{
		++game_counter;
	}
}

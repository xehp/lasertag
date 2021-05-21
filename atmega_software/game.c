/*
game.c

The main state of this program.

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
#include "avr_tmr2.h"
#include "avr_eeprom.h"
#include "utility.h"
#include "radio.h"
#include "beep.h"
#include "game.h"

enum
{
	msg_category_ignore = 0,
	msg_category_log = 1,
	msg_category_cmd = 2,
	msg_category_ack = 3,
	msg_category_nok = 4,
	msg_category_status = 5
};

enum
{
	hit_msg = 1,
};

enum
{
	pointer_dev = 0,
	detector_dev = 1,
};


enum
{
	game_state_idle,
	game_state_vib,
	game_state_led,
	game_state_dark,
	game_state_ready,
	trigger_pulled_leds_on,
	trigger_pulled_leds_off,
	trigger_released
};

#define MSG_CAT_SIZE 3
#define MSG_CMD_SIZE (8-MSG_CAT_SIZE)
#define MSG_CMD_MASK ((1<<MSG_CMD_SIZE)-1)

#define DEV_TYPE_SIZE 2
#define PLAYER_ID_SIZE (8-DEV_TYPE_SIZE)
#define PLAYER_ID_MASK ((1<<PLAYER_ID_SIZE)-1)


static int8_t game_state = 0;
static uint32_t msg_counter = 0;
static int16_t game_timer_ms = 0;
static int8_t hit_flag = 0;
static int8_t log_game_state = 0;

void game_init(void)
{
	UART_PRINT_P("game\r\n");
	game_timer_ms = avr_systime_ms_16();
}

static uint8_t type_of_other_dev(uint8_t this_device)
{
	return (this_device == detector_dev) ? pointer_dev : detector_dev;
}

static void send_ack_to_other_dev(uint8_t hit_msg, uint8_t seq_nr, uint8_t hit_by)
{
	uint8_t data[RADIO_PAYLOAD_MAX_LEN];
	data[0] = (msg_category_ack << MSG_CMD_SIZE) | (hit_msg & MSG_CMD_MASK);
	data[1] = seq_nr;
	data[2] = (ee.device_type << PLAYER_ID_SIZE) | (ee.player_number & PLAYER_ID_MASK);
	data[3] = (type_of_other_dev(ee.device_type) << PLAYER_ID_SIZE) | ee.player_number;
	data[4] = hit_by;

	const int8_t r = radio_transmit_data(data, sizeof(data));
	if (r!=0)
	{
		UART_PRINT_P("fail\r\n");
	}
}

static void send_got_hit_msg(uint8_t ch)
{
	/*
	 * Message format:
	 *   msg_category and command
	 *   sequence number for ack
	 *   source dev and ID
	 *   destination dev and ID
	 *   Hit by player ID (no dev info needed)
	 */
	uint8_t data[RADIO_PAYLOAD_MAX_LEN];
	data[0] = (msg_category_cmd << MSG_CMD_SIZE) | (hit_msg & MSG_CMD_MASK);
	data[1] = msg_counter & 0xFF;
	data[2] = (ee.device_type << PLAYER_ID_SIZE) | (ee.player_number & PLAYER_ID_MASK);
	data[3] = (type_of_other_dev(ee.device_type) << PLAYER_ID_SIZE) | (ee.player_number & PLAYER_ID_MASK);
	data[4] = ch;
	msg_counter++;
	radio_transmit_data(data, sizeof(data));
}


static void play_got_hit_sound(void)
{
	beep_fifo_put(VHIGH_TONE, 50);
	beep_fifo_put(HIGH_TONE, 100);
	beep_fifo_put(MID_TONE, 200);
	beep_fifo_put(LOW_TONE, 800);
	beep_fifo_put(NO_TONE, 50);
}

static void play_fire_laser_sound(void)
{
	beep_fifo_put(MID_TONE, 50);
	beep_fifo_put(HIGH_TONE, 50);
	beep_fifo_put(MID_TONE, 50);
}

static void play_hit_someone_sound(void)
{
	beep_fifo_put(VHIGH_TONE, 50);
}


static void hit_received_on_ir(uint8_t code)
{
	uint8_t ch = translate_from_ir_code(code);

	uart_print_hex8(code);
	uart_putchar(' ');
	uart_print_hex8(ch);

	if (ch==0xFF)
	{
		// just noise, ignore
		UART_PRINT_P("?\r\n");
	}
	/*else if ((ch & PLAYER_ID_MASK) == (ee.player_number & PLAYER_ID_MASK))
	{
		// Ignore own signal.
		UART_PRINT_P("-\r\n");
	}*/
	else
	{
		// Inform our other device
		UART_PRINT_P("+\r\n");
		send_got_hit_msg(ch);
		if (ee.device_type == pointer_dev)
		{
			beep_led_on();
			play_got_hit_sound();
			hit_flag = 1;
		}
	}
}

void hit_message_received(const uint8_t *msg, uint8_t n)
{
	uint8_t seq_nr = msg[1];
	uint8_t player_that_was_hit = msg[2] & PLAYER_ID_MASK;
	//uint8_t hit_by = msg[4];

	if (player_that_was_hit == ee.player_number)
	{
		// we got hit
		UART_PRINT_P("hit\r\n");
		beep_led_on();
		send_ack_to_other_dev(hit_msg, seq_nr, player_that_was_hit);
		play_got_hit_sound();
		hit_flag = 1;
	}
	else
	{
		UART_PRINT_P("*\r\n");
	}

	/*if ((hit_by & PLAYER_ID_MASK) == ee.player_number)
	{
		// we hit someone else
		beep_fifo_put(MID_TONE, 50);
	}*/
}

void hit_ack_received(const uint8_t *msg, uint8_t n)
{
	//uint8_t seq_nr = msg[1];
	uint8_t player_that_was_hit = msg[2] & PLAYER_ID_MASK;
	uint8_t hit_by = msg[4];

	if (player_that_was_hit == (ee.player_number & PLAYER_ID_MASK))
	{
		UART_PRINT_P("ack\r\n");

		if (ee.device_type != pointer_dev)
		{
			beep_led_on();
			play_got_hit_sound();
			hit_flag = 1;
		}
	}
	else if ((hit_by & PLAYER_ID_MASK) == (ee.player_number & PLAYER_ID_MASK))
	{
		// we hit someone else
		UART_PRINT_P("ping\r\n");
		play_hit_someone_sound();
	}
	else
	{
		UART_PRINT_P(".\r\n");
	}
}

void command_category_received(const uint8_t *msg, uint8_t n)
{
	const uint8_t msg_type = msg[0] & MSG_CMD_MASK;
	switch (msg_type)
	{
		case hit_msg:
		{
			hit_message_received(msg, n);
			break;
		}
		default:
			// ignore
			break;
	}
}

void ack_received(const uint8_t *msg, uint8_t n)
{
	const uint8_t msg_type = msg[0] & MSG_CMD_MASK;
	switch (msg_type)
	{
		case hit_msg:
		{
			hit_ack_received(msg, n);
			break;
		}
		default:
			// ignore
			UART_PRINT_P("cmd\r\n");
			break;
	}
}

void msg_received(const uint8_t *msg, uint8_t n)
{
	const uint8_t msg_cat = msg[0] >> MSG_CMD_SIZE;
	switch (msg_cat)
	{
		case msg_category_cmd:
		{
			command_category_received(msg, n);
			break;
		}
		case msg_category_ack:
		{
			ack_received(msg, n);
			break;
		}
		default:
			// ignore
			UART_PRINT_P("cat\r\n");
			break;
	}
}

#if 0
static void test_message(void)
{
	// Here is an example on how to send a message over radio.
	uint8_t data[RADIO_PAYLOAD_MAX_LEN];
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
}
#endif

static void enter_vib_state(void)
{
	beep_led_on();
	VIB_ON();
	hit_flag = 0;
	UART_PRINT_P("vib\r\n");
	LASER_OFF();
	game_timer_ms = avr_systime_ms_16() + 3000;
	game_state = game_state_vib;
}

void game_process(void)
{
	const int16_t t = avr_systime_ms_16();
	const int16_t d = t-game_timer_ms;

#if 1

	// This was just while debugging. Can be removed later.
	/*if (uart_get_free_space_in_write_buffer() < (UART_TX_BUFFER_SIZE/2))
	{
		return;
	}*/

	// This is how we check for new messages from radio.
	uint8_t tmp[RADIO_PAYLOAD_MAX_LEN] = {0};
	const uint8_t n = radio_receive_data(tmp, sizeof(tmp));
	if (n>0)
	{
		UART_PRINT_P("gr ");
		for (uint8_t i=0; i<sizeof(tmp); ++i)
		{
			uart_print_hex8(tmp[i]);
		}
		uart_print_crlf();
		msg_received(tmp, n);
	}

	// Checking for codes received on IR.
	if (!rx1_fifo_is_empty())
	{
		const int8_t code = rx1_fifo_take();
		UART_PRINT_P("rx1 ");
		hit_received_on_ir(code);
	}

	if (!rx2_fifo_is_empty())
	{
		const int8_t code = rx2_fifo_take();
		UART_PRINT_P("rx2 ");
		hit_received_on_ir(code);
	}


	switch (game_state)
	{
		default:
		case game_state_idle:
			beep_led_on();
			game_timer_ms = t + 3000;
			game_state = game_state_led;
			break;
		case game_state_vib:
			if (hit_flag)
			{
				enter_vib_state();
			}
			else if (d >= 0)
			{
				VIB_OFF();
				//UART_PRINT_P("led\r\n");
				game_timer_ms = t + 3000;
				game_state = game_state_led;
			}
			break;
		case game_state_led:
			if (hit_flag)
			{
				enter_vib_state();
			}
			else if (d >= 0)
			{
				//UART_PRINT_P("dark\r\n");
				beep_led_off();
				game_timer_ms = t + 3000;
				game_state = game_state_dark;
			}
			break;
		case game_state_dark:
			if (hit_flag)
			{
				enter_vib_state();
			}
			else if (d >= 0)
			{
				//UART_PRINT_P("ready\r\n");
				beep_led_off();
				game_timer_ms = t + 3000;
				game_state = game_state_ready;
			}
			break;
		case game_state_ready:
			if (hit_flag)
			{
				enter_vib_state();
			}
			else if (TRIGGER_READ())
			{
				uint8_t c = translate_to_ir_code(ee.player_number);
				UART_PRINT_P("tx ");
				uart_print_hex8(c);
				uart_putchar(' ');
				uart_print_hex8(ee.player_number);
				uart_print_crlf();
				tx1_fifo_put(c);
				LASER_ON();
				beep_led_on();
				play_fire_laser_sound();
				game_state = trigger_pulled_leds_on;
				game_timer_ms = t+100;
			}
			else
			{
				LASER_OFF();
			}
			break;
		case trigger_pulled_leds_on:
		{
			if (hit_flag)
			{
				enter_vib_state();
			}
			else if (d >= 0)
			{
				//UART_PRINT_P("laser\r\n");
				beep_led_off();
				LASER_OFF();
				game_timer_ms = t+100;
				game_state = trigger_pulled_leds_off;
			}
			break;
		}
		case trigger_pulled_leds_off:
			if (hit_flag)
			{
				enter_vib_state();
			}
			else if (!TRIGGER_READ())
			{
				//UART_PRINT_P("off\r\n");
				game_state = trigger_released;
			}
			else if (d >= 0)
			{
				// Comment this out if autofire is not wanted.
				//UART_PRINT_P("ready\r\n");
				game_state = game_state_ready;
			}
			break;
		case trigger_released:
			if (hit_flag)
			{
				enter_vib_state();
			}
			else if (d >= 0)
			{
				//UART_PRINT_P("ready\r\n");
				game_state = game_state_ready;
			}
			break;
	}


	if (game_state != log_game_state)
	{
		UART_PRINT_P("gs ");
		uart_print_hex4(game_state);
		uart_print_crlf();
		log_game_state = game_state;
	}

#else
	// This code is used for testing HW

	// Checking for codes received on IR.
	if (!rx1_fifo_is_empty())
	{
		const int8_t code = rx1_fifo_take();
		UART_PRINT_P("rx1 ");
		uart_print_hex8(code);
		uart_print_crlf();
	}

	if (!rx2_fifo_is_empty())
	{
		const int8_t code = rx2_fifo_take();
		UART_PRINT_P("rx2 ");
		uart_print_hex8(code);
		uart_print_crlf();
	}


	if (d>=0)
	{
		tx1_fifo_put(0x55);

		switch(game_state)
		{
		case 0:
			LASER_ON();
			beep_led_on();
			VIB_ON();
			game_state = 1;
			break;
		default:
			LASER_OFF();
			beep_led_off();
			VIB_OFF();
			game_state = 0;
			break;
		}

		game_timer_ms = t+50;
	}

	beep_fifo_put(MID_TONE, 5000);

	int8_t trig = TRIGGER_READ();

	if (trig != log_game_state)
	{
		UART_PRINT_P("gs ");
		uart_print_hex4(trig);
		uart_print_crlf();
		log_game_state = trig;
	}

#endif

}

void game_tick_s()
{
}

int8_t game_get_state()
{
	return game_state;
}

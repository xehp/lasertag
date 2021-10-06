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
#include "power.h"
#include "radio.h"
#include "beep.h"
#include "msg.h"
#include "game.h"




// message sub category
enum
{
	hit_msg = 1,
	hit_ack_msg = 2,
	alive_msg = 3,
	pairing_request_radio = 4,
	pairing_request_radio_ack = 5,
	already_paired = 6,
	pairing_request_radio_nack = 7,
	goodbye_msg = 8,
	request_alive_msg = 9,
};

// TODO unknown_dev should have been zero
enum
{
	pointer_dev = 0,
	detector_dev = 1,
	unknown_dev = 2,
};

#ifdef RADIO_MODEM_ONLY

static int8_t game_state = 0;





void game_init(void)
{
}

void game_process(void)
{
}

void game_tick_s(void)
{
}

int8_t game_get_state(void)
{
	return game_state;
}

void game_message_category_received_old(struct utility_r_struct *urs)
{
	UART_PRINT_PL("g rcv old");
}


uint16_t is_other_dev_is_alive_s(void)
{
	return 0;
}

uint32_t game_pair_addr(void)
{
	return 0;
}

int64_t game_pair_nr(void)
{
	return 0;
}

// See also init_game_msg_hdr
void game_message_category_received(struct utility_r_struct *urs, uint8_t sub, uint32_t src_addr, int8_t radio_pipe)
{
	const uint32_t rcv_pair_addr = utility_r_take64(urs, ADDR_NR_NOF_BITS);
	const uint16_t rcv_time_s = utility_r_take16(urs, TIME_S_NOF_BITS);
	const uint16_t rcv_pair_nr = utility_r_take16(urs, PAIR_NR_NOF_BITS);

	UART_PRINT_P("g rcv ");
	print_int16(radio_pipe);
	uart_putchar(' ');
	print_int64(src_addr);
	uart_putchar(' ');
	print_int64(rcv_pair_addr);
	uart_putchar(' ');
	print_int16(rcv_time_s);
	uart_putchar(' ');
	uart_print_hex16(rcv_pair_nr);
	uart_putchar(' ');

	switch (sub)
	{
	case hit_msg:
		UART_PRINT_P("hit_msg");
		break;
	case hit_ack_msg:
		UART_PRINT_P("hit_ack");
		break;
	case alive_msg:
		UART_PRINT_P("alive_msg");
		break;
	case pairing_request_radio:
		UART_PRINT_P("pairing_request_radio");
		break;
	case pairing_request_radio_ack:
		UART_PRINT_P("pairing_request_radio_ack");
		break;
	case already_paired:
		UART_PRINT_P("already_paired");
		break;
	case pairing_request_radio_nack:
		UART_PRINT_P("pairing_request_radio_nack");
		break;
	case goodbye_msg:
		UART_PRINT_P("goodby");
		break;
	case request_alive_msg:
		UART_PRINT_P("req alive");
		break;
	default:
		uart_print_hex8(sub);
		break;
	}

	uart_print_crlf();

}



#else

enum
{
	game_state_idle = 0,
	game_starting_up,
	game_state_vib,
	game_state_prepare,
	game_state_ready,
	trigger_pulled_leds_on,
	trigger_pulled_leds_off,
	trigger_released,
	timeout_from_other_device,
	pairing_main,
	pairing_waiting_for_pointer_dev,
	pairing_waiting_for_detector_dev,
	pairing_waiting_for_radio_ack,
	wait_for_alive,
};


#define AMMO_COST 2

static int8_t game_state = game_state_idle;
static int32_t game_timer_ms = 0;
static int8_t hit_flag = 0;
static int8_t trig_state = 0;
static int32_t trig_state_ms = 0;
static int32_t ignore_trig_time_ms = 0;
static uint16_t ammo = 100*AMMO_COST;
//static int32_t alive_sent_ms = 0;
static uint16_t other_dev_is_alive_s = 0;

static int8_t team = unknown_team;
static int8_t immediate_recovery_flag = 0;

static uint16_t pair_nr = -1;
static uint32_t pair_addr = RADIO_BROADCAST_ADDRESS;
static uint8_t pair_ir_code = 0;
static uint8_t fire_ir_code = 0;

void game_init(void)
{
	UART_PRINT_PL("g init");
	game_timer_ms = avr_systime_ms_16();
}

/*static uint8_t type_of_other_dev(uint8_t this_device)
{
	return (this_device == detector_dev) ? pointer_dev : detector_dev;
}*/

static uint8_t get_player_number(void)
{
	return (ee.player_number != -1) ? ee.player_number : ee.serial_nr;
}

static uint32_t get_own_addr(void)
{
	return ee.serial_nr;
}

static uint16_t make_pairing_check_number(void)
{
	const uint16_t p = get_player_number();
	const uint16_t c = (p << 8) | pair_ir_code;
	const uint16_t r = ee.game_code;
	return r ^ c;
}

static void alive_message_sent(void)
{
	//const int32_t t = avr_systime_ms_32();
	//alive_sent_ms = t;
}

static uint16_t game_time_remaining_s(void)
{
	const uint16_t at = power_activity_time_remaining_s();
	const uint16_t t = (at < ee.keep_alive_s) ? at : ee.keep_alive_s;
	return t;
}

static void init_game_msg_hdr(struct radio_data_packer *packer, uint8_t sub_category)
{
	// The radio header is currently 6 bytes. Pair nr is 8 bytes so this header is 14 bytes.
	radio_data_packer_init(packer, msg_category_game, sub_category, radio_take_sequence_number());
	radio_data_packer_put64(packer, ADDR_NR_NOF_BITS, pair_addr);
	// TODO Pack game_time_remaining_s(), due to backward compatibility will just send zero here for now.
	radio_data_packer_put16(packer, TIME_S_NOF_BITS, /*game_time_remaining_s()*/0);
	radio_data_packer_put16(packer, PAIR_NR_NOF_BITS, pair_nr);
}

// Message format here must be same as in process_alive_msg
static void send_alive_msg(uint8_t request)
{
	const uint16_t t = game_time_remaining_s();

	struct radio_data_packer packer;
	init_game_msg_hdr(&packer, request);

	UART_PRINT_P("g sa ");
	print_int64(pair_addr);
	uart_putchar(' ');
	uart_print_hex32(pair_nr);
	uart_putchar(' ');
	print_int16(t);
	uart_print_crlf();
	radio_data_packer_put16(&packer, TIME_S_NOF_BITS, t);

	// TODO send to pair_addr or RADIO_BROADCAST_ADDRESS?
	radio_transmit_put_data(packer.buffer, sizeof(packer.buffer), RADIO_BROADCAST_ADDRESS);

	alive_message_sent();
}

/*static void check_send_alive_message(void)
{
	const int32_t t = avr_systime_ms_32();
	const int32_t time_since_alive_sent_ms = t - alive_sent_ms;
	const int32_t keep_alive_ms = (ee.keep_alive_s * 240L);

	if (time_since_alive_sent_ms - keep_alive_ms >= 0)
	{
		send_alive_msg(alive_msg);
	}
}*/

/*static void set_next_alive_msg_ms(uint16_t time_ms)
{
	const int32_t t = avr_systime_ms_32();
	const int32_t time_since_alive_sent_ms = t - alive_sent_ms;
	const int32_t keep_alive_ms = (int32_t)time_ms;
	if (time_since_alive_sent_ms - keep_alive_ms >= 0)
	{
		alive_sent_ms = t - keep_alive_ms;
	}
}*/

static int8_t is_other_dev_alive(void)
{
	return other_dev_is_alive_s != 0;
}

// See also process_goodbye_msg
static void send_goodbye_msg(void)
{
	struct radio_data_packer packer;
	init_game_msg_hdr(&packer, goodbye_msg);

	radio_transmit_put_data(packer.buffer, sizeof(packer.buffer), RADIO_BROADCAST_ADDRESS);

	alive_message_sent();
}

static void goodby_pair(void)
{
	UART_PRINT_P("g gp ");
	print_int64(pair_addr);
	uart_putchar(' ');
	uart_print_hex32(pair_nr);
	uart_print_crlf();

	send_goodbye_msg();
	pair_addr = RADIO_BROADCAST_ADDRESS;
	pair_nr = -1;
	pair_ir_code = -1;
}

/**
 * Message format:
 *   std hdr (64 bits)
 *   source dev
 *   source playerId
 *   spare (2 bits)
 *   Hit by player ID
 *
 * See also hit_message_received()
 */
static void send_hit_msg(uint8_t hit_by_code)
{
	UART_PRINT_P("g shm ");
	uart_print_hex8(hit_by_code);
	uart_print_crlf();

	struct radio_data_packer packer;
	init_game_msg_hdr(&packer, hit_msg);

	radio_data_packer_put16(&packer, IR_CODE_NR_NOF_BITS, hit_by_code);

	radio_transmit_put_data(&packer.buffer[0], sizeof(packer.buffer), RADIO_BROADCAST_ADDRESS);

	alive_message_sent();
}


/**
 * Message format:
 *   std hdr (64 bits)
 *   source dev
 *   source playerId
 *   spare (2 bits)
 *   Hit by player ID
 *
 *   See also process_hit_ack_msg. The message format here and there must match.
 */
static void send_hit_ack_msg(uint8_t hit_by_code)
{
	UART_PRINT_P("g sham ");
	uart_print_hex8(hit_by_code);
	uart_print_crlf();

	struct radio_data_packer packer;
	init_game_msg_hdr(&packer, hit_ack_msg);

	radio_data_packer_put16(&packer, IR_CODE_NR_NOF_BITS, hit_by_code);

	radio_transmit_put_data(&packer.buffer[0], sizeof(packer.buffer), RADIO_BROADCAST_ADDRESS);

	alive_message_sent();
}

// This message is sent when a detector device received a pairing IR signal while already paired.
// Message format here must be same as in pairing_reply_already_paired
static void send_already_paired(uint8_t ir_code)
{
	UART_PRINT_P("g sap ");
	uart_print_hex8(ir_code);
	uart_print_crlf();

	struct radio_data_packer packer;
	init_game_msg_hdr(&packer, already_paired);

	radio_data_packer_put16(&packer, IR_CODE_NR_NOF_BITS, pair_ir_code);
	radio_data_packer_put16(&packer, IR_CODE_NR_NOF_BITS, ir_code);

	radio_transmit_put_data(&packer.buffer[0], sizeof(packer.buffer), RADIO_BROADCAST_ADDRESS);
}

static void play_got_hit_sound(void)
{
	beep_fifo_put(VHIGH_TONE, 50, ee.audio_volume_percent);
	beep_fifo_put(HIGH_TONE, 100, ee.audio_volume_percent);
	beep_fifo_put(MID_TONE, 200, ee.audio_volume_percent);
	beep_fifo_put(LOW_TONE, 800, ee.audio_volume_percent);
	beep_fifo_put(NO_TONE, 50, 0);
}

static void play_fire_laser_sound(void)
{
	beep_fifo_put(MID_TONE, 50, DIV_UP(ee.audio_volume_percent, 2));
	beep_fifo_put(HIGH_TONE, 50, DIV_UP(ee.audio_volume_percent, 2));
	beep_fifo_put(MID_TONE, 50, DIV_UP(ee.audio_volume_percent, 2));
}

static void play_hit_someone_sound(void)
{
	beep_fifo_put(VHIGH_TONE, 50, ee.audio_volume_percent);
}

static void play_unknown_event_sound(void)
{
	beep_fifo_put(NO_TONE, 100, 0);
	beep_fifo_put(HIGH_TONE, 100, DIV_UP(ee.audio_volume_percent, 8));
	beep_fifo_put(NO_TONE, 100, 0);
	beep_fifo_put(HIGH_TONE, 100, DIV_UP(ee.audio_volume_percent, 8));
	beep_fifo_put(NO_TONE, 100, 0);
	beep_fifo_put(HIGH_TONE, 100, DIV_UP(ee.audio_volume_percent, 8));
	beep_fifo_put(NO_TONE, 100, 0);
}

/*static int16_t time_since_alive_received_s(void)
{
	const int32_t t = avr_systime_ms_32();
	const int32_t time_since_alive_received_ms = t - other_dev_is_alive_ms;
	const int16_t time_since_alive_received_s = time_since_alive_received_ms/1000L;
	return time_since_alive_received_s;
}*/


static void process_other_dev_is_alive_s(uint16_t other_dev_time_remaining_s)
{
	if (other_dev_time_remaining_s > ee.keep_alive_s)
	{
		other_dev_time_remaining_s = ee.keep_alive_s;
	}

	if (other_dev_is_alive_s < other_dev_time_remaining_s)
	{
		other_dev_is_alive_s = other_dev_time_remaining_s;
	}

	if (ee.device_type == detector_dev)
	{
		power_activity_set_time_remaining_s(other_dev_is_alive_s);
	}
}


// received alive message
// Message format here must be same as in send_alive_msg
static void process_alive_msg(struct utility_r_struct *urs, uint32_t src_addr, uint32_t received_pair_addr, uint16_t received_pair_nr, int8_t radio_pipe, int8_t sub)
{
	const uint32_t own_addr = get_own_addr();
	const uint16_t time_remaining_s = utility_r_take16(urs, TIME_S_NOF_BITS);

	UART_PRINT_P("g ra ");
	print_int64(src_addr);
	uart_putchar(' ');
	print_int16(radio_pipe);
	uart_putchar(' ');
	uart_print_hex4(sub);
	uart_putchar(' ');
	print_int16(time_remaining_s);
	uart_putchar(' ');
	print_int64(received_pair_addr);
	uart_putchar(' ');
	uart_print_hex32(received_pair_nr);
	uart_print_crlf();

	if ((radio_pipe == radio_specific_pipe) || (received_pair_addr == own_addr))
	{
		if (pair_addr == RADIO_BROADCAST_ADDRESS)
		{
			// Not yet paired. But an alive message was sent to us so we have a pair already?
			// Accept this device as our pair device.
			UART_PRINT_PL("g accept");
			pair_addr = src_addr;
			pair_nr = received_pair_nr;
			//set_next_alive_msg_ms(500);
			process_other_dev_is_alive_s(time_remaining_s);
		}
		else if ((src_addr == pair_addr) /*&& (received_pair_addr == own_addr) && (other_pair_nr == pair_nr)*/)
		{
			process_other_dev_is_alive_s(time_remaining_s);
		}

		if (sub == request_alive_msg)
		{
			send_alive_msg(alive_msg);
		}
	}
	else if ((received_pair_addr == RADIO_BROADCAST_ADDRESS) && (src_addr == pair_addr))
	{
		// Our pair is alive but not paired with anyone.
		if (sub == request_alive_msg)
		{
			send_alive_msg(alive_msg);
		}
	}
	else if ((src_addr == pair_addr))
	{
		// Our pair is alive but paired with someone else.
		goodby_pair();
	}
	else
	{
		UART_PRINT_PL("g ab");
	}


}

// See also send_goodbye_msg
static void process_goodbye_msg(struct utility_r_struct *urs, uint32_t src_addr, uint32_t received_pair_addr, uint16_t received_pair_nr, int8_t radio_pipe)
{
	const uint32_t own_addr = get_own_addr();
	UART_PRINT_PL("g gr");

	if ((radio_pipe == radio_specific_pipe) || (received_pair_addr == own_addr) || (src_addr == pair_addr))
	{
		pair_addr = RADIO_BROADCAST_ADDRESS;
		pair_nr = -1;
		pair_ir_code = 0;
	}
}

// See also send_hit_msg, message format there and here must match.
static void process_hit_msg(struct utility_r_struct *urs, uint32_t src_addr, uint32_t received_pair_addr, uint16_t received_pair_nr)
{
	const uint32_t own_addr = get_own_addr();
	uint8_t hit_by_code = utility_r_take16(urs, IR_CODE_NR_NOF_BITS);

	if ((src_addr == pair_addr) &&
	    (received_pair_addr == own_addr) /*&&
	    (received_pair_nr == pair_nr)*/)
	{
		// This is a message from our pair (AKA other dev) saying we got hit.
		UART_PRINT_P("g hit by ");
		print_int64(src_addr);
		uart_putchar(' ');
		uart_print_hex8(hit_by_code);
		uart_putchar(' ');
		print_int64(received_pair_addr);
		uart_print_crlf();

		beep_led_on(ee.hit_time_third_ms);
		send_hit_ack_msg(hit_by_code);
		play_got_hit_sound();
		hit_flag = 1;
		power_activity_set_time_remaining_s(ee.keep_alive_s);

		process_other_dev_is_alive_s(ee.keep_alive_s);
	}
	else if (hit_by_code == pair_ir_code)
	{
		// we hit someone else
		UART_PRINT_P("g we hit ");
		print_int64(src_addr);
		uart_putchar(' ');
		uart_print_hex8(hit_by_code);
		uart_putchar(' ');
		print_int64(received_pair_addr);
		uart_putchar(' ');
		uart_print_hex16(received_pair_nr);
		uart_print_crlf();

	}
	else
	{
		// someone else got hit (not by us)
		UART_PRINT_P("g someone else ");
		print_int64(src_addr);
		uart_putchar(' ');
		uart_print_hex8(hit_by_code);
		uart_putchar(' ');
		print_int64(received_pair_addr);
		uart_putchar(' ');
		uart_print_hex16(received_pair_nr);
		uart_print_crlf();
	}

}

// received hit ack
// See also send_hit_ack_msg.
// The message format here and there must match.
static void process_hit_ack_msg(struct utility_r_struct *urs, uint32_t src_addr, uint32_t received_pair_addr, uint16_t received_pair_nr)
{
	const uint32_t own_addr = get_own_addr();
	uint8_t hit_by_code = utility_r_take16(urs, IR_CODE_NR_NOF_BITS);

	if ((src_addr == pair_addr) &&
	    (received_pair_addr == own_addr) /*&&
	    (received_pair_nr == pair_nr)*/)
	{
		UART_PRINT_P("g hit ack ");
		uart_print_hex8(hit_by_code);
		uart_print_crlf();

		if (ee.device_type != pointer_dev)
		{
			if (hit_flag != 1)
			{
				beep_led_on(ee.hit_time_third_ms);
				play_got_hit_sound();
				hit_flag = 1;
				power_activity_set_time_remaining_s(ee.keep_alive_s);
			}
		}

		process_other_dev_is_alive_s(ee.keep_alive_s);
	}
	else if (hit_by_code == pair_ir_code)
	{
		// we hit someone else and its confirmed
		UART_PRINT_P("g hit conf ");
		uart_print_hex32(src_addr);
		uart_print_crlf();

		play_hit_someone_sound();
	}
	else
	{
		// someone else got hit (not by us)
		UART_PRINT_P("g someone ack ");
		uart_print_hex32(src_addr);
		uart_putchar(' ');
		uart_print_hex8(hit_by_code);
		uart_print_crlf();
	}
}


// This is sent by pointer device when it has received a pairing request over radio.
// Message format here must be same as in process_pairing_req_radio_nack.
static void send_pairing_req_radio_ack_nack(uint32_t dst_addr, uint8_t ir_code, uint8_t ack_nack)
{

	struct radio_data_packer packer;
	init_game_msg_hdr(&packer, ack_nack);

	radio_data_packer_put16(&packer, IR_CODE_NR_NOF_BITS, ir_code);

	// TODO pair_nr should contain the ir_code so send it instead of ir_code as above?
	// radio_data_packer_put16(&packer, PAIR_NR_NOF_BITS, pair_nr);
	// radio_data_packer_put16(&packer, IR_CODE_NR_NOF_BITS, 0);

	UART_PRINT_P("g sprran ");
	print_int64(dst_addr);
	uart_putchar(' ');
	uart_print_hex8(ir_code);
	uart_putchar(' ');
	uart_print_hex8(ack_nack);
	uart_putchar(' ');
	print_int64(pair_addr);
	uart_print_crlf();

	radio_transmit_put_data(packer.buffer, sizeof(packer.buffer), dst_addr);
}

// pairing_request_radio
// Detector devices broadcast this when they receive a pairing request on IR (and are not already paired).
// Message format here must be same as in send_pairing_request_radio
static void process_pairing_request_radio(struct utility_r_struct *urs, uint32_t src_addr, uint32_t received_pair_addr, uint16_t received_pair_nr, int8_t radio_pipe)
{
	uint8_t in_reply_to_code = utility_r_take16(urs, IR_CODE_NR_NOF_BITS);

	if ((game_state == pairing_waiting_for_detector_dev) &&
		(pair_addr == RADIO_BROADCAST_ADDRESS) &&
		(ee.device_type == pointer_dev) &&
		(in_reply_to_code == pair_ir_code))
	{
		UART_PRINT_P("g prrr ");
		print_int64(src_addr);
		uart_putchar(' ');
		print_int16(radio_pipe);
		uart_putchar(' ');
		uart_print_hex8(in_reply_to_code);
		uart_print_crlf();

		pair_addr = src_addr;
		pair_nr = received_pair_nr;

		process_other_dev_is_alive_s(ee.keep_alive_s);

		send_pairing_req_radio_ack_nack(src_addr, in_reply_to_code, pairing_request_radio_ack);
	}
	else
	{
		UART_PRINT_P("g prrri ");
		print_int64(src_addr);
		uart_putchar(' ');
		print_int16(radio_pipe);
		uart_putchar(' ');
		uart_print_hex8(in_reply_to_code);
		uart_putchar(' ');
		print_int16(game_state);
		uart_putchar(' ');
		print_int64(pair_addr);
		uart_putchar(' ');
		uart_print_hex8(pair_ir_code);
		uart_print_crlf();

		send_pairing_req_radio_ack_nack(src_addr, in_reply_to_code, pairing_request_radio_nack);

		//set_next_alive_msg_ms(5000);

	}
}

// already_paired
// Detector devices broadcast pairing declined if they receive a pairing request on IR but are already paired.
// See also send_already_paired.
static void process_already_paired(struct utility_r_struct *urs, uint32_t src_addr, uint32_t received_pair_addr, uint16_t received_pair_nr)
{
	const uint32_t own_addr = get_own_addr();
	uint8_t other_dev_ir_code = utility_r_take16(urs, IR_CODE_NR_NOF_BITS);
	uint8_t in_reply_to_code = utility_r_take16(urs, IR_CODE_NR_NOF_BITS);

	if ((game_state == pairing_waiting_for_detector_dev) &&
		(pair_addr == RADIO_BROADCAST_ADDRESS) &&
		(ee.device_type == pointer_dev) &&
		(other_dev_ir_code == pair_ir_code) &&
		(in_reply_to_code == pair_ir_code) &&
		(received_pair_addr = own_addr))
	{
		// It was declined because its already paired with us, as it seems. So just continue.
		UART_PRINT_P("g prapu ");
		print_int64(src_addr);
		uart_putchar(' ');
		uart_print_hex8(other_dev_ir_code);
		uart_putchar(' ');
		uart_print_hex8(in_reply_to_code);
		uart_putchar(' ');
		print_int64(received_pair_addr);
		uart_print_crlf();

		pair_addr = src_addr;
		pair_nr = received_pair_nr;

		send_pairing_req_radio_ack_nack(src_addr, in_reply_to_code, pairing_request_radio_ack);

		//set_next_alive_msg_ms(2000);
		process_other_dev_is_alive_s(ee.keep_alive_s);
	}
	else
	{
		UART_PRINT_P("g prap ");
		print_int64(src_addr);
		uart_putchar(' ');
		uart_print_hex8(in_reply_to_code);
		uart_putchar(' ');
		print_int64(received_pair_addr);
		uart_print_crlf();

		//set_next_alive_msg_ms(5000);
	}
}

// Message format here must be same as in send_pairing_req_radio_ack_nack
static void process_pairing_req_radio_ack(struct utility_r_struct *urs, uint32_t src_addr, uint16_t received_pair_addr, uint16_t received_pair_nr, int8_t radio_pipe)
{
	const uint8_t ir_code_received = utility_r_take16(urs, IR_CODE_NR_NOF_BITS);
	const uint32_t own_addr = get_own_addr();

	if ((game_state == pairing_waiting_for_pointer_dev) &&
		(received_pair_nr == pair_nr) &&
		(ee.device_type == detector_dev) &&
		(pair_addr == RADIO_BROADCAST_ADDRESS) &&
		(received_pair_addr == own_addr) &&
		// TODO (radio_pipe == radio_specific_pipe) &&
		(ir_code_received == pair_ir_code))
	{
		// We are waiting for a pairing ack, this was for us.
		UART_PRINT_P("g prrara ");
		print_int64(src_addr);
		uart_putchar(' ');
		uart_print_hex8(ir_code_received);
		uart_putchar(' ');
		print_int64(pair_addr);
		uart_print_crlf();

		pair_addr = src_addr;
		//set_next_alive_msg_ms(2000);
		process_other_dev_is_alive_s(ee.keep_alive_s);
	}
	else if ((received_pair_nr == pair_nr) &&
			(ee.device_type == detector_dev) &&
			(src_addr == pair_addr) &&
			(received_pair_addr == own_addr) &&
			(radio_pipe == radio_specific_pipe) &&
			(ir_code_received == pair_ir_code))

	{
		// We are not waiting for a pairing ack, but this was for us from our pair device.
		UART_PRINT_P("g prrarb ");
		print_int64(src_addr);
		uart_putchar(' ');
		uart_print_hex8(ir_code_received);
		uart_putchar(' ');
		print_int64(pair_addr);
		uart_putchar(' ');
		uart_print_hex8(game_state);
		uart_print_crlf();

		pair_nr = received_pair_nr;
		//set_next_alive_msg_ms(500);
		process_other_dev_is_alive_s(ee.keep_alive_s);
	}
	else
	{
		UART_PRINT_P("g prrari ");
		print_int64(src_addr);
		uart_putchar(' ');
		uart_print_hex8(ir_code_received);
		uart_putchar(' ');
		uart_print_hex8(pair_ir_code);
		uart_putchar(' ');
		uart_print_hex8(game_state);
		uart_putchar(' ');
		uart_print_hex4(ee.device_type);
		uart_putchar(' ');
		uart_print_hex8(received_pair_nr); // only last byte of 8
		uart_putchar(' ');
		uart_print_hex8(pair_nr); // only last byte of 8
		uart_print_crlf();
		//set_next_alive_msg_ms(500);
	}
}

// A pairing request was denied by someone, just log this.
// Perhaps sending/processing this message is not even needed.
// Message format here must be same as in send_pairing_req_radio_ack_nack
static void process_pairing_req_radio_nack(struct utility_r_struct *urs, uint32_t src_addr, uint32_t received_pair_addr, uint16_t received_pair_nr, int8_t radio_pipe)
{
	const uint8_t ir_code_received = utility_r_take16(urs, IR_CODE_NR_NOF_BITS);

	// TODO (radio_pipe == radio_specific_pipe) &&

	UART_PRINT_P("g prrnr ");
	print_int64(src_addr);
	uart_putchar(' ');
	print_int16(radio_pipe);
	uart_putchar(' ');
	print_int64(received_pair_addr);
	uart_putchar(' ');
	uart_print_hex8(ir_code_received);
	uart_putchar(' ');
	uart_print_hex8(pair_ir_code);
	uart_putchar(' ');
	print_int16(game_state);
	uart_putchar(' ');
	print_int16(ee.device_type);
	uart_putchar(' ');
	uart_print_hex16(received_pair_nr);
	uart_putchar(' ');
	uart_print_hex16(pair_nr);
	uart_print_crlf();

	//set_next_alive_msg_ms(3000);
}

// Received a pairing request on IR, send (broadcast) request over radio
// Message format here must be same as in process_pairing_request_radio
static void send_pairing_request_radio(uint8_t ir_code)
{
	UART_PRINT_P("g spqr ");
	uart_putchar(' ');
	uart_print_hex8(ir_code);
	uart_print_crlf();

	struct radio_data_packer packer;
	init_game_msg_hdr(&packer, pairing_request_radio);
	radio_data_packer_put16(&packer, IR_CODE_NR_NOF_BITS, ir_code);

	// TODO pair_nr should contain the ir_code so send it instead of ir_code as above?
	// radio_data_packer_put16(&packer, PAIR_NR_NOF_BITS, pair_nr);
	// radio_data_packer_put16(&packer, IR_CODE_NR_NOF_BITS, 0);

	radio_transmit_put_data(packer.buffer, sizeof(packer.buffer), RADIO_BROADCAST_ADDRESS);
}

void game_message_category_received(struct utility_r_struct *urs, uint8_t sub, uint32_t src_addr, int8_t radio_pipe)
{
	// See also radio_add_generic_message_hdr
	// The data to unpack here shall be same as data packed there.
	// Note that MSG_CATEGORY etc is already unpacked before this is called.

	uint32_t received_pair_addr = utility_r_take64(urs, ADDR_NR_NOF_BITS);
	uint16_t received_pair_time_remaining_s = utility_r_take16(urs, TIME_S_NOF_BITS);
	uint16_t received_pair_nr = utility_r_take16(urs, PAIR_NR_NOF_BITS);

	UART_PRINT_P("g m ");
	//print_int16(sub);
	//uart_putchar(' ');
	//print_int64(src_addr);
	//uart_putchar(' ');
	//print_int16(radio_pipe);
	print_int64(received_pair_addr);
	uart_putchar(' ');
	print_int16(received_pair_time_remaining_s);
	uart_putchar(' ');
	uart_print_hex16(received_pair_nr);
	uart_print_crlf();


	switch (sub)
	{
		case hit_msg:
			process_hit_msg(urs, src_addr, received_pair_addr, received_pair_nr);
			break;
		case hit_ack_msg:
			process_hit_ack_msg(urs, src_addr, received_pair_addr, received_pair_nr);
			break;
		case request_alive_msg:
		case alive_msg:
			process_alive_msg(urs, src_addr, received_pair_addr, received_pair_nr, radio_pipe, sub);
			break;
		case pairing_request_radio:
			process_pairing_request_radio(urs, src_addr, received_pair_addr, received_pair_nr, radio_pipe);
			break;
		case pairing_request_radio_ack:
			process_pairing_req_radio_ack(urs, src_addr, received_pair_addr, received_pair_nr, radio_pipe);
			break;
		case already_paired:
			process_already_paired(urs, src_addr, received_pair_addr, received_pair_nr);
			break;
		case pairing_request_radio_nack:
			process_pairing_req_radio_nack(urs, src_addr, received_pair_addr, received_pair_nr, radio_pipe);
			break;
		case goodbye_msg:
			process_goodbye_msg(urs, src_addr, received_pair_addr, received_pair_nr, radio_pipe);
			break;
		default:
			// ignore
			break;
	}
}

// Returns 1 if there was a hit on this unit.
static int8_t process_ir_char(uint8_t ir_code)
{
	uint8_t game_player_code = translate_from_ir_code(ir_code);


	/*if (ir_code != 0xFF)
	{
		UART_PRINT_P("g ir ");
		uart_print_hex8(ir_code);
		uart_putchar(' ');
		uart_print_hex8(game_code);
		uart_print_crlf();
	}
	else
	{
		return 0;
	}*/

	// Ignore signal just after own fire (if enabled)
	if (ee.ignore_time_end_ms != 0)
	{
		const int32_t t = avr_systime_ms_32();
		const int32_t dt = t - ignore_trig_time_ms;
		if ((dt>=ee.ignore_time_start_ms) && (dt<ee.ignore_time_end_ms))
		{
			UART_PRINT_PL("g ignore");
			return 0;
		}
	}

	if (game_player_code < NOF_AVAILABLE_PLAYER_CODES)
	{
		// We got hit, inform our pair device

		UART_PRINT_P("g ouch ");
		uart_print_hex8(ir_code);
		uart_putchar(' ');
		uart_print_hex8(game_player_code);
		uart_print_crlf();

		send_hit_msg(game_player_code);

		if (ee.device_type == pointer_dev)
		{
			// We are the pointer device so apply hit immediately.
			beep_led_on(ee.hit_time_third_ms);
			play_got_hit_sound();
			hit_flag = 1;
			return 1;
		}
		else
		{
			// We are not the pointer device so wait for confirmation.
		}
	}
	else if (ir_code < NOF_AVAILABLE_PAIRING_CODES)
	{
		// pairing request
		UART_PRINT_P("g pr");
		if (ee.device_type == detector_dev)
		{
			power_activity_set_time_remaining_s(5);
			send_already_paired(ir_code);
		}
		else
		{
			// Ignore
			UART_PRINT_P("g ignored ");
			uart_print_hex8(ir_code);
			uart_putchar(' ');
			uart_print_hex8(game_player_code);
			uart_print_crlf();
		}
	}
	else
	{
		switch(game_player_code)
		{
			case 0xFF:
				// just noise, ignore
				UART_PRINT_P("g ni ");
				uart_print_hex8(ir_code);
				uart_putchar(' ');
				uart_print_hex8(game_player_code);
				uart_print_crlf();
				return 0;
			case red_team_safe_zone:
			{
				switch (team)
				{
					case red_team:
						// Own safe zone
						immediate_recovery_flag = 1;
						break;
					case blue_team:
						hit_flag = 1;
						break;
					default:
						// Join red team
						team = red_team;
						break;
				}
				break;
			}
			case blue_team_safe_zone:
			{
				switch (team)
				{
					case blue_team:
						// Own safe zone
						immediate_recovery_flag = 1;
						break;
					case red_team:
						hit_flag = 1;
						break;
					default:
						// Join blue team
						team = blue_team;
						break;
				}
				break;
			}
			default:
				break;
		}
	}

	return 0;
}

static void discard_ir(void)
{
	if (!rx1_fifo_is_empty())
	{
		const uint8_t ch = rx1_fifo_take();
		UART_PRINT_P("g d1 ");
		uart_print_hex8(ch);
		uart_print_crlf();
		beep_led_on(6);
	}

	if (!rx2_fifo_is_empty())
	{
		const uint8_t ch = rx2_fifo_take();
		UART_PRINT_P("g d2 ");
		uart_print_hex8(ch);
		uart_print_crlf();
		beep_led_on(6);
	}
	hit_flag = 0;
}

// This is called when a detector device has received a pairing IR signal.
static void process_pairing_ir(uint8_t ir_code)
{
	#if 0
	// Ignore signal just after own fire (if enabled)
	// Its currently not needed during pairing since only detector devices will go here and they do not send IR.
	if (ee.ignore_time_end_ms != 0)
	{
		const int32_t t = avr_systime_ms_32();
		const int32_t dt = t - ignore_trig_time_ms;
		if ((dt>=ee.ignore_time_start_ms) && (dt<ee.ignore_time_end_ms))
		{
			UART_PRINT_PL("g ignore");
			return;
		}
	}
	#endif

	if (ir_code < NOF_AVAILABLE_PAIRING_CODES)
	{
		// This is an IR pairing request, so accept and send a radio pairing request.

		//UART_PRINT_P("g pp ");
		//uart_print_hex8(ir_code);
		//uart_print_crlf();

		// Use received ir_code as pairing_ir_code
		pair_ir_code = ir_code;

		// Pair address is not yet known (only IR so far)
		pair_addr = RADIO_BROADCAST_ADDRESS;

		// Now that we have the pairing_ir_code make a new pair check number.
		pair_nr = make_pairing_check_number();

		send_pairing_request_radio(pair_ir_code);
		power_activity_set_time_remaining_s(5);

		beep_led_on(50);
		game_timer_ms = avr_systime_ms_32() + ee.pairing_timeout_ms;
		game_state = pairing_waiting_for_pointer_dev;
	}
	else
	{
		uint8_t game_code = translate_from_ir_code(ir_code);

		if (game_code!=0xFF)
		{
			UART_PRINT_P("g pi ");
			uart_print_hex8(ir_code);
			uart_print_crlf();

			send_alive_msg(request_alive_msg);

			beep_led_on(9);
		}
		else
		{
			// noise
			UART_PRINT_P("g pn ");
			uart_print_hex8(ir_code);
			uart_print_crlf();
			beep_led_on(8);
		}
	}
}

static void take_and_process_pairing_ir(void)
{
	// Checking for codes received on IR.
	if (!rx1_fifo_is_empty())
	{
		const uint8_t ch = rx1_fifo_take();
		UART_PRINT_P("g p1 ");
		uart_print_hex8(ch);
		uart_print_crlf();
		beep_led_on(7);
		if (ee.test_options & 1)
		{
			process_pairing_ir(ch);
		}
		else
		{
			// Ignore this input.
		}
	}

	if (!rx2_fifo_is_empty())
	{
		const uint8_t ch = rx2_fifo_take();
		UART_PRINT_P("g p2 ");
		uart_print_hex8(ch);
		uart_print_crlf();
		beep_led_on(7);
		if (ee.test_options & 2)
		{
			process_pairing_ir(ch);
		}
		else
		{
			// Ignore this input.
		}
	}
}

static void take_and_process_ir(void)
{
	// Checking for codes received on IR.
	if (!rx1_fifo_is_empty())
	{
		const uint8_t ch = rx1_fifo_take();
		UART_PRINT_P("g i1 ");
		uart_print_hex8(ch);
		uart_print_crlf();
		beep_led_on(5);
		process_ir_char(ch);
	}

	if (!rx2_fifo_is_empty())
	{
		const uint8_t ch = rx2_fifo_take();
		UART_PRINT_P("g i2 ");
		uart_print_hex8(ch);
		uart_print_crlf();
		beep_led_on(5);
		process_ir_char(ch);
	}
}

static void enter_vib_state(void)
{
	beep_led_on(2*ee.hit_time_third_ms);
	VIB_ON();
	hit_flag = 0;
	immediate_recovery_flag = 0;
	UART_PRINT_PL("g vib");
	LASER_OFF();

	ammo = ee.full_ammo * AMMO_COST;

	//set_next_alive_msg(10);
	//check_send_alive_message();
	game_timer_ms = avr_systime_ms_16() + ee.hit_time_third_ms;
	game_state = game_state_vib;
}

static void enter_trigger_pulled_leds_on(const int32_t t)
{
	UART_PRINT_P("g trig");
	uart_print_hex8(fire_ir_code);
	uart_print_crlf();

	beep_fifo_clear();
	power_activity_set_time_remaining_s(ee.idle_timeout_s);
	tx1_fifo_put(fire_ir_code);
	LASER_ON();
	beep_led_on(ee.fire_time_half_ms);
	play_fire_laser_sound();
	ammo-=AMMO_COST;
	game_state = trigger_pulled_leds_on;
	game_timer_ms = t+ee.fire_time_half_ms;
	ignore_trig_time_ms = t;
}

static void enter_ready(void)
{
	const int32_t t = avr_systime_ms_32();

	VIB_OFF();
	beep_led_off();
	LASER_OFF();

	#if HW_VERSION != 0
	avr_tmr0_set_signal_strength(ee.signal_strength_percent);
	#endif

	immediate_recovery_flag = 0;
	game_timer_ms = t + 30000L;
	game_state = game_state_ready;
}

static void enter_timeout_from_other_device(void)
{
	const int32_t t = avr_systime_ms_32();

	beep_led_on(4000);
	play_unknown_event_sound();
	ammo = 0;
	send_alive_msg(request_alive_msg);
	game_timer_ms = t + 5000L;
	game_state = timeout_from_other_device;
}

static void enter_game_starting(void)
{
	const int32_t t = avr_systime_ms_32();
	ammo = ee.full_ammo * AMMO_COST;
	beep_led_on(ee.hit_time_third_ms);
	game_timer_ms = t + ee.hit_time_third_ms;
	game_state = game_starting_up;
}

static void enter_pairing_main(void)
{
	const int32_t t = avr_systime_ms_32();
	const uint8_t p = get_player_number();

	LASER_OFF();
	beep_led_on(14);
	pair_nr = -1;
	pair_addr = RADIO_BROADCAST_ADDRESS;
	pair_ir_code = translate_to_ir_pairing_code(p % NOF_AVAILABLE_PAIRING_CODES);
	play_unknown_event_sound();

	#if HW_VERSION != 0
	avr_tmr0_set_signal_strength(ee.signal_strength_pairing_percent);
	#endif

	//send_alive_msg(request_alive_msg);
	game_timer_ms = t + ee.pairing_flash_rate_ms;
	game_state = pairing_main;
}

static void enter_wait_for_alive(void)
{
	const int32_t t = avr_systime_ms_32();
	const int32_t to = ee.pairing_wait_for_alive_ms;
	LASER_OFF();
	power_activity_set_time_remaining_s(5);

	send_alive_msg(request_alive_msg);
	game_timer_ms = t + to;
	game_state = wait_for_alive;
}

static void enter_pairing_main_or_wait_for_alive(void)
{
	LASER_OFF();
	const uint8_t p = get_player_number();
	fire_ir_code = translate_to_ir_code(p % NOF_AVAILABLE_PLAYER_CODES);
	pair_ir_code = translate_to_ir_pairing_code(p % NOF_AVAILABLE_PAIRING_CODES);
	pair_addr = ee.pair_addr;
	pair_nr = ee.pairing_nr;

	if (power_is_low_battery())
	{
		beep_led_on(16);
		play_unknown_event_sound();
		enter_game_starting();
	}
	else if (pair_addr == RADIO_BROADCAST_ADDRESS)
	{
		UART_PRINT_P("g pairing ");
		uart_print_hex8(pair_ir_code);
		uart_putchar(' ');
		uart_print_hex32(pair_nr);
		uart_putchar(' ');
		uart_print_hex8(fire_ir_code);
		uart_print_crlf();

		enter_pairing_main();
	}
	else
	{
		UART_PRINT_P("g paired ");
		print_int64(pair_addr);
		uart_putchar(' ');
		uart_print_hex32(pair_nr);
		uart_putchar(' ');
		uart_print_hex8(fire_ir_code);
		uart_print_crlf();

		enter_wait_for_alive();
	}
}

void game_process(void)
{
	const int32_t t = avr_systime_ms_32();
	const int32_t d = t-game_timer_ms;
	const int8_t trig = TRIGGER_READ();

	switch(trig_state)
	{
		default:
		case 0:
			if (trig)
			{
				UART_PRINT_PL("g tp");
				trig_state = 1;
				trig_state_ms = t;
				power_activity_set_time_remaining_s(ee.idle_timeout_s);
			}
			break;
		case 1:
		{
			if (trig == 0)
			{
				UART_PRINT_PL("g tr");
				trig_state = 0;
			}
			else if (ee.off_time_ms != 0)
			{
				const int32_t trig_duration_ms = t - trig_state_ms;
				if (trig_duration_ms > ee.off_time_ms)
				{
					// Send reboot or power off message to paired device
					if (pair_addr != RADIO_BROADCAST_ADDRESS)
					{
						goodby_pair();
					}
					beep_led_on(3000);
					trig_state_ms = t;
					trig_state = 2;
				}
			}
			break;
		}
		case 2:
		case 3:
		{
			const int32_t trig_duration_ms = t - trig_state_ms;
			if (trig_duration_ms > 1000)
			{
				beep_led_on(2000);
				power_inactive();
				trig_state_ms = t;
				trig_state++;
			}
			break;
		}
		case 4:
		{
			const int32_t trig_duration_ms = t - trig_state_ms;
			if (trig_duration_ms > 1000)
			{
				// Eternal loop to let Watchdog do a reboot?
				for(;;);
			}
			break;
		}
	}

	switch (game_state)
	{
		default:
		case game_state_idle:
			UART_PRINT_PL("g start");
			discard_ir();
			enter_game_starting();
			break;
		case game_starting_up:
			discard_ir();
			if ((d >= 0) && (radio_fifo_get_free_space_in_tx_queue() != 0))
			{
				// Radio is available now, so continue.
				enter_pairing_main_or_wait_for_alive();
			}
			break;
		case pairing_main:
		{
			if (d >= 0)
			{
				if (power_get_state() != POWER_CHARGING_STATE)
				{
					beep_led_on(15);
				}
				game_timer_ms = t + ee.pairing_flash_rate_ms;
			}

			if (is_other_dev_alive())
			{
				UART_PRINT_PL("g paired");
				discard_ir();
				//set_next_alive_msg_ms(1000);
				send_alive_msg(alive_msg);
				enter_vib_state();
			}
			else if (ee.device_type == pointer_dev)
			{
				discard_ir();
				if (trig_state == 1)
				{
					UART_PRINT_P("g send ");
					uart_print_hex8(pair_ir_code);
					uart_print_crlf();

					tx1_fifo_put(pair_ir_code);
					beep_led_on(ee.pairing_timeout_ms/2);
					LASER_ON();
					play_hit_someone_sound();
					pair_nr = -1;
					pair_addr = RADIO_BROADCAST_ADDRESS;
					game_timer_ms = t + ee.pairing_timeout_ms;
					game_state = pairing_waiting_for_detector_dev;
				}
			}
			else if (ee.device_type == detector_dev)
			{
				take_and_process_pairing_ir();
			}
			break;
		}
		case pairing_waiting_for_pointer_dev:
			discard_ir();
			if (pair_addr != RADIO_BROADCAST_ADDRESS)
			{
				enter_wait_for_alive();
			}
			else if (d >= 0)
			{
				UART_PRINT_PL("g no pointer reply");
				enter_pairing_main_or_wait_for_alive();
			}
			break;

		case pairing_waiting_for_detector_dev:
			discard_ir();
			if (pair_addr != RADIO_BROADCAST_ADDRESS)
			{
				enter_wait_for_alive();
			}
			else if (d >= 0)
			{
				UART_PRINT_PL("g no detector reply");
				enter_pairing_main_or_wait_for_alive();
			}
			break;
		case wait_for_alive:
		{
			discard_ir();
			//check_send_alive_message();
			if (is_other_dev_alive())
			{
				UART_PRINT_PL("g live");
				enter_vib_state();
			}
			else if (d >= 0)
			{
				// timeout waiting for alive
				UART_PRINT_PL("g timeout");
				goodby_pair();
				enter_pairing_main_or_wait_for_alive();
			}
			break;
		}
		case game_state_vib:
		{
			// In this state hit LEDs are on and vib (if any) is on.
			take_and_process_ir();
			if (hit_flag)
			{
				enter_vib_state();
			}
			else if (immediate_recovery_flag)
			{
				UART_PRINT_PL("g im");
				enter_ready();
			}
			else if (d >= 0)
			{
				// Enter "dark" state. Dark state will last 2/3 of hit time.
				// LEDs will be on for another 1/3 of the hit time so its not entirely dark.
				UART_PRINT_PL("g prepare");
				VIB_OFF();
				beep_led_on(ee.hit_time_third_ms);
				game_timer_ms = t + (2 * ee.hit_time_third_ms);
				game_state = game_state_prepare;
			}
			break;
		}
		case game_state_prepare:
		{
			// Just an intermediate state between vib and ready.
			take_and_process_ir();
			if (hit_flag)
			{
				enter_vib_state();
			}
			else if ((immediate_recovery_flag) || (d >= 0))
			{
				UART_PRINT_PL("g ready");
				enter_ready();
			}
			break;
		}
		case game_state_ready:
		{
			take_and_process_ir();
			if (!is_other_dev_alive())
			{
				UART_PRINT_PL("g timeout");
				enter_timeout_from_other_device();
			}
			else if (hit_flag)
			{
				enter_vib_state();
			}
			else if ((trig_state == 1) && (ammo>(AMMO_COST-1)))
			{
				enter_trigger_pulled_leds_on(t);
			}
			else if (pair_addr == RADIO_BROADCAST_ADDRESS)
			{
				UART_PRINT_PL("g epm");
				enter_pairing_main_or_wait_for_alive();
			}
			else
			{
				LASER_OFF();
			}
			break;
		}
		case trigger_pulled_leds_on:
		{
			take_and_process_ir();
			if (hit_flag)
			{
				enter_vib_state();
			}
			else if (d >= 0)
			{
				UART_PRINT_PL("g off");
				beep_led_off();
				LASER_OFF();
				game_timer_ms = t+ee.fire_time_half_ms;
				game_state = trigger_pulled_leds_off;
			}
			break;
		}
		case trigger_pulled_leds_off:
		{
			take_and_process_ir();
			if (hit_flag)
			{
				enter_vib_state();
			}
			else if ((trig_state != 1) && (!ee.autofire))
			{
				UART_PRINT_PL("g rel");
				game_state = trigger_released;
			}
			else if (d >= 0)
			{
				UART_PRINT_PL("g auto");
				enter_ready();
			}
			break;
		}
		case trigger_released:
			take_and_process_ir();
			if (hit_flag)
			{
				enter_vib_state();
			}
			else if (d >= 0)
			{
				UART_PRINT_PL("g ready");
				enter_ready();
			}
			break;
		case timeout_from_other_device:
		{
			discard_ir();
			LASER_OFF();
			if (is_other_dev_alive())
			{
				UART_PRINT_PL("g recon");
				enter_vib_state();
			}
			else if (d >= 0)
			{
				// Start over with pairing procedure
				UART_PRINT_PL("g try again");
				goodby_pair();
				enter_pairing_main_or_wait_for_alive();
			}
			break;
		}
	}

	/*if (game_state != log_game_state)
	{
		UART_PRINT_P("g s ");
		uart_print_hex4(game_state);
		uart_print_crlf();
		log_game_state = game_state;
	}*/

}

void game_tick_s(void)
{
	const uint16_t full = ee.full_ammo;

	if (ammo < (full*AMMO_COST))
	{
		ammo++;
	}

	// Keep contact with pair device.
	if (other_dev_is_alive_s >= 5)
	{
		other_dev_is_alive_s--;
		if ((ee.device_type == detector_dev) && (power_activity_time_remaining_s() <= 4))
		{
			send_alive_msg(request_alive_msg);
		}
	}
	else if (other_dev_is_alive_s == 0)
	{
		// do nothing
	}
	else if (other_dev_is_alive_s >= 2)
	{
		other_dev_is_alive_s--;
		send_alive_msg(request_alive_msg);
	}
	else if (other_dev_is_alive_s == 1)
	{
		other_dev_is_alive_s--;
		// TODO goodby_pair();
	}



	/*if (ee.device_type == detector_dev)
	{
		check_send_alive_message();
	}*/
}


int8_t game_get_state(void)
{
	return game_state;
}

uint16_t is_other_dev_is_alive_s(void)
{
	return other_dev_is_alive_s;
}

uint32_t game_pair_addr(void)
{
	return pair_addr;
}

int64_t game_pair_nr(void)
{
	return pair_nr;
}

#endif

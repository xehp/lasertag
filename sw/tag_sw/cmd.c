/*
cmd.h

Provide command line functions.

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

2021-05-01 Created. /Henrik
*/

#include <stdint.h>
#include "avr_cfg.h"
#include "avr_sys.h"
#include "avr_uart.h"
#include "avr_eeprom.h"
#include "avr_tmr0.h"
#include "avr_ports.h"
#include "errlog.h"
#include "utility.h"
#include "power.h"
#include "radio.h"
#include "game.h"
#include "version.h"
#include "beep.h"
#include "msg.h"
#include "cmd.h"

// macro used for extra debugging
#define D(x) x

#define LOG_INT16_P(str, i) {log_int16_p(PSTR(str), i);}
#define LOG_INT64_P(str, i) {log_int64_p(PSTR(str), i);}


char command_buffer[32+1];
uint8_t command_length = 0;


static void log_int16_p(const char *pgm_addr, int16_t i)
{
	uart_print_P(pgm_addr);
	uart_putchar(' ');
	print_int16(i);
	uart_print_crlf();
}

static void log_int64_p(const char *pgm_addr, int64_t i)
{
	uart_print_P(pgm_addr);
	uart_putchar(' ');
	print_int64(i);
	uart_print_crlf();
}

static const char* skip_non_space(const  char* ptr)
{
	while(*ptr > ' ')
	{
		ptr++;
	}
	return ptr;
}

static const char* skip_space(const char* ptr)
{
	while(*ptr==' ')
	{
		ptr++;
	}
	return ptr;
}

static const char* skip_to_next(const char* ptr)
{
	ptr=skip_space(ptr);
	ptr=skip_non_space(ptr);
	return ptr;
}


enum
{
	par_unknown = 0,
	par_device_type,
	par_fire_time_ms,
	par_ignore_time_start_ms,
	par_ignore_time_end_ms,
	par_idle_timeout_s,
	par_short_led_blink_ms,
	par_microVoltsPerUnit,
	par_player_number,
	par_off_time_ms,
	par_stop_charging_battery_at_mv,
	par_signal_strength_percent,
	par_serial_nr,
	par_volume_percent,
	par_game_state,
	par_game_voltage,
	par_power_state,
	par_hw_version,
	par_keep_alive_s,
	par_expected_swhw,
	par_full_ammo,
	par_vmajor,
	par_vminor,
	par_vdebug,
	par_ee_pairing_nr,
	par_pairing_timeout_ms,
	par_pairing_flash_rate_ms,
	par_pair_addr,
	par_is_other_dev_is_alive_ms,
	par_sys_time_ms,
	par_radio_state,
	par_game_pair_addr,
	par_game_pair_nr,
	par_autofire,
	par_detect_margin_mv,
	par_game_code,
	par_power_activity_time_remaining_s,
	par_low_battery_warning_mv,
	par_battery_depleated_at_mv,
	par_signal_strength_pairing_percent,
	par_OCR0B,
	par_trigger_read,
	par_internal_ir_read,
	par_external_ir_read,
	par_test_options,
	par_led_on_time_after_hit_ms,
	par_led_off_time_after_hit_ms,
	par_max_number // This must be last (its not a parameter).
};

static int8_t get_par_id(const char *parname)
{
	while(*parname==' ')
	{
		parname++;
	}

	switch(*parname)
	{
		#ifdef RADIO_MODEM_ONLY
		case 'a': return par_autofire;
		case 'd': return par_device_type;
		case 'e': return par_expected_swhw;
		case 'f': return par_fire_time_ms;
			switch(parname[1])
			{
				case 'a': return par_full_ammo;
				case 't': return par_fire_time_ms;
				default: return par_unknown;
			}
			break;
		case 'g':
			switch(parname[1])
			{
				case 'a': return par_is_other_dev_is_alive_ms;
				case 'c': return par_game_code;
				case 's': return par_game_state;
				case 'p': return par_game_pair_addr;
				case 'n': return par_game_pair_nr;
				default: return par_unknown;
			}
			break;
		case 'h': return par_short_led_blink_ms;
		case 'i':
			switch(parname[1])
			{
				case 's': return par_ignore_time_start_ms;
				case 'e': return par_ignore_time_end_ms;
				case 'd': return par_idle_timeout_s;
				default: return par_unknown;
			}
			break;
		/*case 'i':
			LOG_INT16_P("id", ee.ID);
			break;*/
		case 'k': return par_keep_alive_s;
		case 'm': return par_microVoltsPerUnit;
		case 'n': return par_player_number;
		case 'o': return par_off_time_ms;
		case 'p':
			switch(parname[1])
			{
				case 'a': return par_pair_addr;
				case 'n': return par_ee_pairing_nr;
				case 'o': return par_power_state;
				case 't': return par_pairing_timeout_ms;
				case 'f': return par_pairing_flash_rate_ms;
				default: return par_unknown;
			}
			break;
		case 'r': return par_radio_state;
		case 's':
			switch(parname[1])
			{
				case 't': return par_stop_charging_battery_at_mv;
				case 'i': return par_signal_strength_percent;
				case 'n': return par_serial_nr;
				default: return par_unknown;
			}
			break;
		case 't': return par_sys_time_ms;
		case 'v':
		{
			switch(parname[1])
			{
				case 'h': return par_hw_version;
				case 'm':
					switch(parname[2])
					{
						case 'a': return par_vmajor;
						case 'i': return par_vminor;
						default: return par_unknown;
						break;
					}
				case 'd': return par_vdebug;
				case 'o':
					switch(parname[3])
					{
						case 't': return par_game_voltage;
						case 'u': return par_volume_percent;
						default: return par_unknown;
					}
					break;
				default: return par_unknown;
			}
			break;
		}
		#endif
		default:
			if ((*parname >= '0') && (*parname <= '9'))
			{
				return utility_atoll(parname);
			}
			break;
	}
	return par_unknown;
}

static int64_t get_par_value(uint8_t par)
{
	switch(par)
	{
		case par_device_type: return ee.device_type;
		case par_fire_time_ms: return ee.fire_time_half_ms;
		case par_ignore_time_start_ms: return ee.ignore_time_start_ms;
		case par_ignore_time_end_ms: return ee.ignore_time_end_ms;
		case par_idle_timeout_s: return ee.idle_timeout_s;
		case par_short_led_blink_ms: return ee.short_led_blink_ms;
		case par_microVoltsPerUnit: return ee.microVoltsPerUnit;
		case par_player_number: return ee.player_number;
		case par_off_time_ms: return ee.off_time_ms;
		case par_stop_charging_battery_at_mv: return ee.stop_charging_battery_at_mv;
		case par_signal_strength_percent: return ee.signal_strength_percent;
		case par_serial_nr:	return ee.serial_nr;
		case par_volume_percent: return ee.audio_volume_percent;
		case par_game_state: return game_get_state();
		case par_game_voltage: return power_get_voltage_mV();
		case par_power_state: return power_get_state();
		case par_hw_version: return HW_VERSION;
		case par_keep_alive_s: return ee.keep_alive_s;
		case par_expected_swhw:	return ee.expected_swhw;
		case par_full_ammo:	return ee.full_ammo;
		case par_vmajor: return VERSION_MAJOR;
		case par_vminor: return VERSION_MINOR;
		case par_vdebug: return VERSION_DEBUG;
		case par_ee_pairing_nr: return ee.pairing_nr;
		case par_pairing_timeout_ms: return ee.pairing_timeout_ms;
		case par_pairing_flash_rate_ms: return ee.pairing_flash_rate_ms;
		case par_pair_addr: return ee.pair_addr;
		case par_is_other_dev_is_alive_ms: return is_other_dev_is_alive_s();
		case par_sys_time_ms: return avr_systime_ms_64();
		case par_radio_state: return radio_state();
		case par_game_pair_addr: return game_pair_addr();
		case par_game_pair_nr: return game_pair_nr();
		case par_autofire: return ee.autofire;
		case par_detect_margin_mv: return ee.detect_margin_mv;
		case par_game_code: return ee.game_code;
		case par_power_activity_time_remaining_s: return power_activity_time_remaining_s();
		case par_low_battery_warning_mv: return ee.low_battery_warning_mv;
		case par_battery_depleated_at_mv: return ee.battery_depleated_at_mv;
		case par_signal_strength_pairing_percent: return ee.signal_strength_pairing_percent;
		case par_OCR0B: return tmr0_get_OCR0B();
		case par_trigger_read:return TRIGGER_READ();
		case par_internal_ir_read: return INTERNAL_IR_READ();
		case par_external_ir_read: return EXTERNAL_IR_READ();
		case par_test_options: return ee.test_options;
		case par_led_on_time_after_hit_ms: return ee.led_on_time_after_hit_ms;
		case par_led_off_time_after_hit_ms: return ee.led_off_time_after_hit_ms;

		default: return 0;
	}
}

static void set_par_value(uint8_t par, int64_t val)
{
	switch(par)
	{
		case par_device_type: ee.device_type = val;	break;
		case par_fire_time_ms: ee.fire_time_half_ms = val; break;
		case par_ignore_time_start_ms: ee.ignore_time_start_ms = val; break;
		case par_ignore_time_end_ms: ee.ignore_time_end_ms = val; break;
		case par_idle_timeout_s: ee.idle_timeout_s = val; break;
		case par_short_led_blink_ms: ee.short_led_blink_ms = val; break;
		case par_microVoltsPerUnit: ee.microVoltsPerUnit = val; break;
		case par_player_number: ee.player_number = val;	break;
		case par_off_time_ms: ee.off_time_ms = val; break;
		case par_stop_charging_battery_at_mv: ee.stop_charging_battery_at_mv = val; break;
		case par_signal_strength_percent: ee.signal_strength_percent = val; break;
		case par_serial_nr:	ee.serial_nr = val;	break;
		case par_volume_percent: ee.audio_volume_percent = val;	break;
		case par_keep_alive_s: ee.keep_alive_s = val; break;
		case par_expected_swhw: ee.expected_swhw = val;	break;
		case par_full_ammo:	ee.full_ammo = val;	break;
		case par_ee_pairing_nr: ee.pairing_nr = val; break;
		case par_pairing_timeout_ms: ee.pairing_timeout_ms = val; break;
		case par_pairing_flash_rate_ms: ee.pairing_flash_rate_ms = val;	break;
		case par_pair_addr: ee.pair_addr = val; break;
		case par_autofire: ee.autofire = val; break;
		case par_detect_margin_mv: ee.detect_margin_mv = val; break;
		case par_game_code: ee.game_code = val; break;
		case par_power_activity_time_remaining_s: power_activity_set_time_remaining_s(val); break;
		case par_low_battery_warning_mv: ee.low_battery_warning_mv = val; break;
		case par_battery_depleated_at_mv: ee.battery_depleated_at_mv = val; break;
		case par_signal_strength_pairing_percent: ee.signal_strength_pairing_percent = val; break;
		case par_test_options: ee.test_options = val; break;
		case par_led_on_time_after_hit_ms: ee.led_on_time_after_hit_ms = val; break;
		case par_led_off_time_after_hit_ms: ee.led_off_time_after_hit_ms = val; break;
		default: UART_PRINT_PL("read only"); break;
	}
}

#ifdef RADIO_MODEM_ONLY

//void radio_data_packer_init(struct radio_data_packer *packer, uint8_t msg_cat, uint8_t sub_cmd);
//void radio_data_packer_put16(struct radio_data_packer *packer, uint8_t nof_bits_to_write, uint16_t d);
//void radio_data_packer_put64(struct radio_data_packer *packer, uint8_t nof_bits_to_write, uint64

// The message format must match that in interpret_bin_get
static void make_getset_par_message(int32_t dest_addr, uint8_t sub, uint8_t par, int64_t val)
{
	struct radio_data_packer packer;
	radio_data_packer_init(&packer, msg_category_cmd, sub, radio_take_sequence_number());
	radio_data_packer_put16(&packer, PAR_ID_NOF_BITS, par);
	radio_data_packer_put64(&packer, VALUE_NOF_BITS, val);
	radio_transmit_put_data(packer.buffer, sizeof(packer.buffer), dest_addr);
}

static void make_get_par_message(int32_t dest_addr, uint8_t par)
{
	make_getset_par_message(dest_addr, cmd_get, par, 0);
}

static void make_set_par_message(int32_t dest_addr, uint8_t par, int64_t val)
{
	make_getset_par_message(dest_addr, cmd_set, par, val);
}

// Message format here must be same as in interpret_bin_power_off
static void make_power_off_message(int32_t dest_addr)
{
	struct radio_data_packer packer;
	radio_data_packer_init(&packer, msg_category_cmd, cmd_power_off, radio_take_sequence_number());
	radio_transmit_put_data(packer.buffer, sizeof(packer.buffer), dest_addr);
}

// See also game_make_send_reboot_message in game.c
static void make_send_reboot_message(int32_t dest_addr)
{
	struct radio_data_packer packer;
	radio_data_packer_init(&packer, msg_category_cmd, cmd_reboot, radio_take_sequence_number());
	radio_transmit_put_data(packer.buffer, sizeof(packer.buffer), dest_addr);
}

// Message format here must be same as in interpret_bin_ping
static void make_ping_message(int32_t dest_addr, uint16_t frequency_Hz, uint16_t duration, uint8_t volume)
{
	struct radio_data_packer packer;
	radio_data_packer_init(&packer, msg_category_cmd, cmd_ping, radio_take_sequence_number());

	radio_data_packer_put16(&packer, FREQUENCY_NOF_BITS, frequency_Hz);
	radio_data_packer_put16(&packer, DURATION_NOF_BITS, duration);
	radio_data_packer_put16(&packer, VOLUME_NOF_BITS, volume);

	//LOG_INT64_P("c ping serial_nr ", dest_addr);

	radio_transmit_put_data(packer.buffer, sizeof(packer.buffer), dest_addr);
}

static void make_send_save_message(uint32_t address)
{
	struct radio_data_packer packer;
	radio_data_packer_init(&packer, msg_category_cmd, cmd_save, radio_take_sequence_number());

	// There is plenty of room left in message so just put something?
	//radio_data_packer_put64(&packer, SERIAL64_NR_NOF_BITS, ee.serial_nr);

	radio_transmit_put_data(packer.buffer, sizeof(packer.buffer), address);
}

static void interpret_send_save(const char *ptr)
{
	#ifdef RADIO_MODEM_ONLY
	ptr=skip_space(ptr);
	uint32_t addr = utility_atoll(ptr);
	ptr=skip_non_space(ptr);
	#endif

	make_send_save_message(addr);
}
#endif

static void interpret_get(const char *ptr)
{
	const int par = get_par_id(ptr);

	if (par == 0)
	{
		UART_PRINT_PL("par?");
		return;
	}

	const int64_t val = get_par_value(par);
	LOG_INT16_P("par ", par);
	LOG_INT64_P("val ", val);
}

#ifdef RADIO_MODEM_ONLY
static void interpret_send_get(const char *ptr)
{
	ptr=skip_space(ptr);
	uint32_t addr = utility_atoll(ptr);
	ptr=skip_non_space(ptr);

	const int par = get_par_id(ptr);

	if (par == 0)
	{
		UART_PRINT_PL("par?");
		return;
	}

	UART_PRINT_P("c g ");
	print_int64(addr);
	uart_putchar(' ');
	print_int16(par);
	uart_print_crlf();

	make_get_par_message(addr, par);
}
#endif

static void interpret_set(const char *ptr)
{
	ptr=skip_space(ptr);
	const int par = get_par_id(ptr);
	if (par == 0)
	{
		UART_PRINT_PL("par?");
		return;
	}
	LOG_INT16_P("par ", par);
	ptr=skip_non_space(ptr);

	//uart_print(ptr);

	const int64_t val = utility_atoll(ptr);

	LOG_INT64_P("val ", val);

	set_par_value(par, val);
}

#ifdef RADIO_MODEM_ONLY
static void interpret_send_set(const char *ptr)
{
	ptr=skip_space(ptr);
	uint32_t addr_nr = utility_atoll(ptr);
	ptr=skip_non_space(ptr);

	ptr=skip_space(ptr);
	const int par = get_par_id(ptr);
	if (par == 0)
	{
		UART_PRINT_PL("par?");
		return;
	}
	LOG_INT16_P("par ", par);
	ptr=skip_non_space(ptr);

	const int64_t val = utility_atoll(ptr);

	LOG_INT64_P("val ", val);

	make_set_par_message(addr_nr, par, val);
}
#endif

static void interpret_power_off(const char *ptr)
{
	ERRLOG_P("power off", __LINE__);
	power_inactive();
}

#ifdef RADIO_MODEM_ONLY
static void interpret_send_power_off(const char *ptr)
{
	ptr=skip_space(ptr);
	uint32_t addr = utility_atoll(ptr);
	ptr=skip_non_space(ptr);
	make_power_off_message(addr);
}

static void interpret_send_reboot(const char *ptr)
{
	ptr=skip_space(ptr);
	uint32_t addr = utility_atoll(ptr);
	ptr=skip_non_space(ptr);
	make_send_reboot_message(addr);
}
#endif

static void ping(uint16_t frequency_Hz, uint16_t duration_ms, uint8_t volume_percent)
{
	uint16_t tone_period = beep_tonecode_from_frequency(frequency_Hz);
	beep_fifo_put(tone_period, duration_ms, volume_percent);
	beep_led_on(duration_ms);
	//LOG_INT64_P("ping dur ", duration_ms);
}

#ifdef RADIO_MODEM_ONLY
static void interpret_send_ping(const char *ptr)
{
	// Need to know who to send the ping to.
	const uint32_t serial_nr = utility_atoll(ptr);
	ptr=skip_to_next(ptr);

	uint16_t frequency_Hz = utility_atoll(ptr);
	ptr=skip_to_next(ptr);
	uint16_t duration_ms = utility_atoll(ptr);
	ptr=skip_to_next(ptr);
	uint8_t volume_percent = utility_atoll(ptr);

	make_ping_message(serial_nr, frequency_Hz, duration_ms, volume_percent);
}
#endif

static void interpret_tone(const char *ptr)
{
	ptr=skip_space(ptr);

	const int64_t val = utility_atoll(ptr);

	LOG_INT64_P("tone ", val);

	beep_fifo_put(val, 0, ee.audio_volume_percent);
}

static void make_send_bin_getset_reply(uint32_t address, uint8_t par, int64_t value, uint8_t cat, uint8_t sub, uint8_t seq_nr)
{
	struct radio_data_packer packer;
	radio_data_packer_init(&packer, cat, cmd_set, seq_nr);

	radio_data_packer_put16(&packer, PAR_ID_NOF_BITS, par);
	radio_data_packer_put64(&packer, VALUE_NOF_BITS, value);

	radio_transmit_put_data(packer.buffer, sizeof(packer.buffer), address);
}

static void interpret_bin_set(struct utility_r_struct *urs, int32_t address, uint8_t seq_nr)
{
	const uint8_t par = utility_r_take16(urs, PAR_ID_NOF_BITS);

	if ((par == 0) || (par > par_max_number))
	{
		UART_PRINT_PL("par?");
		return;
	}

	const int64_t val = utility_r_take64(urs, VALUE_NOF_BITS);

	LOG_INT64_P("val ", val);

	if ((par == par_microVoltsPerUnit) || (par == par_serial_nr))
	{
		// Not allowed to set this over radio.
		UART_PRINT_PL("illegal");
		const int64_t tmp = get_par_value(par);
		make_send_bin_getset_reply(address, par, tmp, msg_category_reply_nok, cmd_set, seq_nr);
	}
	else
	{
		set_par_value(par, val);
		const int64_t tmp = get_par_value(par);
		make_send_bin_getset_reply(address, par, tmp, msg_category_reply_ok, cmd_set, seq_nr);
	}

}


static void interpret_bin_get(struct utility_r_struct *urs, int32_t address, uint8_t seq_nr)
{
	const uint8_t par = utility_r_take16(urs, PAR_ID_NOF_BITS);

	if ((par == 0) || (par > par_max_number))
	{
		UART_PRINT_PL("par?");
		return;
	}

	const int64_t tmp = get_par_value(par);

	LOG_INT64_P("val ", tmp);

	make_send_bin_getset_reply(address, par, tmp, msg_category_reply_ok, cmd_get, seq_nr);
}

static void make_send_bin_reply(int32_t address, uint8_t sub, uint8_t seq_nr)
{
	struct radio_data_packer packer;
	radio_data_packer_init(&packer, msg_category_reply_ok, sub, seq_nr);

	// There is plenty of room left in message so just put something?
	//radio_data_packer_put64(&packer, SERIAL64_NR_NOF_BITS, ee.serial_nr);

	radio_transmit_put_data(packer.buffer, sizeof(packer.buffer), address);
}

// Message format here must be same as in make_ping_message
static void interpret_bin_ping(struct utility_r_struct *urs, uint32_t address, uint8_t seq_nr)
{
	const uint16_t f = utility_r_take64(urs, FREQUENCY_NOF_BITS);
	const uint16_t d = utility_r_take64(urs, DURATION_NOF_BITS);
	const uint8_t v = utility_r_take64(urs, VOLUME_NOF_BITS);

	UART_PRINT_P("c ping ");
	print_int16(f);
	uart_putchar(' ');
	print_int16(d);
	uart_putchar(' ');
	print_int16(v);
	uart_print_crlf();

	ping(f,d,v);

	make_send_bin_reply(address, cmd_ping, seq_nr);

}

// Message format here must be same as in make_send_save_message
static void interpret_bin_save(struct utility_r_struct *urs, uint32_t address, uint8_t seq_nr)
{
	UART_PRINT_P("c save ");
	uart_print_crlf();
	eepromSave();
	make_send_bin_reply(address, cmd_save, seq_nr);
}


static void log_bin_getset(struct utility_r_struct *urs)
{
	const uint8_t par = utility_r_take16(urs, PAR_ID_NOF_BITS);
	const uint64_t val = utility_r_take64(urs, VALUE_NOF_BITS);

	uart_putchar(' ');
	print_int16(par);
	uart_putchar(' ');
	print_int64(val);
}

static void log_bin_ping(struct utility_r_struct *urs)
{
	const uint64_t serial = utility_r_take64(urs, SERIAL64_NR_NOF_BITS);

	uart_putchar(' ');
	print_int64(serial);
}

static void log_bin_cmd_or_reply_message(struct utility_r_struct *urs, uint8_t cat, uint8_t sub, uint32_t src_addr)
{
	if (uart_get_free_space_in_write_buffer() > (UART_TX_BUFFER_SIZE/4))
	{

		UART_PRINT_P("c reply ");
		print_int16(cat);
		uart_putchar(' ');
		print_int16(sub);
		uart_putchar(' ');
		print_int64(src_addr);

		switch(sub)
		{
			case cmd_get:
			case cmd_set:
			{
				log_bin_getset(urs);
				break;
			}
			case cmd_power_off:
			{
				UART_PRINT_P(" off");
				break;
			}
			case cmd_ping:
			{
				UART_PRINT_P(" ping");
				log_bin_ping(urs);
				break;
			}
			case cmd_save:
			{
				UART_PRINT_P(" save");
				break;
			}
			default:
				UART_PRINT_P(" ?");
				break;
		}

		uart_print_crlf();
	}
	else
	{
		uart_putchar('#');
	}
}

// See also make_hello_message
static void log_bin_hello(struct utility_r_struct *urs)
{
	const int64_t serial_nr = utility_r_take64(urs, SERIAL64_NR_NOF_BITS);

	uart_putchar(' ');
	print_int64(serial_nr);
}

static void log_bin_status_message(struct utility_r_struct *urs, uint8_t sub, uint32_t src_addr)
{
	if (uart_get_free_space_in_write_buffer() > (UART_TX_BUFFER_SIZE/2))
	{
		UART_PRINT_P("c status ");
		print_int16(sub);
		uart_putchar(' ');
		print_int64(src_addr);

		switch(sub)
		{
			case status_hello:
			{
				log_bin_hello(urs);
				break;
			}
			default:
				UART_PRINT_P(" ?");
				return;
		}

		uart_print_crlf();
	}
	else
	{
		uart_putchar('#');
	}

}

// Decode a Hex buffer of 16 bytes (or RADIO_PAYLOAD_MAX_LEN)
// and send it on radio
static void interpret_send_message_from_hex(const char *ptr)
{
	uint8_t buf[RADIO_PAYLOAD_MAX_LEN];
	int32_t addr = utility_atoll(ptr);
	ptr = skip_to_next(ptr);
	uint8_t n = utility_decode_hexstr(ptr, buf, sizeof(buf));
	radio_transmit_put_data(buf, n, addr);
}

static void interpret_command(void)
{
	char* ptr1 = command_buffer;

	while(*ptr1==' ')
	{
		ptr1++;
	}

	const char* ptr2 = skip_to_next(ptr1);

	switch (*ptr1)
	{
#ifdef RADIO_MODEM_ONLY
		case 'E':
		{
			interpret_send_save(ptr2);
			break;
		}
		case 'G':
		{
			interpret_send_get(ptr2);
			break;
		}
		case 'O':
		{
			interpret_send_power_off(ptr2);
			break;
		}
		case 'P':
		{
			interpret_send_ping(ptr2);
			break;
		}
		case 'R':
		{
			interpret_send_reboot(ptr2);
			break;
		}
		case 'S':
		{
			interpret_send_set(ptr2);
			break;
		}
#endif
		case 'e':
		{
			eepromSave();
			break;
		}
		case 'g':
		{
			interpret_get(ptr2);
			break;
		}
		case 'm':
		{
			interpret_send_message_from_hex(ptr2);
			break;
		}
		case 'o':
		{
			interpret_power_off(ptr2);
			break;
		}
		case 'r':
		{
			uart_print_PL(PSTR("reboot"));
			// Do eternal loop until we get reset by WTD.
			for(;;);
			break;
		}
		case 's':
		{
			interpret_set(ptr2);
			break;
		}
		case 't':
		{
			interpret_tone(ptr2);
			break;
		}
		default:
			UART_PRINT_PL("?");
			break;
	}
}

static void send_bin_reply(uint32_t address, uint8_t seq_nr, uint8_t cmd)
{
	struct radio_data_packer packer;
	radio_data_packer_init(&packer, msg_category_reply_ok, cmd_power_off, seq_nr);

	radio_transmit_put_data(packer.buffer, sizeof(packer.buffer), address);
}

static void interpret_bin_power_off(uint32_t address, uint8_t seq_nr)
{
	send_bin_reply(address, seq_nr, cmd_power_off);
	power_inactive();

	// When trying it den not go power down by calling power_inactive.
	// Perhaps it had not detected that it was on battery power yet?
	// Try reboot instead.
}

static void interpret_bin_reboot(uint32_t address, uint8_t seq_nr)
{
	send_bin_reply(address, seq_nr, cmd_reboot);
	power_inactive();

	// Eternal loop until WD
	for(;;);
}

static void interpret_bin_command(struct utility_r_struct *urs, uint8_t sub, uint32_t src_addr, uint8_t seq_nr)
{

	UART_PRINT_P("c bin ");
	print_int16(sub);
	uart_putchar(' ');
	print_int64(src_addr);
	uart_print_crlf();

	switch(sub)
	{
		case cmd_set:
		{
			interpret_bin_set(urs, src_addr, seq_nr);
			break;
		}
		case cmd_get:
		{
			interpret_bin_get(urs, src_addr, seq_nr);
			break;
		}
		case cmd_power_off:
		{
			interpret_bin_power_off(src_addr, seq_nr);
			break;
		}
		case cmd_reboot:
		{
			interpret_bin_reboot(src_addr, seq_nr);
			break;
		}
		case cmd_ping:
		{
			interpret_bin_ping(urs, src_addr, seq_nr);
			break;
		}
		case cmd_save:
		{
			interpret_bin_save(urs, src_addr, seq_nr);
			break;
		}
		default:
			UART_PRINT_PL("?");
			return;
	}
}

static void interpret_bin_reply(struct utility_r_struct *urs, uint8_t sub, uint32_t src_addr)
{
	log_bin_cmd_or_reply_message(urs, msg_category_reply_ok, sub, src_addr);
}

// Unpacking here must match the packing made in radio_data_packer_init
// Note msg_cat shall be unpacked already.
// TODO Add a magic number also. There should be 8 bits available for that.
static void interpret_std_bin_msg(struct utility_r_struct *urs, uint8_t msg_cat, int8_t radio_pipe)
{
	const uint8_t sub = utility_r_take16(urs, MSG_SUBCMD_SIZE_NOF_BITS);
	const uint8_t seq_nr = utility_r_take16(urs, SEQUENCE_NUMBER_NOF_BITS);
	const uint32_t src_addr = utility_r_take64(urs, ADDR_NR_NOF_BITS);

	/*UART_PRINT_P("c m ");
	print_int16(radio_pipe);
	uart_putchar(' ');
	print_int64(src_addr);
	uart_putchar(' ');
	print_int16(msg_cat);
	uart_putchar(' ');
	print_int16(sub);
	uart_print_crlf();*/

	switch (msg_cat)
	{
		case msg_category_ignore:
			UART_PRINT_P("c mi");
			break;
		case msg_category_log:
			UART_PRINT_P("c ml");
			break;
		case msg_category_reply_nok:
			UART_PRINT_P("c nok");
			break;
		case msg_category_game:
			game_message_category_received(urs, sub, src_addr, radio_pipe);
			break;
		case msg_category_status:
			log_bin_status_message(urs, sub, src_addr);
			break;
		case msg_category_cmd:
			interpret_bin_command(urs, sub, src_addr, seq_nr);
			break;
		case msg_category_reply_ok:
			interpret_bin_reply(urs, sub, src_addr);
			break;
		default:
			// unknown message category
			UART_PRINT_P("c c");
			print_int16(radio_pipe);
			uart_putchar(' ');
			print_int64(src_addr);
			uart_putchar(' ');
			print_int16(msg_cat);
			uart_putchar(' ');
			print_int16(sub);
			uart_print_crlf();
			break;
	}
}

// See also radio_data_packer_init where the header is packed.
static void bin_msg_received(const uint8_t *msg, uint8_t n, int8_t radio_pipe)
{
	struct utility_r_struct urs;

	utility_r_init(&urs, msg, n*8);

	const uint8_t msg_cat = utility_r_take16(&urs, MSG_CATEGORY_SIZE_NOF_BITS);

	//const int16_t a = utility_r_nof_bits_available(&urs);

	/*UART_PRINT_P("c r ");
	print_int16(n);
	uart_putchar(' ');
	print_int16(radio_pipe);
	uart_putchar(' ');
	print_int16(msg_cat);
	uart_putchar(' ');
	print_int16(a);
	uart_print_crlf();*/

	switch (msg_cat)
	{
		case msg_category_ignore:
		case msg_category_log:
		case msg_category_reply_nok:
		case msg_category_game:
		case msg_category_status:
		case msg_category_cmd:
		case msg_category_reply_ok:
			interpret_std_bin_msg(&urs, msg_cat, radio_pipe);
			break;
		default:
			UART_PRINT_P("c u ");
			print_int16(radio_pipe);
			uart_putchar(' ');
			print_int16(msg_cat);
			uart_print_crlf();
			break;
	}
}

void cmd_init()
{
	command_length=0;
}

void cmd_process()
{
	// Check for messages from serial port
	const int ch = uart_getchar();
	if (ch >= 0)
	{
		if (ch>=' ')
		{
			if (command_length>=sizeof(command_buffer)-1)
			{
				UART_PRINT_PL("?");
				command_length=0;
			}
			else
			{
				command_buffer[command_length] = ch;
				command_length++;
			}
		}
		else if ((ch == '\n') || (ch == '\r'))
		{
			command_buffer[command_length] = 0;
			interpret_command();
			command_length = 0;
		}
		else if (ch == 0x1b)
		{
			// esc, do eternal loop until we get reset by WTD.
			// TODO Should we do RELAY_OFF() here?
			for(;;);
		}
		else
		{
			uart_print_hex8(ch);
			command_length = 0;
		}
	}

	// This is how we check for new messages from radio.
	uint8_t tmp[RADIO_PAYLOAD_MAX_LEN] = {0};
	const int8_t pipe = radio_receive_take_data(tmp, sizeof(tmp));
	if (pipe != radio_pipe_empty)
	{
		/*UART_PRINT_P("cr ");
		for (uint8_t i=0; i<sizeof(tmp); ++i)
		{
			uart_print_hex8(tmp[i]);
		}
		uart_print_crlf();*/
		bin_msg_received(tmp, sizeof(tmp), pipe);
	}
}

#ifdef RADIO_MODEM_ONLY
static uint8_t count = 0;
void cmd_init_tick_s(void)
{
	if (radio_fifo_get_free_space_in_tx_queue())
	{
		/*switch(count)
		{
			case 15:interpret_send_ping("5 350 100 10"); break;
			case 25:interpret_send_ping("8 350 100 10"); break;
			case 45:interpret_send_get("5 a"); break;
			default:break;
		}*/

		if (count>=60)
		{
			count=0;
		}
		else
		{
			count++;
		}
	}
}
#endif

/*
avr_eeprom.c

Handle stored parameters

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

2017-06-04 Created reusing code from other projects. /Henrik
*/

#ifndef EEPROM_H_
#define EEPROM_H_


#include <stdint.h>


// TODO unknown_dev should have been zero
enum
{
	pointer_dev = 0,
	detector_dev = 1,
	unknown_dev = 2,
	target_dev = 3,
};


// A magic number so we can detect if expected format of stored eedata has been changed (don't use zero).
#define EEDATA_MAGIC_NR 0x0126

// Is there an old data format that we may encounter.
#define EEDATA_LEGACY_MAGIC_NR 0x0125



typedef struct
{
	uint32_t magicNumber;
	uint32_t microVoltsPerUnit;

	uint8_t player_number;
	int8_t spare1;  // TODO We need also pairing_signal_strength_percent
	uint8_t audio_volume_percent;
	uint8_t spare3;
	uint8_t signal_strength_percent;
	uint8_t signal_strength_pairing_percent;
	int8_t autofire;
	uint8_t test_options;

	uint16_t ignore_time_start_ms; // ignore received IR after sending IR
	uint16_t ignore_time_end_ms; // ignore received IR after sending IR
	uint16_t idle_timeout_s; // after how long inactivity to power off
	uint16_t battery_depleated_at_mv;

	uint16_t short_led_blink_ms;
	uint16_t fire_time_half_ms; // how often we can fire, that time is 2x this value.
	uint16_t stop_charging_battery_at_mv;
	uint16_t keep_alive_s; // how often a keep alive message needs to be sent.

	uint16_t expected_swhw;
	uint16_t full_ammo;
	uint16_t pairing_timeout_ms;
	uint16_t low_battery_warning_mv;

	int32_t pairing_nr;
	int32_t game_code;

	int64_t serial_nr;

	int16_t pairing_flash_rate_ms;
	int16_t pairing_wait_for_alive_ms;
	uint16_t off_time_ms; // press button this long to turn device off
	uint16_t device_type;

	int32_t pair_addr;
	int32_t detect_margin_mv;

	int16_t led_on_time_after_hit_ms;
	int16_t led_off_time_after_hit_ms;
	int32_t spare9;

	int64_t spare10;

	int64_t spare11;

	int64_t spare12;

	int64_t spare13;

	int64_t spare14;

	uint32_t spare15;
	uint32_t checkSum;
}
EeDataStruct;

#ifdef EEDATA_LEGACY_MAGIC_NR
typedef struct
{
	uint32_t magicNumber;
	uint32_t microVoltsPerUnit;

	uint16_t device_type;
	uint8_t player_number;
	int8_t spare1;
	uint16_t stop_charging_battery_at_mv;

	// TODO We need also pairing_signal_strength_percent
	uint8_t signal_strength_percent;
	uint8_t spare2;

	uint16_t ignore_time_start_ms; // ignore received IR after sending IR
	uint16_t ignore_time_end_ms; // ignore received IR after sending IR
	uint16_t idle_timeout_s; // after how long inactivity to power off
	uint16_t spare_off_time_s; // This used to be off_time_s but its been replaced by off_time_ms

	uint16_t short_led_blink_ms; // how long to be inactive after being hit, actual time is 3x this value,
	uint16_t fire_time_half_ms; // how often we can fire, that time is 2x this value.
	uint8_t audio_volume_percent;
	uint8_t spare3;
	uint16_t keep_alive_s; // how often a keep alive message needs to be sent.

	uint16_t expected_swhw;
	uint16_t full_ammo;
	uint16_t pairing_timeout_ms;
	uint16_t spare6;

	int32_t pairing_nr;
	int32_t game_code;

	int64_t serial_nr;

	int16_t pairing_flash_rate_ms;
	int16_t pairing_wait_for_alive_ms;
	uint16_t off_time_ms; // press button this long to turn device off
	int8_t autofire;
	int8_t spare7;

	int32_t pair_addr;
	int32_t detect_margin_mv;

	int64_t spare9;
	int64_t spare10;

	int64_t spare11;
	int64_t spare12;
	int64_t spare13;
	int64_t spare14;

	uint32_t spare15;
	uint32_t checkSum;
}
EeDataLegacyStruct;
#endif


extern EeDataStruct ee;


void eepromSave(void);
void eepromLoad(void);

#endif


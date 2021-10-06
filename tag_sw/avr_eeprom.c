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

// http://www.nongnu.org/avr-libc/user-manual/group__avr__eeprom.html#gac5c2be42eb170be7a26fe8b7cce4bc4d
// http://www.avrfreaks.net/forum/tut-c-using-eeprom-memory-avr-gcc?page=all
// http://www.nongnu.org/avr-libc/user-manual/group__avr__eeprom.html


#include <stdint.h>

#include <avr/eeprom.h>
#include "avr_uart.h"


#include "avr_eeprom.h"

// Where in EEPROM the backup message shall be written.
#define BACKUP_EEPROM_OFFSET 512


// At 3.425 Volt got ADC value 0x024c (588)
#define DEFAULT_MicroVolt_PER_ADC_UNIT 5825L
#define DEFAULT_BATTERY_STOP_CHARGING_MV 4100
#define DEFAULT_BATTERY_DEPLEATED_AT_MV 3000
#define DEFAULT_low_battery_warning_mv 3100

#define DEFAULT_signal_strength_percent 50

// Check baudrate in avr_tmr2 and calculate:
// DEFAULT_ignore_time_ms = (1/baudrate * 9) * 1000 + 10
#define DEFAULT_ignore_time_start_ms 30
#define DEFAULT_ignore_time_end_ms 50

#if HW_VERSION == 3
#define DEFAULT_device_type 1
#else
#define DEFAULT_device_type 0
#endif
#define DEFAULT_player_number -1
#define DEFAULT_keep_alive_s 150
#if HW_VERSION == 3
#define DEFAULT_idle_timeout_s 170
#else
#define DEFAULT_idle_timeout_s 500
#endif
#define DEFAULT_hit_time_third_ms 3000
#define DEFAULT_fire_time_half_ms 100
#define DEFAULT_volume_percent 50
#define DEFAULT_expected_swhw HW_VERSION
#define DEFAULT_full_ammo 125
#define DEFAULT_pair_nr -1
#define DEFAULT_pairing_timeout_ms 333
#define DEFAULT_pairing_flash_rate_ms 5000
#define DEFAULT_pair_addr -1
#define DEFAULT_pairing_wait_for_alive_ms 12000
#define DEFAULT_off_time_ms 20000
#define DEFAULT_autofire 1
#define DEFAULT_DETECT_MARGIN_MV 10
#define DEFAULT_game_code 0
#define DEFAULT_serial_nr 0
#define DEFAULT_signal_strength_pairing_percent 4

// To be sure second receiver works we use only IR input 2 for pairing.
#define DEFAULT_test_options 2


EeDataStruct ee={
	EEDATA_MAGIC_NR, // magicNumber
	DEFAULT_MicroVolt_PER_ADC_UNIT,

	DEFAULT_player_number,
	0, // spare1
    DEFAULT_volume_percent,
	0, // spare3
	DEFAULT_signal_strength_percent,
	DEFAULT_signal_strength_pairing_percent,
	DEFAULT_autofire,
	DEFAULT_test_options,

	DEFAULT_ignore_time_start_ms,
	DEFAULT_ignore_time_end_ms,
	DEFAULT_idle_timeout_s,
	DEFAULT_BATTERY_DEPLEATED_AT_MV,

	DEFAULT_hit_time_third_ms,
	DEFAULT_fire_time_half_ms,
	DEFAULT_BATTERY_STOP_CHARGING_MV,
	DEFAULT_keep_alive_s,

	DEFAULT_expected_swhw,
	DEFAULT_full_ammo,
	DEFAULT_pairing_timeout_ms,
	DEFAULT_low_battery_warning_mv,

    DEFAULT_pair_nr,
	DEFAULT_game_code,

	DEFAULT_serial_nr,

	DEFAULT_pairing_flash_rate_ms,
	DEFAULT_pairing_wait_for_alive_ms,
	DEFAULT_off_time_ms,
	DEFAULT_device_type,

	DEFAULT_pair_addr,
	DEFAULT_DETECT_MARGIN_MV,

	0, // spare9
	0, // spare10
	0, // spare11
	0, // spare12
	0, // spare13
	0, // spare14

	0, // spare15
	0  // checkSum
};



// Very simple checksum
static uint32_t calcCSum(const uint8_t *ptr, uint16_t size)
{
	uint32_t s=0;
	uint8_t i;
	for (i=0; i<size; ++i)
	{
		s = s*31 + *ptr;
        ptr++;
	}
	return s;
}



static int8_t saveBytePacker(EeDataStruct* eedataStruct, uint16_t offset)
{
	eeprom_write_block(eedataStruct, (void*)offset, sizeof(EeDataStruct));
	return  0;
}

static int8_t loadBytePacker(EeDataStruct* eedataStruct, uint16_t offset)
{
	eeprom_read_block((void *)eedataStruct, (const void *)offset, sizeof(EeDataStruct));
	return 0;
}



void eepromSave()
{
	// Set checksum to zero while calculating it.
	ee.checkSum = 0;
    const uint32_t calculatedCSum = calcCSum((const uint8_t*)&ee, sizeof(EeDataStruct));
	ee.checkSum = calculatedCSum;

	saveBytePacker(&ee, 0);
	saveBytePacker(&ee, BACKUP_EEPROM_OFFSET);

	UART_PRINT_P("e save\r\n");
}

#ifdef EEDATA_LEGACY_MAGIC_NR
static void eepromDataUpgrade(EeDataStruct* e)
{
	if (e->magicNumber == EEDATA_LEGACY_MAGIC_NR)
	{
		EeDataLegacyStruct* l = (EeDataLegacyStruct*)e;

		// save checksum
		const uint32_t csumLoaded = l->checkSum;

		// Set checksum to zero while calculating it. Since that is what we did when saving
		l->checkSum = 0;

		// Check CRC with old size of data. NOTE new size must not be less than old!
		const uint32_t csumCalculated = calcCSum((const unsigned char *)l, sizeof(EeDataLegacyStruct));

		if (csumCalculated == csumLoaded)
		{
			e->magicNumber = EEDATA_MAGIC_NR;

			// Apply any translation needed here. For example if another
		    // field was added set it to its default value.
			e->microVoltsPerUnit = l->microVoltsPerUnit;

			e->player_number = l->player_number;
			e->spare1 = 0;
			e->audio_volume_percent = DEFAULT_volume_percent;
			e->spare3 = 0;
			e->signal_strength_percent = DEFAULT_signal_strength_percent;
			e->signal_strength_pairing_percent = DEFAULT_signal_strength_pairing_percent;
			e->autofire = DEFAULT_autofire;
			e->test_options = DEFAULT_test_options;

			e->ignore_time_start_ms = DEFAULT_ignore_time_start_ms;
			e->ignore_time_end_ms = DEFAULT_ignore_time_end_ms;
			e->idle_timeout_s = DEFAULT_idle_timeout_s;
			e->battery_depleated_at_mv = DEFAULT_BATTERY_DEPLEATED_AT_MV;

			e->hit_time_third_ms = DEFAULT_hit_time_third_ms;
			e->fire_time_half_ms = DEFAULT_fire_time_half_ms;
			e->stop_charging_battery_at_mv = DEFAULT_BATTERY_STOP_CHARGING_MV;
			e->keep_alive_s = DEFAULT_keep_alive_s;

			e->expected_swhw = l->expected_swhw;
			e->full_ammo = DEFAULT_full_ammo;
			e->pairing_timeout_ms = DEFAULT_pairing_timeout_ms;
			e->low_battery_warning_mv = DEFAULT_low_battery_warning_mv;

			e->pairing_nr = DEFAULT_pair_nr;
			e->game_code = l->game_code;

			e->serial_nr = l->serial_nr;

			e->pairing_flash_rate_ms = DEFAULT_pairing_flash_rate_ms;
			e->pairing_wait_for_alive_ms = DEFAULT_pairing_wait_for_alive_ms;
			e->off_time_ms = DEFAULT_off_time_ms;
			e->device_type = DEFAULT_device_type;

			e->pair_addr = DEFAULT_pair_addr;
			e->detect_margin_mv = DEFAULT_DETECT_MARGIN_MV;

			e->spare9 = 0;
			e->spare10 = 0;
			e->spare11 = 0;
			e->spare12 = 0;
			e->spare13 = 0;
			e->spare14 = 0;
			e->spare15 = 0;

			// Checksum was OK so update it also to new format.
			e->checkSum = 0;
			const uint32_t newCsum = calcCSum((const unsigned char *)e, sizeof(EeDataStruct));
			e->checkSum = newCsum;

			UART_PRINT_PL("e upgraded");
		}

	}
}
#endif


// Returns 0 if OK
static int8_t eepromTryLoad(uint16_t offset)
{
	EeDataStruct tmp;


	const int8_t r = loadBytePacker(&tmp, offset);

	#ifdef EEDATA_LEGACY_MAGIC_NR
	eepromDataUpgrade(&tmp);
	#endif

	// save checksum
	const uint32_t csumLoaded = tmp.checkSum;

	// Set checksum to zero while calculating it. Since that is what we did when saving
	tmp.checkSum = 0;
    const uint32_t csumCalculated = calcCSum((const unsigned char *)&tmp, sizeof(EeDataStruct));

	if(tmp.magicNumber != ee.magicNumber)
	{
		UART_PRINT_PL("e wrong magic");
    	return -1;
	}

	if(csumCalculated != csumLoaded)
	{
		UART_PRINT_PL("e wrong checksum");
    	return -1;
	}

	ee = tmp;

	return r;
}


// If EEProm is available we want to set wantedCurrent and/or wantedVoltage
// We do that by reading EEPROM as if those where commands an pass these commands to
void eepromLoad(void)
{
	int8_t r = eepromTryLoad(0);

	if (r == 0)
	{
		UART_PRINT_PL("e loaded OK");
	}
	else
	{
		r = eepromTryLoad(BACKUP_EEPROM_OFFSET);
		if (r==0)
		{
			UART_PRINT_PL("e backup OK");
		}
		else
		{
			UART_PRINT_PL("e fail");
		}
	}

	// Spare fields can be put in use without changing EEDATA_MAGIC_NR If:
	// 1) The default value is zero.
	// 2) Zero is not needed as a value and the default value can be set here
	//    if the value was zero.
	if (ee.pairing_wait_for_alive_ms == 0)
	{
		ee.pairing_wait_for_alive_ms = DEFAULT_pairing_wait_for_alive_ms;
	}
	if (ee.off_time_ms == 0)
	{
		ee.off_time_ms = DEFAULT_off_time_ms;
	}
	if (ee.detect_margin_mv == 0)
	{
		ee.detect_margin_mv = DEFAULT_DETECT_MARGIN_MV;
	}
	if (ee.idle_timeout_s == 0)
	{
		ee.idle_timeout_s = DEFAULT_idle_timeout_s;
	}
	if (ee.idle_timeout_s < ee.keep_alive_s)
	{
		ee.idle_timeout_s = ee.keep_alive_s;
	}
	if (ee.signal_strength_pairing_percent == 0)
	{
		ee.signal_strength_pairing_percent = DEFAULT_signal_strength_pairing_percent;
	}
	if (ee.test_options == 0)
	{
		ee.test_options = DEFAULT_test_options;
	}

}





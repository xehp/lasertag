/*
power.c

Provide functions to supervise the battery.

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

2021-04-20 Created. /Henrik
*/

/*
References:
[1] https://ww1.microchip.com/downloads/en/DeviceDoc/ATmega48A-PA-88A-PA-168A-PA-328-P-DS-DS40002061B.pdf
*/

#include <avr/pgmspace.h>
#include <stdint.h>
#include "avr_cfg.h"
#include "avr_sys.h"
#include "avr_uart.h"
#include "avr_adc.h"
#include "avr_ports.h"
#include "power.h"

// Its about 154 ADC steps per Volt
// TODO We need to let ADC use its internal voltage reference.
#define BATTERY_DEPLETED 520
#define BATTERY_NEED_CHARGING 550
#define BATTERY_FULL 610
#define BATTERY_TIMEOUT 0xFFFF

enum {
	POWER_MEASURING_IDLE_STATE,
	POWER_MEASURING_STARTUP_STATE,
	POWER_MEASURING_NORMAL_STATE,
	POWER_MEASURING_FULL_STATE,
	POWER_MEASURING_DEPLETED_STATE,
	POWER_MEASURING_FAIL_STATE,
};

static int8_t power_state = 0;
static int8_t power_counter = 0;
static uint16_t log_voltage = 0;
//static int16_t timer_ms = 0;

// Find median value of the latest 3 values.
uint16_t median_of_3(uint16_t v0, uint16_t v1, uint16_t v2)
{
	uint16_t vm = 0xFFFF;
	if ((v0 >= v1) && (v0 <= v2))
	{
		vm = v0;
	}
	else if ((v0 <= v1) && (v0 >= v2))
	{
		vm = v0;
	}
	else if ((v1 >= v2) && (v1 <= v0))
	{
		vm = v1;
	}
	else if ((v1 <= v2) && (v1 >= v0))
	{
		vm = v1;
	}
	else if ((v2 >= v1) && (v2 <= v0))
	{
		vm = v2;
	}
	else if ((v2 <= v1) && (v2 >= v0))
	{
		vm = v2;
	}
	else
	{
		// This should not happen
		avr_error_handler_P(PSTR("median\r\n"), 19);
	}
	return vm;
}

void power_init(void)
{
	uart_print_P(PSTR("WDTCSR "));
	uart_print_hex8(WDTCSR);
	uart_print_P(PSTR("\r\n"));

	AVR_ADC_init();

	RELAY_ENABLE();
	RELAY_ON();

	//timer_ms = avr_systime_ms_16() + 1000;
}

// Time factor determines over how many samples we make the rolling mean value.
#define MEASURING_FILTER_SCALE_FACTOR 16
#define MEASURING_FILTER_TIME_FACTOR 32

static uint32_t filtered_voltage_scaled = 0;
static uint32_t filtered_values_available = 0;

static uint16_t calcRollingMeanValue(uint16_t latestValue)
{
	// Calculate a rolling mean value
	if (filtered_values_available < MEASURING_FILTER_TIME_FACTOR)
	{
		filtered_values_available++;
		const uint32_t latestValueScaled = latestValue*MEASURING_FILTER_SCALE_FACTOR;
		const uint32_t filteredChangeScaledXTime = (filtered_voltage_scaled * (filtered_values_available-1)) + latestValueScaled;
		filtered_voltage_scaled = filteredChangeScaledXTime / filtered_values_available;
	}
	else
	{
		const uint32_t latestValueScaled = latestValue*MEASURING_FILTER_SCALE_FACTOR;
		const uint32_t filteredChangeScaledXTime = (filtered_voltage_scaled * (MEASURING_FILTER_TIME_FACTOR-1)) + latestValueScaled;
		filtered_voltage_scaled = filteredChangeScaledXTime / MEASURING_FILTER_TIME_FACTOR;
	}
	return filtered_voltage_scaled / MEASURING_FILTER_SCALE_FACTOR;
}

/**
 * Returns 0xFFFF if no value was available.
 */
static uint16_t read_adc(void)
{
	if (AVR_ADC_isready())
	{
		uint16_t pv0=AVR_ADC_getSample(0);
		uint16_t pv1=AVR_ADC_getSample(1);
		uint16_t pv2=AVR_ADC_getSample(2);
		uint16_t pv = median_of_3(pv0, pv1, pv2);

		const uint16_t fv = calcRollingMeanValue(pv);

		if (log_voltage != fv)
		{
			uart_print_P(PSTR("v "));
			uart_print_hex16(fv);
			uart_print_P(PSTR("\r\n"));
			log_voltage = fv;
		}
		AVR_ADC_startSampling();
		return fv;
	}
	else
	{
		uart_print_P(PSTR("timeout\r\n"));
		power_counter = 0;
		power_state=POWER_MEASURING_FAIL_STATE;
		return 0xFFFF;
	}
}


void power_tick_s(void)
{
	/*const int16_t t = avr_systime_ms_16();
	const int16_t d = t - timer_ms;

	if (d<0)
	{
		return;
	}
	timer_ms = t+100;*/

    switch (power_state)
    {
		default:
		case POWER_MEASURING_IDLE_STATE:
		{
			// Initial state, start an ADC conversion and go to startup state.
			RELAY_ON();
			AVR_ADC_startSampling();
			uart_print_P(PSTR("power\r\n"));
			power_counter = 0;
			power_state = POWER_MEASURING_STARTUP_STATE;
			break;
		}
		case POWER_MEASURING_STARTUP_STATE:
		{
			// Wait in this state with relay on for a while so that things are stable.
			// Then go to normal state.
			RELAY_ON();
			uint16_t pv = read_adc();
			if (pv != BATTERY_TIMEOUT)
			{
				if (power_counter<60)
				{
					++power_counter;
				}
				else
				{
					uart_print_P(PSTR("charge\r\n"));
					power_counter = 0;
					power_state=POWER_MEASURING_NORMAL_STATE;
				}
			}
			break;
		}
		case POWER_MEASURING_NORMAL_STATE:
		{
			RELAY_ON();
			uint16_t pv = read_adc();
			if (pv != BATTERY_TIMEOUT)
			{
				if (pv > BATTERY_FULL)
				{
					// Battery must be full
					uart_print_P(PSTR("full\r\n"));
					power_counter = 0;
					power_state=POWER_MEASURING_FULL_STATE;
				}
				if (pv < BATTERY_DEPLETED)
				{
					// Battery is empty, turn off
					uart_print_P(PSTR("empty\r\n"));
					power_counter = 0;
					power_state=POWER_MEASURING_DEPLETED_STATE;
				}
				AVR_ADC_startSampling();
			}
			break;
		}
		case POWER_MEASURING_FULL_STATE:
		{
			RELAY_OFF();
			uint16_t pv = read_adc();
			if (pv != BATTERY_TIMEOUT)
			{
				if (power_counter < 60)
				{
					++power_counter;
				}
				else if (pv < BATTERY_NEED_CHARGING)
				{
					// Battery need charging again
					uart_print_P(PSTR("recharge\r\n"));
					power_counter = 0;
					power_state=POWER_MEASURING_NORMAL_STATE;
				}
			}
			break;
		}
		case POWER_MEASURING_DEPLETED_STATE:
		{
			RELAY_OFF();
			uint16_t pv = read_adc();
			if (pv != BATTERY_TIMEOUT)
			{
				if (power_counter < 60)
				{
					++power_counter;
				}
				else if (pv > BATTERY_NEED_CHARGING)
				{
					// Battery need charging again
					uart_print_P(PSTR("recharge\r\n"));
					power_counter = 0;
					power_state=POWER_MEASURING_NORMAL_STATE;
				}
			}
			break;
		}
		case POWER_MEASURING_FAIL_STATE:
		{
			RELAY_OFF();
			if (power_counter<60)
			{
				++power_counter;
			}
			else
			{
				uart_print_P(PSTR("restart\r\n"));
				AVR_ADC_init();
				power_state=POWER_MEASURING_IDLE_STATE;
			}
			break;
		}
    }

    avr_wtd_reset_power();
}


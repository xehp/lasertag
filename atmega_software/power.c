/*
power.c

Provide functions to supervise the battery.

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

2021-04-20 Created. /Henrik
*/

/*
TODO Use Atmega internal temp sensor so we can avoid charging at cold temp.


References:
[1] https://ww1.microchip.com/downloads/en/DeviceDoc/ATmega48A-PA-88A-PA-168A-PA-328-P-DS-DS40002061B.pdf
[2] https://batteryuniversity.com/learn/article/charging_lithium_ion_batteries
[3] https://batteryuniversity.com/learn/article/confusion_with_voltages
*/

#include <avr/pgmspace.h>
#include <stdint.h>
#include "avr_cfg.h"
#include "avr_sys.h"
#include "avr_uart.h"
#include "avr_adc.h"
#include "avr_ports.h"
#include "avr_tmr0.h"
#include "avr_tmr1.h"
#include "utility.h"
#include "beep.h"
#include "power.h"



// At 3.425 Volt got ADC value 0x024c (588)
#define MicroVolt_PER_ADC_UNIT 5825L

// Typical LiIon can be charged to 4.2 Volt (some unusual ones only 4.1 Volt).
// A 50 mV margin from that is recommended so we could use 4150 mV.
// Its only good for the Lithium-ion to not fully charge it (and not fully deplete it).
// We probably have plenty of capacity so we can keep some extra margin.
// We will not optimize performance yet so using a little less here for now.
// Ref [2]
#define BATTERY_STOP_CHARGING_MV 4100
#define BATTERY_FULL_MV 4200

// Typically LiIon is empty at 2.8 to 3.0 Volt.
// We choose a conservative value and some margin.
// If voltage drop to 3.05 Volt (or so) we consider it depleted.
// Ref [3]
#define BATTERY_DEPLETED_MV 3000

// We need 3.3 Volt and there is some loss in the voltage regulator.
#define BATTERY_LOW_BATTERY_WARNING_MV 3400

// If voltage is above this then charging is done,
// battery is disconnected and device is running on USB power only.
#define VOLTAGE_INDICATE_USB_ONLY_MV 4400

// Consider also that translation from ADC to milli volts
// give values in steps (not one mV at a time).
#define LOG_MARGIN_MV ((MicroVolt_PER_ADC_UNIT+999)/1000)

#define OVER_CHARGING_WARNING_MARGIN_MV 50

#define BATTERY_TIMEOUT 0xFFFF

// To indicate charging process debug LEDs are used.
// There are two LEDs but on same port so one at a time can be on.
#ifdef POWER_LED_PORT
#define BOTH_LED_OFF() {POWER_LED_DISABLE();}
#define CHARGING_LED_ON() {POWER_LED_ON();POWER_LED_ENABLE();}
#define FULL_LED_ON() {POWER_LED_OFF();POWER_LED_ENABLE();}
#define TOGGLE_POWER_LED() {POWER_LED_ENABLE();	if (POWER_LED_IS_ON()) {POWER_LED_OFF();} else {POWER_LED_ON();}}
#else
#define BOTH_LED_OFF()
#define CHARGING_LED_ON()
#define FULL_LED_ON()
#define TOGGLE_POWER_LED()
#endif

#define LOG_VOLTAGE_MV_P(str) {log_voltage_mv_p(PSTR(str));}



enum {
	POWER_MEASURING_IDLE_STATE,
	POWER_MEASURING_STARTUP_STATE,
	POWER_MEASURING_ON_STATE,
	POWER_MEASURING_FULL_STATE,
	POWER_MEASURING_USB_ONLY_STATE,
	POWER_MEASURING_DEPLETED_STATE,
	POWER_MEASURING_FAIL_STATE,
	POWER_OVER_FULL_STATE,
};

static int8_t power_state = 0;
static int8_t power_counter = 0;
static uint16_t voltage_mV = 0;
//static int16_t timer_ms = 0;
static int8_t log_state = 0;
static uint16_t led_indicated_voltage_mV = 0;

// Find median value of the latest 3 values.
static uint16_t median_of_3(uint16_t v0, uint16_t v1, uint16_t v2)
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
		AVR_ERROR_HANDLER_P("median", __LINE__);
	}
	return vm;
}

void power_init(void)
{
	AVR_ADC_init();

	// Relay output is initialized in avr_init.
	//RELAY_ENABLE();
	//RELAY_ON();

	//timer_ms = avr_systime_ms_16() + 1000;

	RELAY_ON();
	CHARGING_LED_ON();
}

// Time factor determines over how many samples we make the rolling mean value.
// A higher value for filter time gives a better but slower value so its a compromise.
#define MEASURING_FILTER_TIME_FACTOR 64LL
#define MEASURING_FILTER_SCALE_FACTOR 64LL

static uint64_t filtered_voltage_scaled = 0;
static uint8_t filtered_values_available = 0;

static uint32_t calcRollingMeanValue(uint32_t latestValue)
{
	// Calculate a rolling mean value
	if (filtered_values_available < MEASURING_FILTER_TIME_FACTOR)
	{
		filtered_values_available++;
		const uint64_t latestValueScaled = (uint64_t)latestValue*MEASURING_FILTER_SCALE_FACTOR;
		const uint64_t filteredChangeScaledXTime = (filtered_voltage_scaled * (filtered_values_available-1)) + latestValueScaled;
		filtered_voltage_scaled = filteredChangeScaledXTime / filtered_values_available;
	}
	else
	{
		const uint64_t latestValueScaled = (uint64_t)latestValue*MEASURING_FILTER_SCALE_FACTOR;
		const uint64_t filteredChangeScaledXTime = (filtered_voltage_scaled * (MEASURING_FILTER_TIME_FACTOR-1)) + latestValueScaled;
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
		const uint16_t pv0=AVR_ADC_getSample(0);
		const uint16_t pv1=AVR_ADC_getSample(1);
		const uint16_t pv2=AVR_ADC_getSample(2);
		const uint16_t pv = median_of_3(pv0, pv1, pv2);

		AVR_ADC_startSampling();

		const uint32_t latest_uV = ((uint32_t)pv * MicroVolt_PER_ADC_UNIT);

		const uint32_t filtered_uV = calcRollingMeanValue(latest_uV);

		voltage_mV = filtered_uV / 1000;
		return voltage_mV;
	}
	else
	{
		uart_print_P(PSTR("timeout\r\n"));
		beep_fifo_put(MID_TONE, 100);
		power_counter = 0;
		power_state=POWER_MEASURING_FAIL_STATE;
		return 0xFFFF;
	}
}

static void log_voltage_mv_p(const char *pgm_addr)
{
	char tmp[32];
	utility_lltoa(voltage_mV, tmp, 10);
	uart_print_P(pgm_addr);
	uart_putchar(' ');
	uart_print(tmp);
	uart_print_P(PSTR(" mV\r\n"));
	led_indicated_voltage_mV = voltage_mV;
}

void power_tick_s(void)
{
	// This is just debugging, remove later.
	/*{
		char tmp[32];
		utility_lltoa(log_voltage_mV, tmp, 10);

		uart_print_P(PSTR("power state "));
		uart_print_hex4(power_state);
		uart_putchar(' ');
		uart_print(tmp);
		uart_print_P(PSTR(" mV\r\n"));
	}*/

    switch (power_state)
    {
		default:
		case POWER_MEASURING_IDLE_STATE:
		{
			// Initial state, start an ADC conversion and go to startup state.
			RELAY_ON();
			BOTH_LED_OFF();
			AVR_ADC_startSampling();
			UART_PRINT_P("power startup\r\n");
			power_counter = 0;
			power_state = POWER_MEASURING_STARTUP_STATE;
			break;
		}
		case POWER_MEASURING_STARTUP_STATE:
		{
			// Wait in this state with relay on for a while so that things are stable.
			// Then go to normal/charging state.
			uint16_t pv = read_adc();
			if (pv != BATTERY_TIMEOUT)
			{
				if (power_counter<30)
				{
					#ifdef ADDITIONAL_FEATURES
					if ((voltage_mV + (LOG_MARGIN_MV*2)) < led_indicated_voltage_mV)
					{
						// voltage is falling
						BOTH_LED_OFF();
						LOG_VOLTAGE_MV_P("discharging");
					}
					else if ((voltage_mV ) > led_indicated_voltage_mV + LOG_MARGIN_MV)
					{
						// voltage is rising
						CHARGING_LED_ON();
						LOG_VOLTAGE_MV_P("charging");
					}
					#endif
					++power_counter;
				}
				else
				{
					LOG_VOLTAGE_MV_P("on");
					RELAY_ON();
					power_counter = 0;
					power_state=POWER_MEASURING_ON_STATE;
				}
			}
			break;
		}
		case POWER_MEASURING_ON_STATE:
		{
			uint16_t pv = read_adc();
			if (pv != BATTERY_TIMEOUT)
			{
				if ((pv > BATTERY_FULL_MV) || ((pv > BATTERY_STOP_CHARGING_MV) && (pv > (led_indicated_voltage_mV+LOG_MARGIN_MV))))
				{
					// Battery must be full
					LOG_VOLTAGE_MV_P("full");
					BOTH_LED_OFF();
					RELAY_OFF();
					power_counter = 0;
					power_state=POWER_MEASURING_FULL_STATE;
				}
				else if (pv < BATTERY_DEPLETED_MV)
				{
					// Battery is empty, turn off
					BOTH_LED_OFF();
					RELAY_OFF();
					LOG_VOLTAGE_MV_P("depleted");
					beep_fifo_put(MID_TONE, 500);
					power_counter = 0;
					power_state=POWER_MEASURING_DEPLETED_STATE;
				}
				else if ((voltage_mV + LOG_MARGIN_MV) < led_indicated_voltage_mV)
				{
					// voltage is falling
					BOTH_LED_OFF();
					LOG_VOLTAGE_MV_P("discharging");
				}
				else if ((voltage_mV ) > led_indicated_voltage_mV + LOG_MARGIN_MV)
				{
					// voltage is rising
					CHARGING_LED_ON();
					LOG_VOLTAGE_MV_P("charging");
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
				if (pv > VOLTAGE_INDICATE_USB_ONLY_MV)
				{
					LOG_VOLTAGE_MV_P("usb");
					FULL_LED_ON();
					power_counter = 60;
					power_state=POWER_MEASURING_USB_ONLY_STATE;
				}
				else if (power_counter < 60)
				{
					++power_counter;
				}
				else if (pv > (led_indicated_voltage_mV + (OVER_CHARGING_WARNING_MARGIN_MV)))
				{
					// relay is off but voltage is still rising (slowly), not good.
					CHARGING_LED_ON();
					LOG_VOLTAGE_MV_P("overfull");
					power_state=POWER_OVER_FULL_STATE;
				}
				#ifdef ADDITIONAL_FEATURES
				else if (pv < BATTERY_LOW_BATTERY_WARNING_MV)
				{
					// Battery need charging again
					LOG_VOLTAGE_MV_P("recharging");
					RELAY_ON();
					power_counter = 0;
					power_state=POWER_MEASURING_ON_STATE;
				}
				#endif
			}
			break;
		}
		case POWER_MEASURING_DEPLETED_STATE:
		{
			RELAY_OFF();
			#ifdef ADDITIONAL_FEATURES
			uint16_t pv = read_adc();
			if (pv != BATTERY_TIMEOUT)
			{
				if (power_counter < 60)
				{
					++power_counter;
				}
				else if (pv > BATTERY_LOW_BATTERY_WARNING_MV)
				{
					// Battery need charging again
					LOG_VOLTAGE_MV_P("restored");
					RELAY_ON();
					power_counter = 0;
					power_state=POWER_MEASURING_ON_STATE;
				}
			}
			#endif
			break;
		}
		case POWER_MEASURING_FAIL_STATE:
		{
			RELAY_OFF();

			if (power_counter<10)
			{
				beep_fifo_put(MID_TONE, 100);
				++power_counter;
			}
			else
			{
				LOG_VOLTAGE_MV_P("restart");
				CHARGING_LED_ON();
				AVR_ADC_init();
				power_state=POWER_MEASURING_IDLE_STATE;
			}
			break;
		}
		case POWER_MEASURING_USB_ONLY_STATE:
		{
			// Voltage indicated that charging is done and battery is disconnected as needed.
			RELAY_OFF();

			#ifdef ADDITIONAL_FEATURES
			uint16_t pv = read_adc();
			if (pv != BATTERY_TIMEOUT)
			{
				/*if (TRIGGER_READ())
				{
					LOG_VOLTAGE_MV_P("trig");
					CHARGING_LED_ON();
					power_counter = 60;
					power_state = POWER_MEASURING_ON_STATE;
				} else*/
				if (power_counter < 60)
				{
					++power_counter;
				}
				if (pv < BATTERY_STOP_CHARGING_MV)
				{
					// Battery need charging again
					if (power_counter > 60)
					{
						LOG_VOLTAGE_MV_P("recharging");
						RELAY_ON();
						power_state=POWER_MEASURING_FULL_STATE;
					}
					else
					{
						power_counter++;
					}
				}
				else
				{
					power_counter = 0;
				}
			}
			#endif
			break;
		}
		case POWER_OVER_FULL_STATE:
		{
			RELAY_OFF();

			// Relay should be off but voltage level indicated it is not.
			// It may be simply low voltage from USB (not a problem) or relay
			// has not disconnected battery as it should (that is a problem).
			// If relay did not disconnect try to raise alarm and use as
			// much power as possible.
			LOG_VOLTAGE_MV_P("overfull");

			beep_fifo_put(MID_TONE, 900);
			HIT_LEDS_ON();
			VIB_ON();
			LASER_ON();
			TOGGLE_POWER_LED();

			#ifdef ADDITIONAL_FEATURES
			uint16_t pv = read_adc();
			if (pv != BATTERY_TIMEOUT)
			{
				if (pv < BATTERY_STOP_CHARGING_MV)
				{
					HIT_LEDS_OFF();
					VIB_OFF();
					BOTH_LED_OFF();
					power_state = POWER_MEASURING_FAIL_STATE;
				}
			}
			#endif
			break;
		}
    }

	if (power_state != log_state)
	{
		UART_PRINT_P("ps ");
		uart_print_hex4(power_state);
		uart_print_crlf();
		log_state = power_state;
	}


    avr_wtd_reset_power();
}

uint16_t power_get_voltage_mV()
{
	return voltage_mV;
}

int8_t power_get_state()
{
	return power_state;
}

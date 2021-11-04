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



How to calibrate voltage measuring

Issue command "g v" to get voltage_reading.
Issue command "g m" to get old_microVoltsPerUnit.
Measure also battery voltage with a good voltage meter to get measured_voltage.
new_microVoltsPerUnit = measured_voltage*old_microVoltsPerUnit/voltage_reading
issue command "s m <new_microVoltsPerUnit>"
issue command "e" to save.
reboot for setting to take effect, command "r".
Example:
4100*5825/3970 -> 6016



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
#include "avr_eeprom.h"
#include "utility.h"
#include "beep.h"
#include "power.h"
#include "errlog.h"



// At 3.425 Volt got ADC value 0x024c (588)
//#define MicroVolt_PER_ADC_UNIT 5825L
#define MicroVolt_PER_ADC_UNIT ee.microVoltsPerUnit

// Typical LiIon can be charged to 4.2 Volt (some unusual ones only 4.1 Volt).
// A 50 mV margin from that is recommended so we could use 4150 mV.
// Its only good for the Lithium-ion to not fully charge it (and not fully deplete it).
// We probably have plenty of capacity so we can keep some extra margin.
// We will not optimize performance yet so using a little less here for now.
// Ref [2]
#define BATTERY_STOP_CHARGING_MV ee.stop_charging_battery_at_mv
#define BATTERY_FULL_MV 4200

// Typically LiIon is empty at 2.8 to 3.0 Volt.
// We choose a conservative value and some margin.
// If voltage drop to 3.05 Volt (or so) we consider it depleted.
// Ref [3]
#define BATTERY_DEPLETED_MV ee.battery_depleated_at_mv

// We need 3.3 Volt and there is some loss in the voltage regulator.
#define BATTERY_LOW_BATTERY_WARNING_MV ee.low_battery_warning_mv

// If voltage is above this then charging is done,
// battery is disconnected and device is running on USB power only.
#define VOLTAGE_INDICATE_USB_ONLY_MV 4400

// Consider also that translation from ADC to milli volts
// give values in steps (not one mV at a time).
#define DETECT_MARGIN_MV ee.detect_margin_mv

#define OVER_CHARGING_WARNING_MARGIN_MV 50

#define TIMEOUT_ADC 0xFFFF

// To indicate charging process debug LEDs are used.
// In HW version <=3 there are two LEDs but on same port so one at a time can be on.
// In HW versions after 3 there is only one LED under SW control for low battery warning.
#ifdef POWER_LED_PORT
#if ((defined POWER_LED_ACTIVE_HIGH) || (defined POWER_LED_ACTIVE_LOW))
#define LOW_BATTERY_FAINT_GLOW() {POWER_LED_OFF();POWER_LED_DISABLE();}
#define LOW_BATTERY_LED_ON() {POWER_LED_ON();POWER_LED_ENABLE();}
#define LOW_BATTERY_OFF_CHARGING_LED_ON() LOW_BATTERY_FAINT_GLOW()
#define LOW_BATTERY_TOGGLE() {if (POWER_LED_IS_ON()) {LOW_BATTERY_FAINT_GLOW();} else {LOW_BATTERY_LED_ON();}}
#else
#define LOW_BATTERY_FAINT_GLOW() {POWER_LED_OFF();POWER_LED_DISABLE();}
#define LOW_BATTERY_LED_ON() {POWER_LED_ON();POWER_LED_ENABLE();}
#define LOW_BATTERY_OFF_CHARGING_LED_ON() {POWER_LED_OFF();POWER_LED_ENABLE();}
#define LOW_BATTERY_TOGGLE() {POWER_LED_ENABLE();	if (POWER_LED_IS_ON()) {POWER_LED_OFF();} else {POWER_LED_ON();}}
#endif
#else
#define LOW_BATTERY_FAINT_GLOW()
#define LOW_BATTERY_OFF_CHARGING_LED_ON()
#define FULL_LED_ON()
#define LOW_BATTERY_TOGGLE()
#endif

#define SET_INDICATED_VOLTAGE_MV_P(str) {set_indicated_voltage_mv_p(PSTR(str));}



static int8_t power_state = 0;
static int8_t power_counter = 0;
static uint16_t voltage_mV = 0;
static int8_t log_state = 0;
static uint16_t indicated_voltage_mV = 0;
static uint16_t activity_time_remaining_s = 0;

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
		ERRLOG_P("median", __LINE__);
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
	LOW_BATTERY_LED_ON();
}

// Time factor determines over how many samples we make the rolling mean value.
// A higher value for filter time gives a better but slower value so its a compromise.
#define MEASURING_FILTER_TIME_FACTOR 64LL
#define MEASURING_FILTER_SCALE_FACTOR 64LL

static uint64_t filtered_voltage_scaled = 0;
static uint8_t filtered_values_available = 0;

// Calculate a rolling/gliding mean value or exponential moving average (EMA),
static uint32_t calc_ema_value(uint32_t latestValue)
{
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
static void read_adc_and_start_next(void)
{
	if (AVR_ADC_isready())
	{
		const uint16_t pv0=AVR_ADC_getSample(0);
		const uint16_t pv1=AVR_ADC_getSample(1);
		const uint16_t pv2=AVR_ADC_getSample(2);
		const uint16_t pv = median_of_3(pv0, pv1, pv2);

		AVR_ADC_startSampling();

		const uint32_t latest_uV = ((uint32_t)pv * MicroVolt_PER_ADC_UNIT);

		const uint32_t filtered_uV = calc_ema_value(latest_uV);

		voltage_mV = filtered_uV / 1000;
	}
	else
	{
		uart_print_PL(PSTR("p timeout"));
		voltage_mV = TIMEOUT_ADC;
	}
}

static int8_t is_activity_timeout(void)
{
	return activity_time_remaining_s == 0;
}

static int8_t is_battery_charged(void)
{
	return (voltage_mV > BATTERY_FULL_MV) || ((voltage_mV > BATTERY_STOP_CHARGING_MV));
}

static int8_t is_voltage_rising(void)
{
	return ((voltage_mV ) > indicated_voltage_mV + DETECT_MARGIN_MV);
}


static int8_t is_voltage_falling(void)
{
	return ((voltage_mV + DETECT_MARGIN_MV) < indicated_voltage_mV);
}


static void set_indicated_voltage_mv_p(const char *pgm_addr)
{
	char tmp[32];
	utility_lltoa(voltage_mV, tmp, 10);
	UART_PRINT_P("p ");
	uart_print_P(pgm_addr);
	uart_putchar(' ');
	uart_print(tmp);
	UART_PRINT_PL(" mV");
	indicated_voltage_mV = voltage_mV;
}

static void set_charging_indicator_led_on(void)
{
	if (power_activity_time_remaining_s() == 0)
	{
		LOW_BATTERY_OFF_CHARGING_LED_ON();
	}
	else
	{
		LOW_BATTERY_FAINT_GLOW();
	}
}

static void enter_battery_full(void)
{
	SET_INDICATED_VOLTAGE_MV_P("full");
	LOW_BATTERY_FAINT_GLOW();
	RELAY_OFF();
	power_counter = 0;
	power_state=POWER_FULL_STATE;
}

static void enter_battery_depleted(void)
{
	SET_INDICATED_VOLTAGE_MV_P("depleted");
	beep_fifo_put(MID_TONE, 500, ee.audio_volume_percent);
	avr_delay_ms_16(100);
	LOW_BATTERY_LED_ON();
	RELAY_OFF();
	power_counter = 0;
	power_state=POWER_DEPLETED_STATE;
}

static void enter_power_off(void)
{
	SET_INDICATED_VOLTAGE_MV_P("inactive");
	beep_fifo_put(MID_TONE, 500, ee.audio_volume_percent);
	avr_delay_ms_16(100);
	LOW_BATTERY_FAINT_GLOW();
	RELAY_OFF();
	power_counter = 0;
	power_state=POWER_DEPLETED_STATE;
}

static void enter_charging(void)
{
	set_charging_indicator_led_on();
	SET_INDICATED_VOLTAGE_MV_P("charging");
	power_state=POWER_CHARGING_STATE;
}

static void enter_discharging_state(void)
{
	LOW_BATTERY_FAINT_GLOW();
	SET_INDICATED_VOLTAGE_MV_P("discharging");
	power_state=POWER_DISCHARGING_STATE;
}

static void enter_measuring_with_relay_active(void)
{
	SET_INDICATED_VOLTAGE_MV_P("measuring");
	set_charging_indicator_led_on();
	RELAY_ON();
	power_counter = 0;
	power_state=POWER_MEASURING_STATE;
}

static void enter_usb(void)
{
	SET_INDICATED_VOLTAGE_MV_P("usb");
	LOW_BATTERY_OFF_CHARGING_LED_ON();
	power_counter = 60;
	power_state=POWER_USB_STATE;
}

static void enter_overfull(void)
{
	LOW_BATTERY_LED_ON();
	SET_INDICATED_VOLTAGE_MV_P("overfull");
	power_state=POWER_OVERFULL_STATE;
}

static void enter_fail(void)
{
	beep_fifo_put(MID_TONE, 100, 100);
	SET_INDICATED_VOLTAGE_MV_P("fail");
	LOW_BATTERY_LED_ON();
	power_counter = 0;
	power_state=POWER_FAIL_STATE;
}

static void enter_low_battery_warning_state(void)
{
	LOW_BATTERY_LED_ON();
	SET_INDICATED_VOLTAGE_MV_P("warning");
	power_state=POWER_LOW_BATTERY_WARNING_STATE;
}


void power_tick_s(void)
{
    switch (power_state)
    {
		case POWER_IDLE_STATE:
		{
			// Initial state, start an ADC conversion and go to startup state.
			RELAY_ON();
			LOW_BATTERY_LED_ON();
			AVR_ADC_startSampling();
			UART_PRINT_PL("p startup");
			power_counter = 0;
			power_state = POWER_STARTUP_STATE;
			break;
		}
		case POWER_STARTUP_STATE:
		{
			// Wait in this state with relay on for a while so that things are stable.
			// Then go to normal/charging state.
			read_adc_and_start_next();

			if (voltage_mV == TIMEOUT_ADC)
			{
				enter_fail();
			}
			else if (power_counter<30)
			{
				// Stay in this state a little so that measurements stabilize.
				++power_counter;
			}
			else
			{
				enter_measuring_with_relay_active();
			}
			break;
		}
		case POWER_MEASURING_STATE:
		{
			read_adc_and_start_next();
			set_charging_indicator_led_on();

			if (voltage_mV == TIMEOUT_ADC)
			{
				enter_fail();
			}
			else if (is_battery_charged())
			{
				enter_battery_full();
			}
			else if (is_voltage_rising())
			{
				enter_charging();
			}
			else if (is_voltage_falling())
			{
				enter_discharging_state();
			}
			break;
		}
		case POWER_CHARGING_STATE:
		{
			read_adc_and_start_next();
			set_charging_indicator_led_on();

			if (voltage_mV == TIMEOUT_ADC)
			{
				enter_fail();
			}
			else if (is_battery_charged())
			{
				enter_battery_full();
			}
			else if (is_voltage_falling())
			{
				enter_measuring_with_relay_active();
			}
			else if ((voltage_mV ) > indicated_voltage_mV + DETECT_MARGIN_MV)
			{
				SET_INDICATED_VOLTAGE_MV_P("rising");
			}
			break;
		}
		case POWER_DISCHARGING_STATE:
		{
			read_adc_and_start_next();

			if (voltage_mV == TIMEOUT_ADC)
			{
				enter_fail();
			}
			else if (is_battery_charged())
			{
				enter_battery_full();
			}
			else if (voltage_mV < BATTERY_LOW_BATTERY_WARNING_MV)
			{
				enter_low_battery_warning_state();
			}
			else if (is_activity_timeout())
			{
				// Inactive and running on battery so turn off
				enter_power_off();
			}
			else if (is_voltage_rising())
			{
				// voltage is rising
				enter_measuring_with_relay_active();
			}
			else if (is_voltage_falling())
			{
				// voltage is continuing to fall
				SET_INDICATED_VOLTAGE_MV_P("falling");
			}
			break;
		}
		case POWER_LOW_BATTERY_WARNING_STATE:
		{
			LOW_BATTERY_TOGGLE();
			read_adc_and_start_next();

			if (voltage_mV == TIMEOUT_ADC)
			{
				enter_fail();
			}
			else if (is_battery_charged())
			{
				// Battery must be full
				enter_battery_full();
			}
			else if (voltage_mV < BATTERY_DEPLETED_MV)
			{
				// Battery is empty, turn off
				enter_battery_depleted();
			}
			else if (is_activity_timeout())
			{
				// Inactive and running on battery so turn off
				enter_power_off();
			}
			else if ((voltage_mV ) > indicated_voltage_mV + DETECT_MARGIN_MV)
			{
				// voltage is rising
				enter_measuring_with_relay_active();
			}
			break;
		}
		case POWER_FULL_STATE:
		{
			RELAY_OFF();
			read_adc_and_start_next();

			if (voltage_mV == TIMEOUT_ADC)
			{
				enter_fail();
			}
			else if (voltage_mV > VOLTAGE_INDICATE_USB_ONLY_MV)
			{
				enter_usb();
			}
			else if (power_counter < 125)
			{
				++power_counter;
			}
			else if (voltage_mV > (indicated_voltage_mV + (OVER_CHARGING_WARNING_MARGIN_MV)))
			{
				// relay is off but voltage is still rising (slowly), not good.
				enter_overfull();
			}
			break;
		}
		case POWER_DEPLETED_STATE:
		{
			LOW_BATTERY_TOGGLE();
			RELAY_OFF();
			read_adc_and_start_next();
			if (voltage_mV == TIMEOUT_ADC)
			{
				enter_fail();
			}
			break;
		}
		case POWER_USB_STATE:
		{
			// Voltage indicated that charging is done and battery is disconnected as needed.
			RELAY_OFF();
			read_adc_and_start_next();
			if (voltage_mV == TIMEOUT_ADC)
			{
				enter_fail();
			}
			break;
		}
		case POWER_OVERFULL_STATE:
		{
			RELAY_OFF();
			read_adc_and_start_next();

			// Relay should be off but voltage level indicated it is not.
			// It may be simply low voltage from USB (not a problem) or relay
			// has not disconnected battery as it should (that is a problem).
			// If relay did not disconnect try to raise alarm and use as
			// much power as possible.
			SET_INDICATED_VOLTAGE_MV_P("overfull");

			beep_fifo_put(MID_TONE, 950, 100);
			beep_led_on(950);
			VIB_ON();
			LASER_ON();
			LOW_BATTERY_TOGGLE();

			break;
		}
		case POWER_FAIL_STATE:
		default:
		{
			LOW_BATTERY_TOGGLE();
			RELAY_OFF();
			if (power_counter<10)
			{
				beep_fifo_put(MID_TONE, 200, ee.audio_volume_percent);
				beep_led_on(501);
				++power_counter;
			}
			else
			{
				SET_INDICATED_VOLTAGE_MV_P("restart");
				LOW_BATTERY_LED_ON();
				AVR_ADC_init();
				power_state=POWER_IDLE_STATE;
			}
			break;
		}
    }

    // This is for debugging, can be removed later.
	if (power_state != log_state)
	{
		UART_PRINT_P("p s ");
		uart_print_hex4(power_state);
		uart_print_crlf();
		log_state = power_state;
	}

	if (activity_time_remaining_s > 0)
	{
		activity_time_remaining_s--;
	}

    avr_wtd_reset_from_power();
}

uint16_t power_get_voltage_mV()
{
	return voltage_mV;
}

int8_t power_get_state()
{
	return power_state;
}


void power_inactive(void)
{
	UART_PRINT_PL("p ia");
	activity_time_remaining_s = 0;
}

uint16_t power_activity_time_remaining_s(void)
{
	return activity_time_remaining_s;
}

void power_activity_set_time_remaining_s(uint16_t time_remaining_s)
{
	UART_PRINT_PL("p a");
	if (activity_time_remaining_s < time_remaining_s)
	{
		activity_time_remaining_s = time_remaining_s;
	}
}

int8_t power_is_low_battery(void)
{
	return (power_state == POWER_LOW_BATTERY_WARNING_STATE);
}

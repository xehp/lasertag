/*
power.h

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

#ifndef POWER_H
#define POWER_H

enum {
	POWER_IDLE_STATE,
	POWER_STARTUP_STATE,
	POWER_MEASURING_STATE,
    POWER_CHARGING_STATE,
	POWER_DISCHARGING_STATE,
	POWER_FULL_STATE,
	POWER_USB_STATE,
	POWER_DEPLETED_STATE,
	POWER_FAIL_STATE,
	POWER_OVERFULL_STATE,
	POWER_LOW_BATTERY_WARNING_STATE,
};


void power_init(void);
void power_tick_s(void);
uint16_t power_get_voltage_mV(void);
int8_t power_get_state(void);
void power_inactive(void);
uint16_t power_activity_time_remaining_s(void);
void power_activity_set_time_remaining_s(uint16_t time_remaining_s);
int8_t power_is_low_battery(void);

#endif

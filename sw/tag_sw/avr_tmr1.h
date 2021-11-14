/*
avr_tmr1.h

Provide functions to set up timer 1 hardware.

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

2021-04-20 created using code from avr_tmr0.h Henrik Bjorkman
*/


#ifndef AVR_TMR1_H
#define AVR_TMR1_H

#include "avr_cfg.h"

#ifdef IR_OUTPUT_USE_TMR1

// set up hardware (port directions, registers etc.)
void avr_tmr1_init(void);

void avr_tmr1_pwm_off(void);
void avr_tmr1_pwm_on(void);


#elif defined BEEP_USE_TMR1


#define LOW_TONE 0xFFFF
#define MID_TONE 0x3FFF
#define HIGH_TONE 0xFFF
#define VHIGH_TONE 0x3FF
#define NO_TONE 0



// set up hardware (port directions, registers etc.)
void avr_tmr1_init(void);

void avr_tmr1_pwm_off(void);
void avr_tmr1_pwm_on(uint16_t tone, uint8_t volume_percent);


#endif
#endif

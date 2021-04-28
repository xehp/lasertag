/*
avr_tmr2.h

Provide functions to set up timer 2 hardware.

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


#ifndef AVR_TMR2_H
#define AVR_TMR2_H


#ifdef AVR_SYS_USE_TMR2

// With 16 MHz a divider of 128*125 will give one tick per milli second.
#define AVR_TMR2_DIVIDER (128L*125L)
#define AVR_TMR2_TICKS_PER_SEC (AVR_FOSC/AVR_TMR2_DIVIDER)

#if AVR_TMR2_TICKS_PER_SEC == 1000
#define AVR_TMR2_TRANSLATE_MS_TO_TICKS_64(time_ms) (time_ms)
#define AVR_TMR2_TRANSLATE_TICKS_TO_MS_64(ticks) (ticks)
#define AVR_TMR2_TRANSLATE_MS_TO_TICKS_16(time_ms) (time_ms)
#define AVR_TMR2_TRANSLATE_TICKS_TO_MS_16(ticks) (ticks)
#else
#define AVR_TMR2_TRANSLATE_MS_TO_TICKS_64(time_ms) ((((int64_t)time_ms * (AVR_FOSC/1000L))) / AVR_TMR2_DIVIDER)
#define AVR_TMR2_TRANSLATE_TICKS_TO_MS_64(ticks) ((((int64_t)ticks * AVR_TMR2_DIVIDER)) / (AVR_FOSC/1000L))
#define AVR_TMR2_TRANSLATE_MS_TO_TICKS_16(time_ms) ((((AVR_FOSC/1000L) * time_ms)) / AVR_TMR2_DIVIDER)
#define AVR_TMR2_TRANSLATE_TICKS_TO_MS_16(ticks) (((AVR_TMR2_DIVIDER * ticks)) / (AVR_FOSC/1000L))
#endif

// set up hardware (port directions, registers etc.)
void avr_tmr2_init(void);

int64_t avr_tmr2_get_tick_64(void);
int16_t avr_tmr2_get_tick_16(void);

#else

// set up hardware (port directions, registers etc.)
void avr_tmr2_init(void);

void avr_tmr2_pwm_off(void);
void avr_tmr2_pwm_on(void);

#endif

#endif

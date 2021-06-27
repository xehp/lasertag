/*
avr_misc.h

Provide functions to set up the general basics of the system.

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

2005-02-16 Created. Henrik Bjorkman
2005-07-01 cleanup Henrik
*/


#ifndef AVR_MISC_H
#define AVR_MISC_H

#include <stdint.h>


#ifndef NULL
#define NULL 0
#endif




#define AVR_ERROR_HANDLER_P(str, n) {avr_error_handler_P(PSTR(str), n);}

// To save flash memory the __FILE__ is not given here for now. Add it if needed.
#define ASSERT(c) {if (!(c)) {AVR_ERROR_HANDLER_P("ASSERT ", __LINE__);}}



int avr_blink_debug_led(uint8_t n);


// set up hardware (port directions, registers etc.)
void avr_init(void);


// busy-wait seconds, milliseconds or microseconds
void avr_delay_ms_16(int delay_ms);
void avr_delay_us(int delay_us);

int16_t avr_systime_ms_16(void);
int32_t avr_systime_ms_32(void);
int64_t avr_systime_ms_64(void);

// Enter power save mode
void avr_wtd_reset_and_idle(void);

// Reset the extra power watch dog.
// Only power is allowed to call this reset.
void avr_wtd_reset_from_power(void);

void avr_error_handler_P(const char *pgm_addr, uint16_t errorCode);



// Just calling avr_delay_us takes about 10 us so subtracting that.
#define AVR_DELAY_US(t) {avr_delay_us((t)>=10?(t)-10:0);}


#endif // AVR_H

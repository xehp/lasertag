/*
beep.h

Provide functions to do audio beeps.

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

2021-04-22 Created. /Henrik
*/

#ifndef BEEP_H
#define BEEP_H

void beep_init(void);
void beep_process(void);

uint8_t beep_get_free_space_in_fifo(void);
void beep_fifo_put(uint16_t tone, uint16_t duration_ms, uint8_t volume_in_percent);
void beep_fifo_clear(void);
int8_t beep_fifo_is_empty(void);

void beep_led_on(uint16_t duration_ms);
void beep_led_off(void);

uint16_t beep_tonecode_from_frequency(uint16_t frequency_Hz);

#endif

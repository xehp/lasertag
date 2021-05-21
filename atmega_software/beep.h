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

uint8_t beep_get_free_space_in_queue(void);
void beep_fifo_put(uint16_t tone, uint16_t duration_ms);

void beep_led_on(void);
void beep_led_off(void);

#endif

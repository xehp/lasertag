/*
game.h

The main state of this program.

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

#ifndef GAME_H
#define GAME_H

#include "utility.h"




void game_init(void);
void game_process(void);
void game_tick_s(void);
int8_t game_get_state(void);
void game_message_category_received(struct utility_r_struct *urs, uint8_t sub, uint32_t src_addr, int8_t radio_pipe);
uint16_t is_other_dev_is_alive_s(void);


uint32_t game_pair_addr(void);
int64_t game_pair_nr(void);

#endif

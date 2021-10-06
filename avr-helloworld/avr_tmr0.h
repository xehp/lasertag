/*
avr_tmr0.h

provide functions to set up hardware

Copyright 2005 EIT European Infotech AB www.eit.se.

This file is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

History

2005-07-02
created using code from avr_misc.h
Henrik Björkman
*/


#ifndef AVR_TMR0_H
#define AVR_TMR0_H



#define AVR_TMR0_TICKS_PER_SEC() (AVR_FOSC/(1024L*256L))

// set up hardware (port directions, registers etc.)
void avr_tmr0_init(void);

int avr_tmr0_gettick(void);




#endif

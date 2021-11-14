/*
avr_tmr0.c 

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

For a list ov available signals see:
http://www.nongnu.org/avr-libc/user-manual/index.html

History

2005-07-02
created using code from avr_misc.c
Henrik Björkman

*/

// When reading 16bit reg remember to read low byte first then high byte
// when writing write high byte first then low byte but hopefully the C compiler does this.


// includes

#include <avr/interrupt.h>
//#include <avr/signal.h>
#include <avr/pgmspace.h>

#include "avr_cfg.h"
#include "avr_tmr0.h"

// local variables and defines




volatile int timer0count;

// SIG_OVERFLOW0 renamed to TIMER0_OVF_vect acording to:
// http://www.nongnu.org/avr-libc/user-manual/group__avr__interrupts.html

SIGNAL (TIMER0_OVF_vect)
{
  timer0count++;

  if (timer0count&0x100)
  {
    PORTC &= ~_BV(PC5);
  }
  else
  {
    PORTC|= _BV(PC5);
  }
}

int avr_tmr0_gettick(void)
{
  int tmp;
  TIMSK0&=(unsigned char)~(1<<TOIE0); // disable interrupt, I hope
  tmp=timer0count;
  TIMSK0|=(1<<TOIE0); // enable interrupt
  return tmp;
}

// set up hardware (port directions, registers etc.)
void avr_tmr0_init(void) 
{
  // set up timer 0
  TCCR0B=0x05; // internal clock with prescaler to 1024 gives interrupt frequency=AVR_FOSC/(1024*256)
  TIMSK0|=(1<<TOIE0); // enable interrupt 
}









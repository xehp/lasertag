/*
avr_misc.c 

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

2005-02-16
Created. 
Henrik Björkman

2005-02-20
Set up timer0 interrupt
Henrik Björkman

2005-02-22
Using timer1 to generate 38 MHz. 
Will need that to send IR later.
Henrik Björkman

2005-02-22
Using timer2 to get a timer with a 1 second resolution
Henrik Björkman

2005-02-22
Will try to use timer2 interrupt to
make a software uart together with timer1

2005-07-01
cleanup
Henrik

2005-07-02
Moved timer drivers to own files.
Henrik

*/

// When reading 16bit reg remember to read low byte first then high byte
// when writing write high byte first then low byte but hopefully the C compiler does this.


// includes

#include <avr/interrupt.h>
//#include <avr/signal.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>

#include "avr_cfg.h"
#include "avr_misc.h"
#include "avr_uart.h"
#include "avr_tmr0.h"

#if ((defined(__AVR_ATmega88__)) || (defined(__AVR_ATmega168__)) || (defined(__AVR_ATmega328P__)))
// OK
#else
#error /* well it should work with other atmega mcus to but that is not tested. */
#endif



#define DEBUG_UART_OUTBUF_EMPTY() (!( UCSR0A & (1<<UDRE0)))
#define DEBUG_UART_INBUF_EMPTY() (!(UCSR0A & (1<<RXC0)))


// local variables and defines






#ifdef DEBUG_LED_PORT

// Flash n times with the LED, just to see that CPU is running before doing anything else.
// This may be called before debug_init has been called.
int avr_blink(char n)
{
 
  avr_delay_ms(200);
 
  /* INITIALIZE */

  /* set output to off */
  DEBUG_LED_OFF();

  /* enable PC5 as output */
  DEBUG_LED_ENABLE();

  /* BLINK, BLINK ... */

  while (n>0) 
  {
    /* set LED on */
    DEBUG_LED_ON();

    avr_delay_ms(200);

    /* set LED off */
    DEBUG_LED_OFF();

    avr_delay_ms(800);

    n--;
    
  }

  /* disable debug LED as output */
  DEBUG_LED_DISABLE();


  avr_delay_ms(500);


  avr_delay_ms(500);


  return 0;
}

#endif



// This function will set CPU in idle mode, saving energy.
void avr_idle(void) 
{
  wdt_reset();

  // enable sleep
  SMCR|=(1<<SE);
  
  // if needed, reduce master clock speed here
  // using XDIV register

  // if needed, turn of current for analog comparator here
    
  // set sleep mode to idle
  SMCR&=~(7<<SM0);
  //MCUCR|=(0<<SM0);
  
  // Here we go to sleep
  asm ("sleep");

  // we continue here after beeing wakened up by an interrupt.

  // reset clock speed to normal value
  // using XDIV register

  // turn on current for analog comparator here (if we turned it off)

  // disable sleep
  SMCR&=~(1<<SE);
}

// busy-wait approximately s seconds
// Don't use this for to many seconds (s*AVR_TMR0_TICKS_PER_SEC() must not be more than ~30000)
// timer 0 must be running for this to work, this uses less power than avr_delay_ms does.
void avr_delay_s(int s) 
{
  int t=avr_tmr0_gettick()+(s*AVR_TMR0_TICKS_PER_SEC());
  while((t-avr_tmr0_gettick())>0)
  {
    // wait
    wdt_reset();
    avr_idle();
  }
}

// busy-wait approximatly ms milliseconds (if function is not interrupted)
#if 1
void avr_delay_ms(int ms) 
{
  int i;
  while(ms>0)
  {
    for (i=0; i<(AVR_FOSC/6000L); i++)
    {      
      wdt_reset(); // If wdt_reset isn't wanted, put a no OP here instead: asm ("nop");
    }
    ms--;
  }
}
#else
// alternative function using the avr_delay_us below
void avr_delay_ms(int ms) 
{
  while(ms>0)
  {
    avr_delay_us(990);
    ms--;
  }
}
#endif


// For better precision on pulses disable interrupt when using this.
// cli(); // disable global interrupts
// sei(); // enable global interrupts
void avr_delay_us(int us) 
{
  if (us==0)
  {
    return;
  }
  else
  {  
    us--;
    while(us>0)
    {
#if AVR_FOSC==16000000L
      asm ("nop"); 
      asm ("nop"); 
      asm ("nop"); 
      asm ("nop"); 
      asm ("nop"); 
      asm ("nop"); 
      asm ("nop"); 
      asm ("nop"); 
      asm ("nop"); 
#elif AVR_FOSC==8000000L
      asm ("nop"); 
#elif AVR_FOSC==20000000L
      asm ("nop"); 
      asm ("nop"); 
      asm ("nop"); 
      asm ("nop"); 
      asm ("nop"); 
      asm ("nop"); 
      asm ("nop"); 
      asm ("nop"); 
      asm ("nop"); 
      asm ("nop"); 
      asm ("nop"); 
      asm ("nop"); 
      asm ("nop"); 
#else
#error selected AVR_FOSC not supported by delay_us
#endif
      us--;
    }
  }
}





// set up hardware (port directions, registers etc.)
void avr_init() 
{

  // Analog comparator
  //ACSR|=1<<ACD; // disable analog comparator to save power


  sei(); // enable global interrupts

  
}









/*
main.c

Henriks AVR application
Copyright Henrik Bjorkman www.eit.se/hb 2005
All rights reserved etc etc...

For hardware see: 
H:\d\eget\projects\inteligent_electronic_thermostat\hw\0.1


History

2005-02-14
want to use serial port. Have adapted some code I found
at http://www.lka.ch/projects/avr/
Henrik Bjorkman

2005-02-20
Will try to interpret some commands
Henrik

*/


#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "avr_cfg.h"
#include "avr_misc.h"
#include "avr_uart.h"
#include "avr_tmr0.h"
#include "version.h"






int mainTimer=0;
int mainCounter=0;






int main( void ) 
{
  wdt_enable(WDTO_2S);

  avr_init();

  avr_tmr0_init();

  sei(); // enable global interrupts



  #ifdef DEBUG_LED_PORT
    avr_blink(2); // just so we see that it started
  #else
    avr_delay_ms(200);
  #endif

  wdt_reset();

  // enable global interrupts, disable analog comparator to save power.
  avr_init();

  //AVR_ADC_init();

  #if defined(USE_TIMER0_TICK) || defined(USE_TIMER0_TONE_GENERATOR) || defined(USE_TIMER0_SOUND)
    avr_tmr0_init();
  #endif

  
  #ifdef AVR_TIMER1_FREQUENCY
    avr_tmr1_init();
  #endif

  /*
  #ifdef AVR_TIMER2_TICKS_PER_SEC
    avr_tmr2_init();
  #else
    #error
  #endif
  */

  /*
  #ifdef DEBUG_TEST
    debug_init();
  #endif
  */



  uart_init();



  uart_print_P(PSTR("\r\n" VERSION_STRING "\r\n")); 





  

  mainTimer=avr_tmr0_gettick()+AVR_TMR0_TICKS_PER_SEC();



  for(;;)
  {
    const int t=avr_tmr0_gettick();
    int ch = uart_getchar();
    if (ch>=0)
    {
      uart_putchar(ch);
      mainTimer=t+10*AVR_TMR0_TICKS_PER_SEC();
    }

    if ((mainTimer-t)<0)
    {
      uart_putchar('.');
      mainTimer+=AVR_TMR0_TICKS_PER_SEC();
    }



    wdt_reset();

    // Set CPU in idle mode to save energy, it will wake up next time there is an interrupt
    avr_idle();

  }

  

  return(0);
} // end main()




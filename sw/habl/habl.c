/*
habl.c

Configure baudrate and CPU frequency in avr_cfg.h.

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

2007-03-15 Created Henrik Bjorkman

*/


#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>

/*#include <stdio.h>*/
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "avr_cfg.h"
#include "avr_uart.h"
#include "version.h"
#include "xmodem.h"



#define CANCEL_AUTOBOOT_TIME 5 // in seconds (roughly)
#define CANCEL_AUTOBOOT_CHAR 27 // 27 = ESC
#define N_CANCEL_CHARS_REQUESTED 5

#define TIMERCOUNT_START_VALUE ((CANCEL_AUTOBOOT_TIME*30000L)*(AVR_FOSC/1000000L))


// Application is assumed to be at address 0. If this is changed something in xmodem needs to be changed also.
void (*app)(void)=0;




int main( void ) 
{
  long timerCount=TIMERCOUNT_START_VALUE;
  unsigned char xCount=0;


  uart_init();

#ifdef ENABLE_WDT
  wdt_enable(WDTO_2S);
#endif


  #ifdef WELCOME_STRING
  uart_print_P(PSTR(WELCOME_STRING)); 
  #endif



  while(timerCount!=0)
  {
    int c=uart_getchar();
    if (c==CANCEL_AUTOBOOT_CHAR)
    {
      // It was an esc char. Now count the number of these.
      ++xCount;
    }
    else if (c>0)
    {
      // It was not an esc char.
      xCount=0;
      break;
    }
    --timerCount;
  }

  if (xCount==N_CANCEL_CHARS_REQUESTED)
  {
    xmodem();
  }


  // Call the application.
  (app)();


  return(0);
} // end main()




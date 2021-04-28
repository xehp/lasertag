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


#ifndef NULL
#define NULL 0
#endif



#ifdef DEBUG_LED_PORT


/* enable debug LED as output */
#define DEBUG_LED_ENABLE() DEBUG_LED_DDR|= _BV(DEBUG_LED_BIT);

/* disable debug LED as output */
#define DEBUG_LED_DISABLE() DEBUG_LED_DDR&= ~(_BV(DEBUG_LED_BIT));

/* set debug LED output to on or off */
#ifdef DEBUG_LED_ACTIVE_HIGH
#define DEBUG_LED_ON() DEBUG_LED_PORT|= _BV(DEBUG_LED_BIT);
#define DEBUG_LED_OFF() DEBUG_LED_PORT &= ~_BV(DEBUG_LED_BIT);
#else
#define DEBUG_LED_ON() DEBUG_LED_PORT &= ~_BV(DEBUG_LED_BIT);
#define DEBUG_LED_OFF() DEBUG_LED_PORT|= _BV(DEBUG_LED_BIT);
#endif

#else

// Dummy macros, in case there is no debug LED.
#define DEBUG_LED_ENABLE()
#define DEBUG_LED_DISABLE()
#define DEBUG_LED_ON()
#define DEBUG_LED_OFF()

#endif

// To save flash memory the __FILE__ is not given here for now. Add it if needed.
#define ASSERT(c) {if (!(c)) {avr_error_handler_P(PSTR("ASSERT "), __LINE__);}}


int avr_blink(char n);


// set up hardware (port directions, registers etc.)
void avr_init(void);


// busy-wait seconds, milliseconds or microseconds
void avr_delay_ms_16(int delay_ms);
void avr_delay_us(int delay_us);

int16_t avr_systime_ms_16(void);
int64_t avr_systime_ms_64(void);

// Enter power save mode
void avr_idle(void);

void avr_error_handler_P(const char *pgm_addr, uint16_t errorCode);


#if 0
void debug_uart_init(void);


/* send if not using interrupt */
void debug_uart_putchar(char data );


/* receive if not using interrupt */
/* wait until a character is available */
unsigned char debug_uart_waitchar( void );


/* receive if not using interrupt */
/* return -1 if no character was available */
int debug_uart_getchar(void);


void debug_uart_print_P(const char *addr);

void debug_uart_test(void);


/* send if not using interrupt */
void debug_uart_putchar(char data );


/* receive if not using interrupt */
/* wait until a character is available */
unsigned char debug_uart_waitchar( void );


/* receive if not using interrupt */
/* return -1 if no character was available */
int debug_uart_getchar(void);

void debug_uart_print_P(const char *addr);

void debug_uart_test(void);
#endif


// Just calling avr_delay_us takes about 10 us so subtracting that.
#define AVR_DELAY_US(t) {avr_delay_us((t)>=10?(t)-10:0);}


#endif // AVR_H

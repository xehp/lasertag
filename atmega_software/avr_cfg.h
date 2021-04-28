/*
avr_cfg.h

provide functions to configure SW to match hardware

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

2005-03-23 Created by Henrik Bjorkman
2021-04-20 Updated. /Henrik
*/


#ifndef AVR_CFG_H
#define AVR_CFG_H

#define AVR_FOSC 16000000L

// Recommended baud rates are 300, 9600 or 19200,
// If FOSC is 16 MHz and 115200 is selected here we get 125000 baud instead.
#define UART_BAUDRATE 9600


// Using pin PD3 for debug LED
#define DEBUG_LED_DDR DDRD
#define DEBUG_LED_PORT PORTD
#define DEBUG_LED_BIT PD3


// Configure timer use. Make sure to use each timer for only one thing.


// We can use timer0 or timer2 as system clock
// Uncomment one of these:
//#define AVR_SYS_USE_TMR0
#define AVR_SYS_USE_TMR2


// On next HW version 2021-04-23 and later IR output will be on TMR0.
// Uncomment only one of the below
//#define IR_OUTPUT_USE_TMR0
#define IR_OUTPUT_USE_TMR1


// On next HW version 2021-04-23 and later beep will be on TMR1.
// Uncomment only one of the below
#define BEEP_USE_TMR0
//#define BEEP_USE_TMR1




// Configuration of IO pins
// Uncomment ACTIVE_HIGH macros if input/output is active low.


#define RELAY_DDR DDRB
#define RELAY_PORT PORTB
#define RELAY_BIT PB0
//#define RELAY_ACTIVE_HIGH


// Output for hit indication LEDs, PB2
#define HIT_LEDS_DDR DDRB
#define HIT_LEDS_PORT PORTB
#define HIT_LEDS_BIT PB2
#define HIT_LEDS_ACTIVE_HIGH
// NOTE in next HW version 2021-04-23 and later HIT LEDS will be on PD5 (pin 11 of 28)

// Output for Laser, PD7
#define LASER_DDR DDRD
#define LASER_PORT PORTD
#define LASER_BIT PD7
#define LASER_ACTIVE_HIGH


// Output for vibrator, PC5
#define VIB_DDR DDRC
#define VIB_PORT PORTC
#define VIB_BIT PC5
#define VIB_ACTIVE_HIGH



// Radio Chip enable (assumed active high) is on PC4
#define RADIO_CE_DDR DDRC
#define RADIO_CE_PORT PORTC
#define RADIO_CE_BIT PC4
#define RADIO_CE_ACTIVE_HIGH

// Radio CSN Chip select (active low) is on PD4
#define RADIO_CSN_DDR DDRD
#define RADIO_CSN_PORT PORTD
#define RADIO_CSN_BIT PD4
//#define RADIO_CSN_ACTIVE_HIGH


// Radio IRQ active low on PD2
#define RADIO_IRQ_PORT PORTD
#define RADIO_IRQ_PORT_PIN PIND
#define RADIO_IRQ_BIT PD2
//#define RADIO_IRQ_ACTIVE_HIGH



// Input for trigger button, PC2
#define TRIGGER_PORT PORTC
#define TRIGGER_PORT_PIN PINC
#define TRIGGER_BIT PC2
//#define TRIGGER_ACTIVE_HIGH
// NOTE in next HW version 2021-04-24 and later trigger will be active high


// Input for internal IR detector, PC3
#define INTERNAL_IR_PORT PORTC
#define INTERNAL_IR_PIN PINC
#define INTERNAL_IR_BIT PC3
//#define INTERNAL_IR_ACTIVE_HIGH

// Input for external IR detector, PC1
#define EXTERNAL_IR_PORT PORTC
#define EXTERNAL_IR_PIN PINC
#define EXTERNAL_IR_BIT PC1
//#define EXTERNAL_IR_ACTIVE_HIGH



#endif

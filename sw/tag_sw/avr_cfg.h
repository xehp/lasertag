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

// config HW_VERSION:
// 0      2021-04-05 a legacy version
// 1      2021-04-23 or 2021-04-27 (PCBs with no SMD)
// 2      2021-05-27 (wrong transistor was used)
// 3      2021-06-14 detector board
// 4      2021-10-24 Pointer_board_hammond_box (also FT232RL/FT230XS)
#define HW_VERSION 4
// TODO Move this to its own file since we change it often.



#if HW_VERSION <= 2
#define AVR_FOSC 16000000L
#else
#define AVR_FOSC 8000000L
#endif
//#define AVR_FOSC 7370000



// Recommended baud rates are 300, 9600 or 19200,
// If FOSC is 16 MHz and 115200 is selected here we get 118000 baud instead.
#define UART_BAUDRATE 9600


// Define this if its to be used as a radio modem/GW.
//#define RADIO_MODEM_ONLY


// Using pin PD3 for debug LED
//#define DEBUG_LED_DDR DDRD
//#define DEBUG_LED_PORT PORTD
//#define DEBUG_LED_BIT PD3

// Using pin PD3 for charging indication LEDs
#define POWER_LED_DDR DDRD
#define POWER_LED_PORT PORTD
#define POWER_LED_BIT PD3
#if HW_VERSION > 3
#define POWER_LED_ACTIVE_LOW
#endif

// Configure timer use. Make sure to use each timer for only one thing.


// We can use timer0 or timer2 as system clock
// Uncomment one of these:
//#define AVR_SYS_USE_TMR0
#define AVR_SYS_USE_TMR2


// Remember that IR and BEEP can not use same timer.

// On next HW version 2021-04-23 and later IR output will be on TMR0.
// Uncomment only one of the below
#if (HW_VERSION == 0)
#define IR_OUTPUT_USE_TMR1
#else
#define IR_OUTPUT_USE_TMR0
#endif

// On second generation PCB IR was on OCA0/PD6
// On third generation it shall be on OC0B/PD5
// Uncomment only one of the below
//#define IR_USE_OC0A_PD6
#define IR_USE_OC0B_PD5

// On next HW version 2021-04-23 and later beep will be on TMR1.
#ifdef IR_OUTPUT_USE_TMR1
#define BEEP_USE_TMR0
#else
#define BEEP_USE_TMR1
#endif




// Configuration of IO pins

// Relay is on PB0 and i is active low
#define RELAY_DDR DDRB
#define RELAY_PORT PORTB
#define RELAY_BIT PB0
#define RELAY_ACTIVE_LOW

// Output for hit indication LEDs
#if HW_VERSION == 0
#define HIT_LED0_DDR DDRB
#define HIT_LED0_PORT PORTB
#define HIT_LED0_BIT PB2
#define HIT_LED0_ACTIVE_HIGH
#elif HW_VERSION == 2
#define HIT_LED0_DDR DDRD
#define HIT_LED0_PORT PORTD
#define HIT_LED0_BIT PD6
#define HIT_LED0_ACTIVE_LOW
#else
#define HIT_LED0_DDR DDRD
#define HIT_LED0_PORT PORTD
#define HIT_LED0_BIT PD6
#define HIT_LED0_ACTIVE_HIGH
#endif

// Output for Laser, PD7
#if HW_VERSION == 2
#define LASER_DDR DDRD
#define LASER_PORT PORTD
#define LASER_BIT PD7
#define LASER_ACTIVE_LOW
#else
#define LASER_DDR DDRD
#define LASER_PORT PORTD
#define LASER_BIT PD7
#define LASER_ACTIVE_HIGH
#endif

// Output for vibrator, PC5
#if HW_VERSION >= 3
// no vib on later HW
#elif HW_VERSION == 2
#define VIB_DDR DDRC
#define VIB_PORT PORTC
#define VIB_BIT PC5
#define VIB_ACTIVE_LOW
#else
#define VIB_DDR DDRC
#define VIB_PORT PORTC
#define VIB_BIT PC5
#define VIB_ACTIVE_HIGH
#endif


// Radio Chip enable (assumed active high) is on PC4
#define RADIO_CE_DDR DDRC
#define RADIO_CE_PORT PORTC
#define RADIO_CE_BIT PC4
#define RADIO_CE_ACTIVE_HIGH

// Radio CSN Chip select (active low) is on PD4
#define RADIO_CSN_DDR DDRD
#define RADIO_CSN_PORT PORTD
#define RADIO_CSN_BIT PD4
#define RADIO_CSN_ACTIVE_LOW


// Radio IRQ active low on PD2
#define RADIO_IRQ_PORT PORTD
#define RADIO_IRQ_PORT_PIN PIND
#define RADIO_IRQ_BIT PD2
#define RADIO_IRQ_ACTIVE_LOW


// Input for trigger button, PC2
// NOTE in next HW version 2021-04-24 and later trigger will be active high
#define TRIGGER_PORT PORTC
#define TRIGGER_PORT_PIN PINC
#define TRIGGER_BIT PC2
#if HW_VERSION == 0
#define TRIGGER_ACTIVE_LOW
#else
#define TRIGGER_ACTIVE_HIGH
#endif


// Input for internal IR detector, PC3
// This is active low, but also active is for zero. So lets call it active high.
#define INTERNAL_IR_PORT PORTC
#define INTERNAL_IR_PIN PINC
#define INTERNAL_IR_BIT PC3
#define INTERNAL_IR_ACTIVE_HIGH

// Input for external IR detector, PC1
#if HW_VERSION >= 3
#define EXTERNAL_IR_PORT PORTC
#define EXTERNAL_IR_PIN PINC
#define EXTERNAL_IR_BIT PC5
#define EXTERNAL_IR_ACTIVE_HIGH
#else
#define EXTERNAL_IR_PORT PORTC
#define EXTERNAL_IR_PIN PINC
#define EXTERNAL_IR_BIT PC1
#define EXTERNAL_IR_ACTIVE_HIGH
#endif

#if HW_VERSION == 4
// In HW version 4 the EXTERNAL IR Port also controls the LEDs on other side.
#define EXTERNAL_IR_LED_PORT EXTERNAL_IR_PORT
#define EXTERNAL_IR_LED_DDR DDRC
#define EXTERNAL_IR_LED_BIT EXTERNAL_IR_BIT
#define EXTERNAL_IR_LED_ACTIVE_LOW
#endif

#endif

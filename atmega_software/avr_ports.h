/*
avr_ports.h

provide functions to use general purpose IO ports.

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

#ifndef AVR_PORTS_H
#define AVR_PORTS_H

#include "avr_cfg.h"

// Macros to turn various functions on or off


/* enable relay power pin as output */
#define RELAY_ENABLE() RELAY_DDR |= _BV(RELAY_BIT);

/* disable relay power pin as output */
#define RELAY_DISABLE() RELAY_DDR &= ~(_BV(RELAY_BIT));

/* set relay power output to on or off */
#ifdef RELAY_ACTIVE_HIGH
#define RELAY_ON() RELAY_PORT |= _BV(RELAY_BIT);
#define RELAY_OFF() RELAY_PORT &= ~_BV(RELAY_BIT);
#elif defined RELAY_ACTIVE_LOW
#define RELAY_ON() RELAY_PORT &= ~_BV(RELAY_BIT);
#define RELAY_OFF() RELAY_PORT |= _BV(RELAY_BIT);
#endif



/* enable relay power pin as output */
#define HIT_LEDS_ENABLE() HIT_LEDS_DDR |= _BV(HIT_LEDS_BIT);

/* disable relay power pin as output */
#define HIT_LEDS_DISABLE() HIT_LEDS_DDR &= ~(_BV(HIT_LEDS_BIT));

/* set relay power output to on or off */
#ifdef HIT_LEDS_ACTIVE_HIGH
#define HIT_LEDS_ON() HIT_LEDS_PORT |= _BV(HIT_LEDS_BIT);
#define HIT_LEDS_OFF() HIT_LEDS_PORT &= ~_BV(HIT_LEDS_BIT);
#elif defined HIT_LEDS_ACTIVE_LOW
#define HIT_LEDS_ON() HIT_LEDS_PORT &= ~_BV(HIT_LEDS_BIT);
#define HIT_LEDS_OFF() HIT_LEDS_PORT |= _BV(HIT_LEDS_BIT);
#endif



/* enable relay power pin as output */
#define LASER_ENABLE() LASER_DDR |= _BV(LASER_BIT);

/* disable relay power pin as output */
#define LASER_DISABLE() LASER_DDR &= ~(_BV(LASER_BIT));

/* set relay power output to on or off */
#ifdef LASER_ACTIVE_HIGH
#define LASER_ON() LASER_PORT |= _BV(LASER_BIT);
#define LASER_OFF() LASER_PORT &= ~_BV(LASER_BIT);
#elif defined LASER_ACTIVE_LOW
#define LASER_ON() LASER_PORT &= ~_BV(LASER_BIT);
#define LASER_OFF() LASER_PORT |= _BV(LASER_BIT);
#endif


/* enable relay power pin as output */
#define VIB_ENABLE() VIB_DDR |= _BV(VIB_BIT);

/* disable relay power pin as output */
#define VIB_DISABLE() VIB_DDR &= ~(_BV(VIB_BIT));

/* set relay power output to on or off */
#ifdef VIB_ACTIVE_HIGH
#define VIB_ON() VIB_PORT |= _BV(VIB_BIT);
#define VIB_OFF() VIB_PORT &= ~_BV(VIB_BIT);
#elif defined VIB_ACTIVE_LOW
#define VIB_ON() VIB_PORT &= ~_BV(VIB_BIT);
#define VIB_OFF() VIB_PORT |= _BV(VIB_BIT);
#endif

#define TRIGGER_INIT() {TRIGGER_PORT |= _BV(TRIGGER_BIT);} // activate internal pull up
#ifdef TRIGGER_ACTIVE_HIGH
#define TRIGGER_READ() (TRIGGER_PORT_PIN & _BV(TRIGGER_BIT))
#elif defined TRIGGER_ACTIVE_LOW
#define TRIGGER_READ() ((TRIGGER_PORT_PIN & _BV(TRIGGER_BIT)) == 0)
#endif

#define INTERNAL_IR_INIT() {INTERNAL_IR_PORT |= _BV(INTERNAL_IR_BIT);} // activate internal pull up
#ifdef INTERNAL_IR_ACTIVE_HIGH
#define INTERNAL_IR_READ() (INTERNAL_IR_PIN & _BV(INTERNAL_IR_BIT))
#elif defined INTERNAL_IR_ACTIVE_LOW
#define INTERNAL_IR_READ() ((INTERNAL_IR_PIN & _BV(INTERNAL_IR_BIT)) == 0)
#endif

#define EXTERNAL_IR_INIT() {EXTERNAL_IR_PORT |= _BV(EXTERNAL_IR_BIT);} // activate internal pull up
#ifdef EXTERNAL_IR_ACTIVE_HIGH
#define EXTERNAL_IR_READ() (EXTERNAL_IR_PIN & _BV(EXTERNAL_IR_BIT))
#elif defined EXTERNAL_IR_ACTIVE_LOW
#define EXTERNAL_IR_READ() ((EXTERNAL_IR_PIN & _BV(EXTERNAL_IR_BIT)) == 0)
#endif

#ifdef IR_OUTPUT_USE_TMR1
#define IR_OUTPUT_ON() avr_tmr1_pwm_on()
#define IR_OUTPUT_OFF() avr_tmr1_pwm_off()
#elif defined IR_OUTPUT_USE_TMR0
#define IR_OUTPUT_ON() avr_tmr0_pwm_on()
#define IR_OUTPUT_OFF() avr_tmr0_pwm_off()
#else
#error
#endif


#ifdef BEEP_USE_TMR0
#define BEEP_ON(tone) avr_tmr0_pwm_on(tone)
#define BEEP_OFF() avr_tmr0_pwm_off()
#elif defined BEEP_USE_TMR1
#define BEEP_ON(tone) avr_tmr1_pwm_on(tone)
#define BEEP_OFF() avr_tmr1_pwm_off()
#else
#error
#endif




#define RADIO_CE_ENABLE() RADIO_CE_DDR |= _BV(RADIO_CE_BIT);
#define RADIO_CE_DISABLE() RADIO_CE_DDR &= ~(_BV(RADIO_CE_BIT));
#ifdef RADIO_CE_ACTIVE_HIGH
#define RADIO_CE_ON() RADIO_CE_PORT |= _BV(RADIO_CE_BIT);
#define RADIO_CE_OFF() RADIO_CE_PORT &= ~_BV(RADIO_CE_BIT);
#elif defined RADIO_CE_ACTIVE_LOW
#define RADIO_CE_ON() RADIO_CE_PORT &= ~_BV(RADIO_CE_BIT);
#define RADIO_CE_OFF() RADIO_CE_PORT |= _BV(RADIO_CE_BIT);
#endif

#define RADIO_CSN_ENABLE() RADIO_CSN_DDR |= _BV(RADIO_CSN_BIT);
#define RADIO_CSN_DISABLE() RADIO_CSN_DDR &= ~(_BV(RADIO_CSN_BIT));
#ifdef RADIO_CSN_ACTIVE_HIGH
#define RADIO_CSN_ON() RADIO_CSN_PORT |= _BV(RADIO_CSN_BIT);
#define RADIO_CSN_OFF() RADIO_CSN_PORT &= ~_BV(RADIO_CSN_BIT);
#elif defined RADIO_CSN_ACTIVE_LOW
#define RADIO_CSN_ON() RADIO_CSN_PORT &= ~_BV(RADIO_CSN_BIT);
#define RADIO_CSN_OFF() RADIO_CSN_PORT |= _BV(RADIO_CSN_BIT);
#endif

#define RADIO_IRQ_INIT() {RADIO_IRQ_PORT |= _BV(RADIO_IRQ_BIT);} // activate internal pull up
#ifdef RADIO_IRQ_ACTIVE_HIGH
#define RADIO_IRQ_READ() (RADIO_IRQ_PORT_PIN & _BV(RADIO_IRQ_BIT))
#elif defined RADIO_IRQ_ACTIVE_LOW
#define RADIO_IRQ_READ() ((RADIO_IRQ_PORT_PIN & _BV(RADIO_IRQ_BIT)) == 0)
#endif


#endif

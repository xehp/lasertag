/*
avr_adc.c 

provide functions to set up hardware

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

2005-07-02 Created. Henrik Bjorkman

*/

// When reading 16bit reg remember to read low byte first then high byte
// when writing write high byte first then low byte but hopefully the C compiler does this.


#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdint.h>
#include "avr_cfg.h"
#include "avr_sys.h"
#include "avr_adc.h"

// List analog channels that shall be sampled here.
// Same channel can be listed more than once if it is needed more often than others.
// Or for noise suppression take 3 values so that the median of 3 can be used.
static const uint8_t sampling_sequence[] = { 0, 0, 0 };

#define AVR_ADC_N_SAMPLES_TO_TAKE (sizeof(sampling_sequence))

static volatile uint16_t result[AVR_ADC_N_SAMPLES_TO_TAKE]; // Store results for each sample in this array.
static volatile uint8_t avr_adc_counter=0xff;





static inline void AVR_ADC_setChannelAndStartConversion(uint8_t channel) 
{
  // Set the ADMUX to channel
  ADMUX&=(uint8_t)~(0xf<<MUX0);
  ADMUX|=(uint8_t)((channel&0xf)<<MUX0);

  // Start conversion
  ADCSRA|=(1<<ADSC);  
}


// This is called when a conversion is completed.
SIGNAL(ADC_vect) // old name SIG_ADC
{
  uint16_t s;

  // get result from ADC
  // ADCL must be read first and then ADCH
  s=ADCL;
  s|=(ADCH<<8);

  // The result is now in variable s.

  // store result
  result[avr_adc_counter] = s;

  ++avr_adc_counter;

  if (avr_adc_counter < AVR_ADC_N_SAMPLES_TO_TAKE)
  {
    // Start sampling next channel.

    AVR_ADC_setChannelAndStartConversion(sampling_sequence[avr_adc_counter]);    
  }
  else
  {
    // All samples have been converted.

    // disable adc compleate interrupt
    ADCSRA&=~(1<<ADIE);
  }
}

void AVR_ADC_init(void)
{
  // Disable digital input buffers to save power.
  //DIDR0=0x3f;


  // ADMUX: REFS1, REFS0, ADLAR, -, MUX3, MUX2, MUX1, MUX0
  // REFS1:0 = 3 means use internal bandgap reference
  //ADMUX=(3<<REFS0);
  // REFS1:0 = 1 means use Vcc reference
  ADMUX=(1<<REFS0);


  // TODO Use builtin reference instead of Vcc reference. Change R18 in HW from 10 to 22 KOhm.
  
  // ADCSRA: ADEN, ADSC, ADFR, ADIF, ADIE, ADPS2, ADPS1, ADPS0
  // ADEN: 1 = enable ADC (The user is advised to write zero to ADEN before entering some sleep modes to avoid excessive power consumption.)
  // ADPS2:0 = 7 we assume 16MHz OSC so we need to divide with 128
  #if AVR_FOSC>25600000L
  #error AVR_FOSC to high fo ADC
  #elif AVR_FOSC>=12800000L
  ADCSRA=(1<<ADEN) | (7<<ADPS0);
  #elif AVR_FOSC>=6400000L
  ADCSRA=(1<<ADEN) | (6<<ADPS0);
  #elif AVR_FOSC>=3200000L
  ADCSRA=(1<<ADEN) | (5<<ADPS0);
  #elif AVR_FOSC>=1600000L
  ADCSRA=(1<<ADEN) | (4<<ADPS0);
  #elif AVR_FOSC>=800000L
  ADCSRA=(1<<ADEN) | (3<<ADPS0);
  #elif AVR_FOSC>=400000L
  ADCSRA=(1<<ADEN) | (2<<ADPS0);
  #elif AVR_FOSC>=200000L
  ADCSRA=(1<<ADEN) | (1<<ADPS0);
  #elif AVR_FOSC>=50000L
  ADCSRA=(1<<ADEN) | (0<<ADPS0);
  #else
  #error AVR_FOSC to low for ADC
  #endif

  // TODO For analog input pins, the digital input buffer should be disabled at all times.
  // An analog signal level close to VCC/2 on an input pin can cause significant current
  // even in active mode. Digital input buffers can be disabled by writing to the Digital
  // Input Disable Registers (DIDR1 and DIDR0).
  // Refer to ”DIDR1 – Digital Input Disable Register 1” on page 245
  // and ”DIDR0 – Digital Input Disable Register 0” on page 260 for details.
}



// TODO Perhaps we should take desired sampling sequence as parameter?
void AVR_ADC_startSampling(void)
{
    avr_adc_counter=0;


    // enable adc compleate interrupt
    ADCSRA|=(1<<ADIE);

    // Start the conversion
    AVR_ADC_setChannelAndStartConversion(sampling_sequence[avr_adc_counter]);
}


// Returns true if the requested number of samples are ready.
int8_t AVR_ADC_isready(void) 
{
	return (avr_adc_counter==AVR_ADC_N_SAMPLES_TO_TAKE);
}


uint16_t AVR_ADC_getSample(uint8_t index)
{
	ASSERT(index < AVR_ADC_N_SAMPLES_TO_TAKE);
	return result[index];
} 
 



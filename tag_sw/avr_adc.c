/*
avr_adc.c 

Provide functions to use analog to digital converter.

Copyright (C) 2021 Henrik Bjorkman www.eit.se/hb.

This file is free software; you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License version 2.1.

Removing this comment or the history section is not allowed. Even if only
a few lines from this file is actually used. If you modify this code make
a note about it in the history section below. That is required!

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

History

2005-07-02 Created. Henrik Bjorkman

*/

/*
When reading 16bit reg remember to read low byte first then high byte
when writing write high byte first then low byte but hopefully the C compiler does this.

References:
[1] https://ww1.microchip.com/downloads/en/DeviceDoc/ATmega48A-PA-88A-PA-168A-PA-328-P-DS-DS40002061B.pdf
*/


#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdint.h>
#include "avr_cfg.h"
#include "avr_sys.h"
#include "avr_adc.h"

#include "errlog.h"

// List analog channels that shall be sampled here.
// Same channel can be listed more than once if it is needed more often than others.
// Or for noise suppression do 3 values and take the median value.
// Remember to update setting to DIDR0 (digital input buffers) if changing here.
#if HW_VERSION < 3
static const uint8_t sampling_sequence[] = { 0, 0, 0 };
#else
static const uint8_t sampling_sequence[] = { 7, 7, 7 };
#endif

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
  // Ref [1] Chapter 23.9.5 "DIDR0 – Digital Input Disable Register 0"
  // Disable digital input buffers on pins used as analog input to save power.
  // This setting need to match that in sampling_sequence above, if this is
  // changed that need to be changed also.
  DIDR0=0x01;

  // Ref [1] Chapter 24.9.1 "ADMUX – ADC Multiplexer Selection Register"
  // ADMUX: REFS1, REFS0, ADLAR, -, MUX3, MUX2, MUX1, MUX0
  // REFS1:0 = 3 means use internal bandgap reference
  // REFS[1:0] shall be 0x3
  ADMUX=(3<<REFS0);
  // REFS1:0 = 1 means use Vcc reference
  //ADMUX=(1<<REFS0);
  
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
}



// TODO Perhaps we should take desired sampling sequence as parameter?
void AVR_ADC_startSampling(void)
{
    avr_adc_counter=0;


    // enable ADC complete interrupt
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
 



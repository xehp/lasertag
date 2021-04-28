/*
avr_adc.h

provide functions to use analog to digital converter.

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

2005-07-02 Created. /Henrik
*/


#ifndef AVR_ADC_H
#define AVR_ADC_H

#include <stdint.h>

// set up hardware (port directions, registers etc.)
void AVR_ADC_init(void);

void AVR_ADC_startSampling(void);

void AVR_ADC_tick(void);

int8_t AVR_ADC_isready(void);

uint16_t AVR_ADC_getSample(uint8_t index);

#endif

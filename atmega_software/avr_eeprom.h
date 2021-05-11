/*
avr_eeprom.c

Handle stored parameters

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

2017-06-04 Created reusing code from other projects. /Henrik
*/

#ifndef EEPROM_H_
#define EEPROM_H_


#include <stdint.h>




// A magic number so we can detect if expected format of stored eedata has been changed (don't use zero).
#define EEDATA_MAGIC_NR 0x0121


typedef struct
{
	uint32_t magicNumber;
	uint32_t ID;
	uint32_t device_type;
	uint32_t player_number;
	uint32_t spare4; // team?
	uint32_t spare5;
	uint32_t spare6;
	uint32_t spare7;
	uint32_t spare8;
	uint32_t spare9;
	uint32_t spare10;
	uint32_t spare11;
	uint32_t spare12;
	uint32_t spare13;
	uint32_t spare14;
	uint32_t checkSum;
}
EeDataStruct;


extern EeDataStruct ee;


void eepromSave(void);
void eepromLoad(void);

#endif


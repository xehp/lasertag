/*
avr_eeprom.h

Handle stored parameters

Copyright (C) 2017 EIT European Infotech AB www.eit.se.

History

2017-06-04
Created.
Henrik Bjorkman

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
	uint32_t spare2;
	uint32_t spare3;
	uint32_t spare4;
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


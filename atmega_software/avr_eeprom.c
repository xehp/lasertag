/*
avr_eeprom.c

Handle stored parameters

Copyright (C) 2017 EIT European Infotech AB www.eit.se.

History

2017-06-04
Created.
Henrik Bjorkman

*/

// http://www.nongnu.org/avr-libc/user-manual/group__avr__eeprom.html#gac5c2be42eb170be7a26fe8b7cce4bc4d
// http://www.avrfreaks.net/forum/tut-c-using-eeprom-memory-avr-gcc?page=all
// http://www.nongnu.org/avr-libc/user-manual/group__avr__eeprom.html


#include <stdint.h>

#include <avr/eeprom.h>
#include "avr_uart.h"


#include "avr_eeprom.h"

// Where in EEPROM the backup message shall be written.
#define BACKUP_EEPROM_OFFSET 512




EeDataStruct ee={
	EEDATA_MAGIC_NR, // magicNumber
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0
};



// Very simple checksum
static uint32_t calcCSum(const char *ptr, uint16_t size)
{
	uint32_t s=0;
	uint8_t i;
	for (i=0; i<size; ++i)
	{
		s = v*31 + *ptr;
        ptr++;
	}
	return s;
}



static int8_t saveBytePacker(EeDataStruct* eedataStruct, uint16_t offset)
{
	eeprom_write_block(eedataStruct, (void*)offset, sizeof(EeDataStruct));
	return  0;
}

static int8_t loadBytePacker(EeDataStruct* eedataStruct, uint16_t offset)
{
	eeprom_read_block((void *)eedataStruct, (const void *)offset, sizeof(EeDataStruct));
	return 0;
}



void eepromSave()
{
	// Set checksum to zero while calculating it.
	ee.checkSum = 0;
    const uint32_t calculatedCSum = calcCSum((const unsigned char *)&ee, sizeof(EeDataStruct));
	ee.checkSum = calculatedCSum;

	saveBytePacker(&ee, 0);
	saveBytePacker(&ee, BACKUP_EEPROM_OFFSET);
}




// Returns 0 if OK
static int8_t eepromTryLoad(uint16_t offset)
{
	EeDataStruct tmp;


	const int8_t r = loadBytePacker(&tmp, offset);

	// save checksum
	const uint32_t csumLoaded = tmp.checkSum;

	// Set checksum to zero while calculating it. Since that is what we did when saving
	tmp.checkSum = 0;
    const uint32_t csumCalculated = calcCSum((const unsigned char *)&tmp, sizeof(EeDataStruct));

	if(tmp.magicNumber != ee.magicNumber)
	{
		logInt3(EEPROM_WRONG_MAGIC_NUMBER, tmp.magicNumber, ee.magicNumber);
    	return -1;
	}

	if(csumCalculated != csumLoaded)
	{
		logInt3(EEPROM_WRONG_CHECKSUM, csumCalculated, csumLoaded);
    	return -1;
	}

	ee = tmp;

	logInt2(EEPROM_LOADED_OK, ee.magicNumber);

	return r;
}


// If EEProm is available we want to set wantedCurrent and/or wantedVoltage
// We do that by reading EEPROM as if those where commands an pass these commands to
void eepromLoad(void)
{
	int8_t r = eepromTryLoad(0);

	if (r!=0)
	{

		logInt1(EEPROM_TRYING_BACKUP);

		r = eepromTryLoad(BACKUP_EEPROM_OFFSET);
		if (r==0)
		{
			logInt1(EEPROM_BACKUP_LOADED_OK);
		}
		else
		{
			logInt1(EEPROM_FAILED);
		}
	}
}





/*
avr_spi.c

Provide functions to use SPI.

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

2021-04-22 Created. /Henrik
*/

/*
References:
[1] https://ww1.microchip.com/downloads/en/DeviceDoc/ATmega48A-PA-88A-PA-168A-PA-328-P-DS-DS40002061B.pdf
*/

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdint.h>
#include "avr_cfg.h"
#include "avr_sys.h"
#include "avr_uart.h"
#include "avr_ports.h"
#include "avr_spi.h"


// Returns in data.
uint8_t avr_spi_transfer(uint8_t out_data)
{
	AVR_DELAY_US(11);
	SPDR = out_data;
	while(!(SPSR & (1<<SPIF)))
	{
	    // waiting
	}
	return SPDR;
}

void avr_spi_init(void)
{
	UART_PRINT_P("avr_spi_init\r\n");

	// Ref [1] 19.SPI – Serial Peripheral Interface
    // Our device is OK up to 8 Mz, Ref [2] "Features" "4-pin SPI interface with maximum 8 MHzclock rate"

	// MOSI -> PB3, MISO -> PB4, SCK -> PB5
	// SS? PB2 was needed for OC1B in our case so not available here. Should be OK to have it as output
	// for something else, Ref [2] 19.3.2 Master Mode "If SS is configured as an output, the pin is a general output pin which does not affect the SPI system."
	// For CE, CSN, IRQ used see avr_cfg.h.

	/* Set MOSI and SCK output, all others input */
	// Ref [1] Chapter 19.2 "DDR_SPI in the examples must be replaced by the actual..."
	//DDR_SPI = (1<<DD_MOSI)|(1<<DD_SCK);
	DDRB = (1<<DDB2) | (1<<DDB3) | (1<<DDB5);

	/* Enable SPI, Master, set clock rate fck/16 */
	// TODO Use fck/4
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);
}


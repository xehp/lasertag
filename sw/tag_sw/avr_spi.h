/*
avr_spi.h

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

2021-04-20 Created. /Henrik
*/

#ifndef AVR_SPI_H
#define AVR_SPI_H

void avr_spi_init(void);
uint8_t avr_spi_transfer(uint8_t out_data);

#endif

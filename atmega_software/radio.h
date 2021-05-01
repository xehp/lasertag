/*
radio.h

provide functions to use a radio transceiver.

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

#ifndef RADIO_H
#define RADIO_H


#define RADIO_PAYLOAD_SIZE 5

#define RADIO_FIFO_SIZE 8

struct Radio_fifo_entry
{
	uint8_t data[RADIO_PAYLOAD_SIZE];
};

struct Radio_fifo
{
  uint8_t head;
  uint8_t tail;
  struct Radio_fifo_entry buffer[RADIO_FIFO_SIZE];
};


#define RADIO_FIFO_INC(index) ((index+1)&(RADIO_FIFO_SIZE-1))

void radio_fifo_init(struct Radio_fifo *fifoPtr);
void radio_fifo_put(struct Radio_fifo *fifoPtr, const uint8_t *data, uint8_t data_len);
int8_t radio_fifo_is_full(struct Radio_fifo *fifoPtr);
int8_t radio_fifo_is_empty(struct Radio_fifo *fifoPtr);
const uint8_t radio_fifo_take(struct Radio_fifo *fifoPtr, uint8_t *buf_ptr, uint8_t buf_size);


/**
 * Returns number of bytes received.
 * buf_size must be same as RADIO_PAYLOAD_SIZE
 */
uint8_t radio_receive_data(uint8_t *buf_ptr, uint8_t buf_size);
int8_t radio_transmit_data(const uint8_t *buf_ptr, uint8_t buf_size);

void radio_init(void);
void radio_process(void);

#endif

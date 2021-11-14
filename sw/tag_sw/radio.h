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

// Max payload for the RFM75 is 32 bytes.
// Here we decide how much of that we actually use.
#define RADIO_PAYLOAD_MAX_LEN 16

// If fixed size is used it shall be same as RADIO_PAYLOAD_MAX_LEN
#define RADIO_PAYLOAD_FIXED_SIZE RADIO_PAYLOAD_MAX_LEN


// The address we use is XORed with a magic number (to ignore
// messages not concerning us). So we need a random number.
// https://www.random.org/cgi-bin/randbyte?nbytes=5&format=h
// got: 9c 33 d3 9e bc
// This is then XORed with the 32 bit addresses.
//#define RADIO_MAGIC_ADDR {0x9c, 0x33, 0xd3, 0x9e, 0xbc}
//#define RADIO_MAGIC_ADDR64 0x9C33D39EBCLL
// Wanted broadcast address FFFFFFFF top be encoded as it was
// so inverted lower 4 bytes in RADIO_MAGIC_ADDR64
#define RADIO_MAGIC_ADDR64 0x9ccc2c6143LL

#define RADIO_BROADCAST_ADDRESS 0xFFFFFFFFLL

enum
{
	radio_pipe_empty = -1,
	radio_broadcast_pipe=0,
	radio_specific_pipe=1
};


struct radio_data_packer
{
	uint8_t buffer[RADIO_PAYLOAD_MAX_LEN];
	struct utility_w_struct uws;
};

//void radio_add_generic_message_hdr(struct utility_w_struct *uws, uint8_t msg_cat, uint8_t sub_cmd);
//void radio_add_getset_message_hdr(struct utility_w_struct *uws, uint8_t msg_cat, uint8_t sub_cmd, uint8_t par);

/**
 * Returns number of bytes received.
 * buf_size must be same as RADIO_PAYLOAD_SIZE
 */
int8_t radio_receive_take_data(uint8_t *buf_ptr, uint8_t buf_size);

int8_t radio_transmit_put_data(const uint8_t *buf_ptr, uint8_t buf_size, int32_t addr);

uint8_t radio_fifo_get_free_space_in_tx_queue(void);

void radio_init(void);
void radio_process(void);
void radio_tick(void);

int8_t radio_take_sequence_number(void);

void radio_data_packer_init(struct radio_data_packer *packer, uint8_t msg_cat, uint8_t sub_cmd, uint8_t seq_nr);
void radio_data_packer_put16(struct radio_data_packer *packer, uint8_t nof_bits_to_write, uint16_t d);
void radio_data_packer_put64(struct radio_data_packer *packer, uint8_t nof_bits_to_write, uint64_t d);

int8_t radio_state(void);

#endif

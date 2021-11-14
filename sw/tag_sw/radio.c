/*
radio.c

provide functions to use a radio transceiver.

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

2021-04-22 Created. /Henrik
*/

/*
References:
[1] https://ww1.microchip.com/downloads/en/DeviceDoc/ATmega48A-PA-88A-PA-168A-PA-328-P-DS-DS40002061B.pdf
[2] https://www.hoperf.com/data/upload/portal/20181122/5bf67582882a9.pdf
*/

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdint.h>
#include "avr_cfg.h"
#include "avr_sys.h"
#include "avr_uart.h"
#include "avr_ports.h"
#include "avr_spi.h"
#include "avr_eeprom.h"
#include "utility.h"
#include "msg.h"
#include "radio.h"
#include "errlog.h"


// Extra transmit of every message
// Send messages one (or more) time extra.
// Number of extra sending (retransmitting) of the message.
// 0 for no extra, 1 for one extra (that is message is sent twice)
// Use zero to not send any duplicates.
#define NOF_EXTRA_TRANSMIT 1


// Ref [2] 6.3.1 SPI Command table 3
#define RFM75_R_REGISTER 0
#define RFM75_W_REGISTER 0x20
#define RFM75_R_RX_PAYLOAD 0x61
#define RFM75_W_TX_PAYLOAD 0xA0
#define RFM75_FLUSH_TX 0xE1
#define RFM75_FLUSH_RX 0xE2
#define RFM75_REUSE_TX_PL 0xE3
#define RFM75_ACTIVATE 0x50
#define RFM75_R_RX_PL_WID 0x60
#define RFM75_W_ACK_PAYLOAD 0xA0
#define RFM75_W_TX_PAYLOAD_NO_ACK 0xB0
#define RFM75_NOP 0xFF

// Registers
#define RFM75_CONFIG 0x00
#define RFM75_EN_AA 0x01
#define RFM75_EN_RXADDR 0x02
#define RFM75_SETUP_RETR 0x04
#define RFM75_RF_CH 0x05
#define RFM75_STATUS 0x07
#define RFM75_RX_ADDR_P0 0x0a
#define RFM75_RX_ADDR_P1 0x0b
#define RFM75_TX_ADDR 0x10
#define RFM75_RX_PW_P0 0x11
#define RFM75_RX_PW_P1 0x12
#define RFM75_RX_PW_P2 0x13
#define RFM75_RX_PW_P3 0x14
#define RFM75_RX_PW_P4 0x15
#define RFM75_RX_PW_P5 0x16
#define RFM75_FIFO_STATUS 0x17
#define RFM75_DYNPD 0x1c
#define RFM75_FEATURE 0x1d

// Bit masks for registers.
#define RFM75_CONFIG_EN_CRC_MASK 0x08
#define RFM75_CONFIG_CRCO_MASK 0x04
#define RFM75_CONFIG_PWR_UP_MASK 0x02
#define RFM75_STATUS_RX_DR_MASK 0x40
#define RFM75_STATUS_TX_DR_MASK 0x20
#define RFM75_STATUS_MAX_RT_MASK 0x10
#define RFM75_PRIM_RX_MASK 0x01
#define RFM75_EN_RXADDR_ERX_P0_MASK 1
#define RFM75_EN_RXADDR_ERX_P1_MASK 2
#define RFM75_RX_EMPTY_MASK 1
#define RFM75_EN_DPL_MASK 4
#define RFM75_EN_DYN_ACK_MASK 1
#define RFM75_RFM75_DPL_P0_MASK 1
#define RFM75_RX_P_NO_MASK 0xe
#define RFM75_ENAA_P0_MASK 1
#define RFM75_DPL_P0_MASK 1
#define RFM75_DPL_P1_MASK 2
#define RFM75_RF_CH_DEFAULT 2


#define radio_is_data_received() (status & RFM75_STATUS_RX_DR_MASK)
#define radio_is_max_retrans() (status & RFM75_STATUS_MAX_RT_MASK)
#define radio_is_data_sent() (status & RFM75_STATUS_TX_DR_MASK)
#define radio_get_rx_p_no() ((status & RFM75_RX_P_NO_MASK) >> 1)

enum
{
		radio_initial_state=0,
		radio_detecting_state,
		radio_delay_after_detect_state,
		radio_send_hello_state,
		radio_check_result_after_sending_state,
		radio_delay_after_sending,
		radio_check_result_after_send_state,
		radio_send_data_state,
		radio_fail_state
};

enum
{
	cleanup_idle=0,
	cleanup_active=1
};


static uint8_t state = 0;
static int16_t timer_ms = 0;
static int16_t counter = 0;
static uint8_t status = 0;
static uint8_t log_state = 0;
static uint8_t log_status = 0;
static uint8_t logged_rx_pw_p0 = 0;
static uint8_t logged_rx_p_no = 0;
static int8_t nof_msg_received = 0;
//static int8_t nof_msg_received_sec = 0;
static int8_t log_counter = -1;
static int8_t radio_detected = 0;
static int8_t cleanup_state = 0;
static int8_t sequence_number = 0;

struct Radio_rx_fifo_entry
{
	int8_t pipe;
	uint8_t data[RADIO_PAYLOAD_MAX_LEN];
	#ifndef RADIO_PAYLOAD_FIXED_SIZE
	uint8_t length;
	#endif
};

struct Radio_tx_fifo_entry
{
	int32_t addr;
	uint8_t data[RADIO_PAYLOAD_MAX_LEN];
	#ifndef RADIO_PAYLOAD_FIXED_SIZE
	uint8_t length;
	#endif
	uint8_t send_count;
};


// Perhaps 4 is enough, change if we run out of RAM.
#define RADIO_TX_FIFO_SIZE 8
#define RADIO_TX_FIFO_MASK (RADIO_TX_FIFO_SIZE-1)
#define RADIO_TX_FIFO_INC(index) ((index+1)&(RADIO_TX_FIFO_MASK))


// In this fifo items are added at head and removed at tail.
struct tx_fifo_struct
{
	uint8_t head;
	uint8_t tail;
	struct Radio_tx_fifo_entry buffer[RADIO_TX_FIFO_SIZE];
} tx_fifo;

static void radio_tx_fifo_init(void)
{
	tx_fifo.head = 0;
	tx_fifo.tail = 0;
	#ifndef RADIO_PAYLOAD_FIXED_SIZE
	for (uint8_t i=0; i<RADIO_TX_FIFO_SIZE; i++)
	{
		tx_fifo.buffer[i].length=0;
	}
	#endif
}

static int8_t radio_tx_fifo_is_full(void)
{
  return (((uint8_t)RADIO_TX_FIFO_INC(tx_fifo.head)) == tx_fifo.tail);
}

static int8_t radio_tx_fifo_is_empty(void)
{
  return (tx_fifo.head == tx_fifo.tail);
}

// Returns 0 if FIFO is empty. Non zero if not empty.
static const uint8_t radio_tx_fifo_take(struct Radio_tx_fifo_entry *tx_data)
{
	if (radio_tx_fifo_is_empty())
	{
		return 0;
	}

	*tx_data = tx_fifo.buffer[tx_fifo.tail];

	if (tx_fifo.buffer[tx_fifo.tail].send_count>0)
	{
		tx_fifo.buffer[tx_fifo.tail].send_count--;
	}
	else
	{
		tx_fifo.tail = RADIO_TX_FIFO_INC(tx_fifo.tail);
	}

	return 1;
}

static void radio_tx_fifo_delete_oldest_if_full(void)
{
	if (radio_tx_fifo_is_full())
	{
		// Throw away oldest message in queue.
		tx_fifo.tail = RADIO_TX_FIFO_INC(tx_fifo.tail);
	}
}

// send_count : Number of extra sending (retransmitting) of the message.
// 0 for no extra, 1 for one extra (that is message is sent twice)
static void radio_tx_fifo_put(const uint8_t *data, uint8_t data_len, uint8_t send_count, uint32_t addr)
{
	radio_tx_fifo_delete_oldest_if_full();

	ASSERT(data_len <= RADIO_PAYLOAD_MAX_LEN);
	utility_memcpy(tx_fifo.buffer[tx_fifo.head].data, data, data_len);
	#ifndef RADIO_PAYLOAD_FIXED_SIZE
	tx_fifo.buffer[tx_fifo.head].length=data_len;
	#endif
	tx_fifo.buffer[tx_fifo.head].send_count = send_count;
	tx_fifo.buffer[tx_fifo.head].addr = addr;

	tx_fifo.head = RADIO_TX_FIFO_INC(tx_fifo.head);
}


static uint8_t radio_tx_fifo_get_entries_in_queue(void)
{
    return ( tx_fifo.tail - tx_fifo.head ) & RADIO_TX_FIFO_MASK;
}

static uint8_t radio_tx_fifo_get_free_space_in_queue(void)
{
	return((RADIO_TX_FIFO_SIZE-1)-radio_tx_fifo_get_entries_in_queue());
}

// Preferably this is same or bigger than transmit buffer.
#define RADIO_RX_FIFO_SIZE 16
#define RADIO_RX_FIFO_MASK (RADIO_RX_FIFO_SIZE-1)
#define RADIO_RX_FIFO_INC(index) ((index+1)&(RADIO_RX_FIFO_MASK))


// In this fifo items are added at head and removed at tail.
struct rx_fifo_struct
{
	uint8_t head;
	uint8_t tail;
	uint8_t recent_messages_tail;
	struct Radio_rx_fifo_entry buffer[RADIO_RX_FIFO_SIZE];
} rx_fifo;

static void radio_rx_fifo_init(void)
{
	rx_fifo.head = 0;
	rx_fifo.tail = 0;
	rx_fifo.recent_messages_tail = 0;
	#ifndef RADIO_PAYLOAD_FIXED_SIZE
	for (uint8_t i=0; i<RADIO_RX_FIFO_SIZE; i++)
	{
		rx_fifo.buffer[i].length=0;
	}
	#endif
}

static int8_t radio_rx_fifo_is_full(void)
{
  return (((uint8_t)RADIO_RX_FIFO_INC(rx_fifo.head)) == rx_fifo.tail);
}

static int8_t radio_rx_fifo_is_empty(void)
{
  return (rx_fifo.head == rx_fifo.tail);
}

// Returns zero if there was no message, non zero if there was a message.
// TODO Shall we provide pipe also? So that caller can know if it was a broadcast message or not.
static const int8_t radio_rx_fifo_take(uint8_t *buf_ptr, uint8_t buf_size)
{
	if (radio_rx_fifo_is_empty())
	{
		return radio_pipe_empty;
	}

	#ifndef RADIO_PAYLOAD_FIXED_SIZE
	const uint8_t len = rx_fifo.buffer[rx_fifo.tail].length;
	ASSERT(buf_size >= len);
	utility_memcpy(buf_ptr, rx_fifo.buffer[rx_fifo.tail].data, len);
	#else
	utility_memcpy(buf_ptr, rx_fifo.buffer[rx_fifo.tail].data, buf_size);
	#endif
	const int8_t pipe = rx_fifo.buffer[rx_fifo.tail].pipe;

	rx_fifo.tail = RADIO_RX_FIFO_INC(rx_fifo.tail);

	#ifndef RADIO_PAYLOAD_FIXED_SIZE
	// TODO return the message length in a paramter.
	#endif

	return pipe;
}

static void radio_rx_fifo_delete_one_if_full(void)
{
	if (radio_rx_fifo_is_full())
	{
		// Throw away oldest message in queue.
		rx_fifo.tail = RADIO_RX_FIFO_INC(rx_fifo.tail);
	}
}

static void radio_rx_fifo_put(const struct Radio_rx_fifo_entry *rx_data)
{
	radio_rx_fifo_delete_one_if_full();

	rx_fifo.buffer[rx_fifo.head] = *rx_data;
	rx_fifo.head = RADIO_RX_FIFO_INC(rx_fifo.head);
}


// Old messages are used for detecting retransmissions (duplicates).

static int8_t radio_rx_fifo_is_recent_messages_empty(void)
{
  return (rx_fifo.head == rx_fifo.recent_messages_tail);
}

// The oldest of old messages is deleted.
// Do not call this if there is no old message!
// See also radio_fifo_delete_old_if_any.
static void radio_rx_fifo_delete_oldest_of_recent(void)
{
	rx_fifo.recent_messages_tail = RADIO_RX_FIFO_INC(rx_fifo.recent_messages_tail);
}

static void radio_rx_fifo_delete_one_from_recent_if_any(void)
{
	if (!radio_rx_fifo_is_recent_messages_empty())
	{
		radio_rx_fifo_delete_oldest_of_recent();
	}
}

// This will look in the log of old messages to see if the messages has been seen before.
// If its a duplicate then we will regard it as a retransmission and discard it.
static uint8_t radio_rx_fifo_is_data_seen_recently(const struct Radio_rx_fifo_entry *rx_data)
{
	uint8_t f=0;
	uint8_t recent = rx_fifo.recent_messages_tail;

	while(recent != rx_fifo.head)
	{
		#ifndef RADIO_PAYLOAD_FIXED_SIZE
		if (rx_data->length == rx_fifo.buffer[recent].length)
		{
			if (utility_memcmp((const int8_t*)rx_data->data, (const int8_t*)rx_fifo.buffer[recent].data, rx_data->length) == 0)
			{
				f++;
			}
		}
		#else
		if (utility_memcmp((const int8_t*)rx_data->data, (const int8_t*)rx_fifo.buffer[recent].data, RADIO_PAYLOAD_FIXED_SIZE) == 0)
		{
			f++;
		}
		#endif
		recent = RADIO_RX_FIFO_INC(recent);
	}
	return f;
}




static void log_status_change(void)
{
	#if 0
	uint8_t irq = RADIO_IRQ_READ();
	if (irq != logged_irq)
	{
		UART_PRINT_P("r irq ");
		uart_print_hex4(irq);
		uart_print_crlf();
		logged_irq = irq;
	}
	if (status != log_status)
	{
		UART_PRINT_P("r status ");
		uart_print_hex8(status);
		uart_print_crlf();
		log_status = status;
	}
	#else
	AVR_DELAY_US(10);
	#endif
}

static void radio_write_command(uint8_t cmd)
{
	RADIO_CSN_ON();
	status = avr_spi_transfer(cmd);
	RADIO_CSN_OFF();
	log_status_change();
}

static void radio_flush_tx(void)
{
	RADIO_CSN_ON();
	status = avr_spi_transfer(RFM75_FLUSH_TX);
	RADIO_CSN_OFF();
	log_status_change();
}

static uint8_t radio_read_register(uint8_t reg_id)
{
	RADIO_CSN_ON();
	status = avr_spi_transfer(RFM75_R_REGISTER | reg_id);
	const uint8_t r2 = avr_spi_transfer(0);
	RADIO_CSN_OFF();
	log_status_change();
	return r2;
}

static uint8_t radio_write_register(uint8_t reg_id, uint8_t data)
{
	RADIO_CSN_ON();
	status = avr_spi_transfer(RFM75_W_REGISTER | reg_id);
	const uint8_t r2 = avr_spi_transfer(data);
	RADIO_CSN_OFF();
	log_status_change();
	return r2;
}

static void radio_read_register_data(uint8_t reg_id, uint8_t *data, uint8_t len)
{
	RADIO_CSN_ON();
	status = avr_spi_transfer(RFM75_R_REGISTER | reg_id);
	for (int8_t i=0; i<len; ++i)
	{
		data[i] = avr_spi_transfer(0);
	}
	RADIO_CSN_OFF();
	log_status_change();
}

void radio_write_register_data(uint8_t reg_id, const uint8_t *data, uint8_t len)
{
	RADIO_CSN_ON();
	status = avr_spi_transfer(RFM75_W_REGISTER | reg_id);
	for (int8_t i=0; i<len; ++i)
	{
		avr_spi_transfer(data[i]);
	}
	RADIO_CSN_OFF();
	log_status_change();
}

static void radio_clear_max_retrans(void)
{
	//const uint8_t tmp = radio_read_register(RFM75_STATUS);
	//radio_write_register(RFM75_STATUS, tmp | RFM75_STATUS_MAX_RT_MASK);

	radio_write_register(RFM75_STATUS, RFM75_STATUS_MAX_RT_MASK);
}

static void radio_clear_data_received(void)
{
	//const uint8_t tmp = radio_read_register(RFM75_STATUS);
	//radio_write_register(RFM75_STATUS, tmp | RFM75_STATUS_RX_DR_MASK);
	radio_write_register(RFM75_STATUS, RFM75_STATUS_RX_DR_MASK);
}

static void radio_clear_data_sent(void)
{
	//const uint8_t tmp = radio_read_register(RFM75_STATUS);
	//radio_write_register(RFM75_STATUS, tmp | RFM75_STATUS_RX_DR_MASK);
	radio_write_register(RFM75_STATUS, RFM75_STATUS_TX_DR_MASK);
}

static void radio_clear_all_flags(void)
{
	radio_write_register(RFM75_STATUS, RFM75_STATUS_RX_DR_MASK | RFM75_STATUS_TX_DR_MASK | RFM75_STATUS_MAX_RT_MASK);
}

static void radio_read_status(void)
{
	radio_write_command(RFM75_NOP);
}

static int8_t read_rx_empty(void)
{
	uint8_t tmp = radio_read_register(RFM75_FIFO_STATUS);
	return tmp & RFM75_RX_EMPTY_MASK;
}


static uint8_t read_nof_bytes(uint8_t receive_pipe_number)
{
	switch(receive_pipe_number)
	{
		default:
		case 0: return radio_read_register(RFM75_RX_PW_P0);
		case 1: return radio_read_register(RFM75_RX_PW_P1);
		case 2: return radio_read_register(RFM75_RX_PW_P2);
		case 3: return radio_read_register(RFM75_RX_PW_P3);
		case 4: return radio_read_register(RFM75_RX_PW_P4);
		case 5: return radio_read_register(RFM75_RX_PW_P5);
	}
}

static void radio_flush_rx(void)
{
	RADIO_CSN_ON();
	status = avr_spi_transfer(RFM75_FLUSH_RX);
	RADIO_CSN_OFF();
	log_status_change();
}

#ifndef RADIO_PAYLOAD_FIXED_SIZE
static void send_activate_command(uint8_t d)
{
	RADIO_CSN_ON();
	status = avr_spi_transfer(RFM75_ACTIVATE);
	avr_spi_transfer(d);
	RADIO_CSN_OFF();
	log_status_change();
}


static uint8_t read_rx_pl_wid(void)
{
	// Get variable message length, see ref [2].
	// The MCU can read the length of the received payload by using the command: R_RX_PL_WID

	// To use this the activate command must be issued first:
	// ACTIVATE This write command  followed by data 0x73
	// Ref [2] Page 13

	// TODO In ref [2] there is no #Databytes given for R_RX_PL_WID.

	RADIO_CSN_ON();
	status = avr_spi_transfer(RFM75_R_RX_PL_WID);
	const uint8_t n= avr_spi_transfer(0);
	RADIO_CSN_OFF();
	log_status_change();

	return n;
}

#endif

// Call this to read data from radio over SPI.
static void radio_read_rx_data(struct Radio_rx_fifo_entry *rx_data)
{
	//UART_PRINT_P("r read rx\r\n");

	// Some logging to help debugging (can be removed later).
	// Get the "pipe" on which the data was received. P0, P1 etc.
	// and how much data there is for this message.
	// TODO If we open more than pipe 0 then we need to save pipe number
	// in the receive fifo.
	const uint8_t pipe_number = radio_get_rx_p_no();
	const uint8_t nof_bytes = read_nof_bytes(pipe_number);
	if ((nof_bytes != logged_rx_pw_p0) /*|| (pipe_number != logged_rx_p_no)*/)
	{
		UART_PRINT_P("r p ");
		uart_print_hex4(pipe_number);
		uart_putchar(' ');
		uart_print_hex8(nof_bytes);
		uart_print_crlf();
		logged_rx_pw_p0 = nof_bytes;
		logged_rx_p_no = pipe_number;
	}

	rx_data->pipe = pipe_number;

	#ifndef RADIO_PAYLOAD_FIXED_SIZE
	// Variable message length, see ref [2].
	// The MCU can read the length of the received payload by using the command: R_RX_PL_WID
	const uint8_t len = read_rx_pl_wid();
	UART_PRINT_P("r w ");
	uart_print_hex8(len);
	uart_print_crlf();
	if (len > RADIO_PAYLOAD_MAX_LEN)
	{
		// To long message. ignore it.
		UART_PRINT_PL("r i");
		radio_clear_data_received();
		radio_flush_rx();
		return;
	}
	#endif

	RADIO_CSN_ON();
	status = avr_spi_transfer(RFM75_R_RX_PAYLOAD);
	#ifndef RADIO_PAYLOAD_FIXED_SIZE
	for (int8_t i=0; i<len; ++i)
	#else
	for (int8_t i=0; i<RADIO_PAYLOAD_FIXED_SIZE; ++i)
	#endif
	{
		rx_data->data[i] = avr_spi_transfer(0);
	}
	RADIO_CSN_OFF();
	log_status_change();
}

/**
XOR the 32 bit address with a 40 bit magic number.

We ignore the specification [2] saying LSB first and
just use it the way it happens to go because it was more
convenient that way.
*/
void radio_make_addr5(uint8_t *addr40, uint32_t addr32)
{
	const uint64_t addr64 = RADIO_MAGIC_ADDR64 ^ addr32;
	struct utility_w_struct uws;
	utility_w_init(&uws, addr40, 40);
	utility_w_put64(&uws, 40, addr64);

	/*UART_PRINT_P("r addr ");
	print_int64(addr32);
	uart_putchar(' ');
	uart_print_hex((uint8_t*)addr40, 40/8);
	uart_print_crlf();*/
}


// Will send a message over SPI to radio module.
void radio_send_data(const struct Radio_tx_fifo_entry *tx_data)
{
	RADIO_CE_OFF();
	AVR_DELAY_US(10);

	radio_write_command(RFM75_FLUSH_TX);
	AVR_DELAY_US(10);

	// Set TX addr.
	uint8_t a5[5];
	radio_make_addr5(a5, tx_data->addr);
	radio_write_register_data(RFM75_TX_ADDR, a5, sizeof(a5));

	RADIO_CSN_ON();

	#ifndef RADIO_PAYLOAD_FIXED_SIZE
	// TODO Do we need to tell how long the message is or will radio understand that
	// from number of bytes we transfer?
	status = avr_spi_transfer(RFM75_W_TX_PAYLOAD_NO_ACK);
	for (int8_t i=0; i<tx_data->length; ++i)
	#else
	status = avr_spi_transfer(RFM75_W_TX_PAYLOAD);
	for (int8_t i=0; i<RADIO_PAYLOAD_FIXED_SIZE; ++i)
	#endif
	{
		avr_spi_transfer(tx_data->data[i]);
	}

	AVR_DELAY_US(10);
	RADIO_CSN_OFF();
	AVR_DELAY_US(10);

	RADIO_CE_ON();

	// Some logging for debugging, can be removed or commented out later.
	//UART_PRINT_P("r tx ");
	//uart_print_hex(data, len);
	//uart_print_crlf();

	AVR_DELAY_US(20);

	RADIO_CE_OFF();

	log_status_change();
}





static uint8_t get_reg_size(uint8_t reg_id)
{
	switch(reg_id)
	{
		case 0x00:
		case 0x01:
		case 0x02:
		case 0x03:
		case 0x04:
		case 0x05:
		case 0x06:
		case 0x07:
		case 0x08:
		case 0x09:
		case 0x11:
		case 0x17:
		case 0x1c:
		case 0x1d:
			return 1;
		case RFM75_RX_ADDR_P0:
		case RFM75_RX_ADDR_P1:
		case RFM75_TX_ADDR:
			return 5;
		default:
			return 0;
	}
}

// Not only logging, we check if radio is present here.
static int8_t radio_log_register(uint8_t reg_id)
{
	uint8_t tmp[5];
	uint8_t len = get_reg_size(reg_id);
	if (len > 0)
	{
		radio_read_register_data(reg_id, tmp, len);
		UART_PRINT_P("r reg ");
		uart_print_hex8(reg_id);
		uart_putchar(' ');
		for(uint8_t i = 0; i<len; i++)
		{
			uart_print_hex8(tmp[i]);
		}
		uart_print_crlf();
	}

	return tmp[0];
}



static void radio_tx_mode(void)
{
	//UART_PRINT_PL("r tx mode");

	// Set CE ON only later when it is time to send (see radio_send_data).
    RADIO_CE_OFF();

    radio_flush_tx();
    radio_clear_all_flags();

    //radio_write_one_byte_register(RFM75_TX_PW, PAYLOAD_SIZE);
    radio_write_register(RFM75_CONFIG, RFM75_CONFIG_EN_CRC_MASK + RFM75_CONFIG_CRCO_MASK + RFM75_CONFIG_PWR_UP_MASK);
	log_status_change();
}

void radio_rx_mode(void)
{
	//UART_PRINT_PL("r rx mode\r\n");

	radio_clear_data_received();
	radio_flush_rx();

	// TODO This should only be needed to be written once, in radio_set_addr_and_channel
	/*#ifdef RADIO_PAYLOAD_FIXED_SIZE
    radio_write_register(RFM75_RX_PW_P0, RADIO_PAYLOAD_FIXED_SIZE);
    radio_write_register(RFM75_RX_PW_P1, RADIO_PAYLOAD_FIXED_SIZE);
	#else
    radio_write_register(RFM75_RX_PW_P0, 0);
    radio_write_register(RFM75_RX_PW_P1, 0);
    #endif*/

    radio_write_register(RFM75_CONFIG, RFM75_CONFIG_EN_CRC_MASK + RFM75_CONFIG_CRCO_MASK + RFM75_CONFIG_PWR_UP_MASK + RFM75_PRIM_RX_MASK);
	log_status_change();
    RADIO_CE_ON();
}




static void radio_set_addr_and_channel(void)
{
	uint8_t a5[5] = {0};
	radio_make_addr5(a5, RADIO_BROADCAST_ADDRESS);

	#ifdef RADIO_PAYLOAD_FIXED_SIZE
	radio_write_register(RFM75_RX_PW_P0, RADIO_PAYLOAD_FIXED_SIZE);
	radio_write_register(RFM75_RX_PW_P1, RADIO_PAYLOAD_FIXED_SIZE);
	#else
	radio_write_register(RFM75_RX_PW_P0, 0);
	radio_write_register(RFM75_RX_PW_P1, 0);
	#endif

	radio_write_register_data(RFM75_RX_ADDR_P0, a5, 5);
	radio_write_register_data(RFM75_TX_ADDR, a5, 5);

	UART_PRINT_P("r broadcast ");
	uart_print_hex(a5, 5);
	uart_print_crlf();

	// We will use P0 as a broadcast address and P1 as own specific address.
	radio_make_addr5(a5, ee.serial_nr);
	radio_write_register_data(RFM75_RX_ADDR_P1, a5, 5);

	// The reset value for RFM75_EN_RXADDR is that both P0 & P1 shall be used
	// so no need to set it.
	radio_write_register(RFM75_EN_RXADDR, RFM75_EN_RXADDR_ERX_P0_MASK | RFM75_EN_RXADDR_ERX_P1_MASK);

	UART_PRINT_P("r own ");
	print_int64(ee.serial_nr);
	uart_putchar(' ');
	uart_print_hex(a5, 5);
	uart_print_crlf();

	// If other data pipes are to be used, set RFM75_RX_ADDR_P2 etc
	// and enable bits in RFM75_EN_RXADDR.
	// radio_write_register_data(RFM75_RX_ADDR_P1, (uint8_t*)addr1, sizeof(addr1));
	// radio_write_register(RFM75_RX_ADDR_P2, addr1[4]+1);
	// radio_write_register(RFM75_RX_ADDR_P2, addr1[4]+2);
	// radio_write_register(RFM75_RX_ADDR_P2, addr1[4]+3);
	// radio_write_register(RFM75_RX_ADDR_P2, addr1[4]+4);
	// radio_write_register(RFM75_EN_RXADDR, 0x3F);

	// set channel, again to avoid someone else's messages as
	// much as possible. Using a random number between 3 and 82:
	// https://www.random.org/integers/?num=1&min=3&max=126&col=5&base=10&format=html&rnd=new
	// 74
	radio_write_register(RFM75_RF_CH, 74);
}


/**
 * Returns number of bytes received.
 * buf_size must be same as RADIO_PAYLOAD_MAX_LEN
 *
 * TODO Shall we provide pipe also? So that caller can know if it was a broadcast message or not.
 */
int8_t radio_receive_take_data(uint8_t *buf_ptr, uint8_t buf_size)
{
	ASSERT(buf_size == RADIO_PAYLOAD_MAX_LEN);

	if (!radio_rx_fifo_is_empty())
	{
		return radio_rx_fifo_take(buf_ptr, buf_size);;
	}

	return radio_pipe_empty;
}

/**
 * Returns zero if message was added to send data queue.
 * -1 if it was busy.
 * buf_size must be same as RADIO_PAYLOAD_SIZE
 */
int8_t radio_transmit_put_data(const uint8_t *buf_ptr, uint8_t buf_size, int32_t addr)
{
	ASSERT(buf_size == RADIO_PAYLOAD_MAX_LEN);

	// Some logging for debugging, can be removed or commented out later.
	UART_PRINT_P("r tx ");
	print_int64(addr);
	uart_putchar(' ');
	uart_print_hex(buf_ptr, buf_size);
	uart_print_crlf();

	if (radio_tx_fifo_is_full())
	{
		UART_PRINT_PL("r full");
		return -1;
	}

	radio_tx_fifo_put(buf_ptr, buf_size, NOF_EXTRA_TRANSMIT, addr);

	return 0;
}

uint8_t radio_fifo_get_free_space_in_tx_queue(void)
{
	switch(state)
	{
		default:
		case radio_initial_state:
		case radio_detecting_state:
		case radio_delay_after_detect_state:
		case radio_send_hello_state:
		case radio_fail_state:
			break;
		case radio_check_result_after_sending_state:
		case radio_delay_after_sending:
		case radio_check_result_after_send_state:
		case radio_send_data_state:
			return radio_tx_fifo_get_free_space_in_queue();
	}
	return 0;
}



void radio_init(void)
{
	avr_spi_init();

	//UART_PRINT_PL("r init");

	radio_rx_fifo_init();
	radio_tx_fifo_init();

    RADIO_CE_OFF();
    RADIO_CE_ENABLE();
    AVR_DELAY_US(20);

    RADIO_CSN_OFF();
    RADIO_CSN_ENABLE();
    AVR_DELAY_US(20);

    RADIO_IRQ_INIT();
    AVR_DELAY_US(20);

	radio_clear_all_flags();
    AVR_DELAY_US(20);


#ifdef RADIO_PAYLOAD_FIXED_SIZE

	// Turn off Auto Ack & Automatic Retransmission
    radio_write_register(RFM75_EN_AA, 0);
	radio_write_register(RFM75_SETUP_RETR, 0);

	radio_set_addr_and_channel();

#else

	// Enables "Dynamic Payload Length" and "W_TX_PAYLOAD_NOACK command".
	radio_write_register(RFM75_FEATURE, RFM75_EN_DPL_MASK | RFM75_EN_DYN_ACK_MASK);

	// Enable auto acknowledgement data pipe 0 & 1
	// This is default setting so skip setting it.
	//radio_write_register(RFM75_EN_AA, RFM75_ENAA_P0_MASK | RFM75_ENAA_P1_MASK);

	// Try dynamic payload length, ref [2] Table 4 Register Bank 0
	// Enable dynamic payload length data pipe0.
	// (Requires EN_DPL and ENAA_P0)
	// "In order to enable DPL the EN_DPL bit in the FEATURE register must be set.
	// In RX mode the DYNPD register has to be set. A PTX that transmits to a PRX
	// with DPL enabled must have the DPL_P0 bit in DYNPD set.
	radio_write_register(RFM75_DYNPD, RFM75_DPL_P0_MASK | RFM75_DPL_P1_MASK);

	// Rev [2] Table 2 SPI command, ACTIVATE
	// This write command followed by data 0x73 activates the following features:
	// • R_RX_PL_WID
	// • W_ACK_PAYLOAD
	// • W_TX_PAYLOAD_NOACK
	// TODO Ref [2] say "Use the same command and data to deactivate the registers again."
	// So there is a risk that this will toggle the feature,
	// if so we must first figure out if these are active or not.
	// Or find a reset to default command so that we know the state.
	// If we can't do that then we simply can't use dynamic length.
	// Perhaps we can find hints here: https://github.com/jnk0le/RFM7x-lib
	// Seems to be many issues with these chips. Will just give up on dynamic length.
	send_activate_command(0x73);

	radio_set_addr_and_channel();

#endif


    timer_ms = avr_systime_ms_16() + 5000;
}

/*
// Deprecated, remove later
void radio_add_generic_message_hdr(struct utility_w_struct *uws, uint8_t msg_cat, uint8_t sub_cmd)
{
	const uint32_t own_serialnumber = ee.serial_nr & 0xFFFFFFFF;

	utility_w_put16(uws, MSG_CATEGORY_SIZE_NOF_BITS, msg_cat);
	utility_w_put16(uws, MSG_SUBCMD_SIZE_NOF_BITS, sub_cmd);
	utility_w_put16(uws, SEQUENCE_NUMBER_NOF_BITS, radio_take_sequence_number());
	utility_w_put64(uws, ADDR_NR_NOF_BITS, own_serialnumber);
}

// Deprecated, remove later
void radio_add_getset_message_hdr(struct utility_w_struct *uws, uint8_t msg_cat, uint8_t sub_cmd, uint8_t par)
{
	radio_add_generic_message_hdr(uws, msg_cat, sub_cmd);

	utility_w_put16(uws, PAR_ID_NOF_BITS, par);
	utility_w_put16(uws, HDR_SPARE_NOF_BITS, 0);
}
*/

// Make a hello message and put it in transmit queue.
// See also log_bin_hello.
static void make_hello_message(void)
{
	struct radio_data_packer packer;
	radio_data_packer_init(&packer, msg_category_status, status_hello, radio_take_sequence_number());
	radio_data_packer_put64(&packer, SERIAL64_NR_NOF_BITS, ee.serial_nr);
	radio_transmit_put_data(packer.buffer, sizeof(packer.buffer), RADIO_BROADCAST_ADDRESS);
}


static void check_and_process_incomeing_msg(void)
{
	if (radio_is_data_received() || (!read_rx_empty()))
	{
		struct Radio_rx_fifo_entry rx_data;
		radio_read_rx_data(&rx_data);
		radio_clear_data_received();

		// Simple way to do retransmission, send everything twice.
		// Then here when receiving duplicates are ignored.
		if (radio_rx_fifo_is_data_seen_recently(&rx_data))
		{
			// ignore data already seen
			if (log_counter != 0)
			{
				//UART_PRINT_PL("r duplicate");
				log_counter--;
			}
		}
		else
		{
			radio_rx_fifo_put(&rx_data);

			// Some logging for debugging, can be removed or commented out later.
			UART_PRINT_P("r rx ");
			print_int16(rx_data.pipe);
			uart_putchar(' ');
			#ifndef RADIO_PAYLOAD_FIXED_SIZE
			print_int16(rx_data.data, rx_data.length);
			uart_putchar(' ');
			uart_print_hex(rx_data.data, rx_data.length);
			#else
			uart_print_hex(rx_data.data, RADIO_PAYLOAD_FIXED_SIZE);
			#endif

			uart_print_crlf();
			log_counter = -1;
		}

		nof_msg_received++;
	}
}

void radio_process(void)
{
	const int16_t t = avr_systime_ms_16();
	const int16_t d = t - timer_ms;

	radio_read_status();

	switch(state)
	{
	case radio_initial_state:
		if (d >= 0)
		{
			radio_read_status();

			state++;
			counter = 0;

			timer_ms = t + 1000;
		}
		break;

	case radio_detecting_state:
		if (uart_get_free_space_in_write_buffer() >= (UART_TX_BUFFER_SIZE/2))
		{
			const int8_t r = radio_log_register(counter);

			if ((radio_detected == 0) && (r != 0) && (r != -1) && (counter < 10))
			{
				UART_PRINT_PL("r detected");
				radio_detected = 1;
				state = radio_delay_after_detect_state;
				timer_ms = t + 200;
			}

			++counter;
			if (counter > 0x1F)
			{
				counter = 0;
				if (radio_detected == 0)
				{
					UART_PRINT_PL("r NOK");
					state = 0xFF;
				}
				else
				{
					state = radio_delay_after_detect_state;
					timer_ms = t + 200;
				}
			}
			else
			{
				timer_ms = t + 100;
			}
		}
		break;

	case radio_delay_after_detect_state:
		if (d >= 0)
		{
			radio_tx_mode();
			timer_ms = t + 10;
			state = radio_send_hello_state;
		}
		break;

	case radio_send_hello_state:
	{
		#if 0
		if (d >= 0)
		{
			uint8_t data[RADIO_PAYLOAD_MAX_LEN] = {'H', 'e', 'l','l','o'};
			radio_send_data(data, sizeof(data));
 			timer_ms = t + 1000;
			state = radio_check_result_after_sending_state;
		}
		#else
		if (d >= 0)
		{
			make_hello_message();
 			timer_ms = t + 2;
			state = radio_delay_after_sending;
		}
		#endif
		break;
	}

	case radio_check_result_after_sending_state:
		if (radio_is_max_retrans())
		{
			UART_PRINT_PL("r max");
			radio_clear_all_flags();
			radio_flush_tx();
			timer_ms = t + 2;
			state = radio_delay_after_sending;
		}
		else if (radio_is_data_sent())
		{
			//UART_PRINT_PL("r ok");
			radio_clear_all_flags();
			timer_ms = t + 2;
			state = radio_delay_after_sending;
		}
		else if (d>0)
		{
			// timeout
			UART_PRINT_PL("r timeout");
			radio_clear_all_flags();
			timer_ms = t + 2;
			state = radio_delay_after_sending;
		}
		break;

	case radio_delay_after_sending:
		// This state is just a short delay, 1 or 2 ms.
		if (d >= 0)
		{
			radio_rx_mode();

			// Timer is set so that no (new) transmit is done until a randomly long delay.
			timer_ms = t + 2 + utility_pseudo_random(32);
			state = radio_check_result_after_send_state;
		}
		break;

	case radio_check_result_after_send_state:
		if (radio_is_max_retrans())
		{
			UART_PRINT_PL("r max late");
			radio_clear_max_retrans();
		}
		if (radio_is_data_sent())
		{
			UART_PRINT_PL("r OK late");
			radio_clear_data_sent();
		}

		check_and_process_incomeing_msg();

		// Don't send again until after some random long time of receiving.
		if (d >= 0)
		{
			// Now its OK to send again, check if there is something.
			if (!radio_tx_fifo_is_empty())
			{
				// There is data to be sent
				radio_tx_mode();
				timer_ms = t + 2;
				state = radio_send_data_state;
			}
			else
			{
				// Stay in state, no more delay.
				timer_ms = t;
			}
		}
		break;

	case radio_send_data_state:
		if (d >= 0)
		{
			struct Radio_tx_fifo_entry tx_data;
			radio_tx_fifo_take(&tx_data);
			radio_send_data(&tx_data);

			timer_ms = t + 1000;
			state = radio_check_result_after_sending_state;
		}

		break;
	case radio_fail_state:
	default:
		if (d >= 0)
		{
			UART_PRINT_PL("r fail");
			radio_read_status();
			timer_ms = t + 5000;
		}
		break;
	}

	if ((state != log_state) || ((status != log_status)))
	{
		/*UART_PRINT_P("r s ");
		uart_print_hex4(state);
		uart_putchar(' ');
		uart_print_hex8(status);
		uart_print_crlf();*/
		log_state = state;
		log_status = status;
	}
}

void radio_tick()
{
	switch(cleanup_state)
	{
		default:
		case cleanup_idle:
		{
			if (!radio_rx_fifo_is_recent_messages_empty())
			{
				// we have old messages to cleanup, but wait a second before deleting
				// since the list of old messages is used to detect retransmissions (duplicates)
				cleanup_state = cleanup_active;
			}
			break;
		}
		case cleanup_active:
		{
			UART_PRINT_PL("r c");
			radio_rx_fifo_delete_one_from_recent_if_any();
			if (radio_rx_fifo_is_recent_messages_empty())
			{
				// no more old messages to delete.
				cleanup_state = cleanup_idle;
			}
			break;
		}
	}
}

int8_t radio_take_sequence_number(void)
{
	return sequence_number++;
}

// The header is currently 6 bytes.
// See also bin_msg_received & interpret_std_bin_msg where unpacking of these messages are done.
// TODO Add a magic number also. There should be 8 bits available for that.
void radio_data_packer_init(struct radio_data_packer *packer, uint8_t msg_cat, uint8_t sub_cmd, uint8_t seq_nr)
{
	const uint32_t own_addr = ee.serial_nr & 0xFFFFFFFF;
	utility_memclr(packer->buffer, sizeof(packer->buffer));
	utility_w_init(&packer->uws, packer->buffer, sizeof(packer->buffer)*8);
	utility_w_put16(&packer->uws, MSG_CATEGORY_SIZE_NOF_BITS, msg_cat);
	utility_w_put16(&packer->uws, MSG_SUBCMD_SIZE_NOF_BITS, sub_cmd);
	utility_w_put16(&packer->uws, SEQUENCE_NUMBER_NOF_BITS, seq_nr);
	utility_w_put64(&packer->uws, ADDR_NR_NOF_BITS, own_addr);
}

void radio_data_packer_put16(struct radio_data_packer *packer, uint8_t nof_bits_to_write, uint16_t d)
{
	utility_w_put16(&packer->uws, nof_bits_to_write, d);
}

void radio_data_packer_put64(struct radio_data_packer *packer, uint8_t nof_bits_to_write, uint64_t d)
{
	utility_w_put64(&packer->uws, nof_bits_to_write, d);
}

int8_t radio_state(void)
{
	return state;
}

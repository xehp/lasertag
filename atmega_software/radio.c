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
#include "utility.h"
#include "radio.h"






// Ref [2] 6.3.1 SPI Command table 3
#define RFM75_R_REGISTER 0
#define RFM75_W_REGISTER 0x20
#define RFM75_R_RX_PAYLOAD 0x61
#define RFM75_W_TX_PAYLOAD 0xA0
#define RFM75_FLUSH_TX 0xE1
#define RFM75_FLUSH_RX 0xE2
#define RFM75_REUSE_TX_PL 0xE3
#define RFM75_ACTIVAE 0x50
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
#define RFM75_RX_EMPTY_MASK 1
#define RFM75_EN_DPL 4
#define RFM75_RFM75_DPL_P0 1
#define RFM75_RX_P_NO_MASK 0xe

#define RFM75_RF_CH_DEFAULT 2


#define radio_is_data_received() (status & RFM75_STATUS_RX_DR_MASK)
#define radio_is_max_retrans() (status & RFM75_STATUS_MAX_RT_MASK)
#define radio_is_data_sent() (status & RFM75_STATUS_TX_DR_MASK)
#define radio_get_rx_p_no() ((status & RFM75_RX_P_NO_MASK) >> 1)


static uint8_t state = 0;
static int16_t timer_ms = 0;
static int16_t counter = 0;
static uint8_t status = 0;
static uint8_t log_state = 0;
static uint8_t log_status = 0;
static uint8_t logged_rx_pw_p0 = 0;
static uint8_t logged_rx_p_no = 0;
static int16_t pseudo_random_counter = 0;

static void radio_fifo_init(struct Radio_fifo *fifoPtr)
{
	fifoPtr->head = 0;
	fifoPtr->tail = 0;
	for (uint8_t i=0; i<RADIO_FIFO_SIZE; i++)
	{
		fifoPtr->buffer[i].length=0;
	}
}

static int8_t radio_fifo_is_full(const struct Radio_fifo *fifoPtr)
{
  return (((uint8_t)RADIO_FIFO_INC(fifoPtr->head)) == fifoPtr->tail);
}

static void radio_fifo_put(struct Radio_fifo *fifoPtr, const uint8_t *data, uint8_t data_len)
{
	if (radio_fifo_is_full(fifoPtr))
	{
		fifoPtr->tail = RADIO_FIFO_INC(fifoPtr->tail);
	}
	ASSERT(data_len <= RADIO_PAYLOAD_SIZE);
	utility_memcpy(fifoPtr->buffer[fifoPtr->head].data, data, data_len);
	fifoPtr->buffer[fifoPtr->head].length=data_len;
	fifoPtr->head = RADIO_FIFO_INC(fifoPtr->head);
}

static int8_t radio_fifo_is_empty(const struct Radio_fifo *fifoPtr)
{
  return (fifoPtr->head == fifoPtr->tail);
}

static const uint8_t radio_fifo_take(struct Radio_fifo *fifoPtr, uint8_t *buf_ptr, uint8_t buf_size)
{
	if (radio_fifo_is_empty(fifoPtr))
	{
		return 0;
	}
	const uint8_t len = fifoPtr->buffer[fifoPtr->tail].length;
	ASSERT(buf_size >= len);
	utility_memcpy(buf_ptr, fifoPtr->buffer[fifoPtr->tail].data, len);
	fifoPtr->tail = RADIO_FIFO_INC(fifoPtr->tail);
	return len;
}

static uint8_t radio_fifo_get_entries_in_queue(const struct Radio_fifo *fifoPtr)
{
    return ( fifoPtr->tail - fifoPtr->head ) & RADIO_FIFO_MASK;
}

static uint8_t radio_fifo_get_free_space_in_queue(const struct Radio_fifo *fifoPtr)
{
	return((RADIO_FIFO_SIZE-1)-radio_fifo_get_entries_in_queue(fifoPtr));
}

static uint8_t is_data_seen_before(const struct Radio_fifo *fifoPtr, const uint8_t *buf_ptr, uint8_t buf_size)
{
	uint8_t f=0;
	for (uint8_t i=0; i<RADIO_FIFO_SIZE; i++)
	{
		if (buf_size == fifoPtr->buffer[i].length)
		{
			if (utility_memcmp((const int8_t*)buf_ptr, (const int8_t*)fifoPtr->buffer[i].data, buf_size) == 0)
			{
				f++;
			}
		}
	}
	return f;
}

struct Radio_fifo receive_fifo;
struct Radio_fifo transmit_fifo;


static void log_status_change(void)
{
	#if 0
	uint8_t irq = RADIO_IRQ_READ();
	if (irq != logged_irq)
	{
		UART_PRINT_P("irq ");
		uart_print_hex4(irq);
		uart_print_crlf();
		logged_irq = irq;
	}
	if (status != log_status)
	{
		UART_PRINT_P("status ");
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


static void radio_read_rx_data(uint8_t *data, uint8_t len)
{
	//UART_PRINT_P("radio_read_rx_data\r\n");

	// TODO Use variable message length, see ref [2].
	const uint8_t pipe_number = radio_get_rx_p_no();
	const uint8_t nof_bytes = read_nof_bytes(pipe_number);

	if ((nof_bytes != logged_rx_pw_p0) || (pipe_number != logged_rx_p_no))
	{
		uart_putchar('p');
		uart_print_hex4(pipe_number);
		uart_putchar(' ');
		uart_print_hex8(nof_bytes);
		uart_print_crlf();
		logged_rx_pw_p0 = nof_bytes;
		logged_rx_p_no = pipe_number;
	}

	RADIO_CSN_ON();
	status = avr_spi_transfer(RFM75_R_RX_PAYLOAD);
	for (int8_t i=0; i<len; ++i)
	{
		data[i] = avr_spi_transfer(0);
	}
	RADIO_CSN_OFF();
	log_status_change();
}

void radio_send_data(const uint8_t *data, uint8_t len)
{
	RADIO_CE_OFF();
	AVR_DELAY_US(10);

	radio_write_command(RFM75_FLUSH_TX);
	AVR_DELAY_US(10);

	RADIO_CSN_ON();
	status = avr_spi_transfer(RFM75_W_TX_PAYLOAD);
	for (int8_t i=0; i<len; ++i)
	{
		avr_spi_transfer(data[i]);
	}
	AVR_DELAY_US(10);
	RADIO_CSN_OFF();
	AVR_DELAY_US(10);

	RADIO_CE_ON();

	// Some logging for debugging, can be removed or commented out later.
	if (uart_get_free_space_in_write_buffer() >= (UART_TX_BUFFER_SIZE/4))
	{
		UART_PRINT_P("rt ");
		for (uint8_t i=0; i<len; ++i)
		{
			uart_print_hex8(data[i]);
		}
		uart_print_crlf();
	}
	else
	{
		uart_putchar('%');
		AVR_DELAY_US(20);
	}

	RADIO_CE_OFF();

	log_status_change();
}


void radio_flush_rx(void)
{
	RADIO_CSN_ON();
	status = avr_spi_transfer(RFM75_FLUSH_RX);
	RADIO_CSN_OFF();
	log_status_change();
}


/*
static void radio_log_register(uint8_t reg_id)
{
	UART_PRINT_P("R ");
	uart_print_hex8(reg_id);
	uart_putchar(' ');
	RADIO_CSN_ON();
	status = avr_spi_transfer(RFM75_R_REGISTER | reg_id);
	switch(reg_id)
	{
		case RFM75_RX_ADDR_P0:
		case RFM75_RX_ADDR_P1:
		case RFM75_TX_ADDR:
		{
			for (int8_t i=0; i<5; ++i)
			{
				uart_print_hex8(avr_spi_transfer(0));
			}
			break;
		}
		default:
		{
			uart_print_hex8(avr_spi_transfer(0));
			break;
		}
	}
	RADIO_CSN_OFF();
	uart_print_crlf();
}
*/

static uint8_t get_reg_size(uint8_t reg_id)
{
	switch(reg_id)
	{
		case RFM75_RX_ADDR_P0:
		case RFM75_RX_ADDR_P1:
		case RFM75_TX_ADDR:
			return 5;
		default:
			return 1;
	}
}

static void radio_log_register(uint8_t reg_id)
{
	uint8_t tmp[5];
	uint8_t len = get_reg_size(reg_id);
	radio_read_register_data(reg_id, tmp, len);

	UART_PRINT_P("Reg ");
	uart_print_hex8(reg_id);
	uart_putchar(' ');
	for(uint8_t i = 0; i<len; i++)
	{
		uart_print_hex8(tmp[i]);
	}
	uart_print_crlf();
}



static void radio_tx_mode(void)
{
	//UART_PRINT_P("tx mode\r\n");

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
	//UART_PRINT_P("radio_rx_mode\r\n");

	radio_clear_data_received();
	radio_flush_rx();
    radio_write_register(RFM75_RX_PW_P0, RADIO_PAYLOAD_SIZE);
    radio_write_register(RFM75_CONFIG, RFM75_CONFIG_EN_CRC_MASK + RFM75_CONFIG_CRCO_MASK + RFM75_CONFIG_PWR_UP_MASK + RFM75_PRIM_RX_MASK);
	log_status_change();
    RADIO_CE_ON();
}





static void radio_set_addr_and_channel(void)
{
	// The address we use more like a magic number (that is to ignore
	// messages not concerning us. So we use a random number here.
	// https://www.random.org/cgi-bin/randbyte?nbytes=5&format=h
	// 9c 33 d3 9e bc
	static const uint8_t addr[5] = {0x9c, 0x33, 0xd3, 0x9e, 0xbc};
	radio_write_register_data(RFM75_RX_ADDR_P0, (uint8_t*)addr, sizeof(addr));
	radio_write_register_data(RFM75_TX_ADDR, (uint8_t*)addr, sizeof(addr));

	// Disable all pipes except P0.
	radio_write_register(RFM75_EN_RXADDR, RFM75_EN_RXADDR_ERX_P0_MASK);
	// If other data pipes are to be used, set RFM75_RX_ADDR_P2 etc
	// and enable bits in RFM75_EN_RXADDR.
	// radio_write_register_data(RFM75_RX_ADDR_P1, (uint8_t*)addr1, sizeof(addr1));
	// radio_write_register(RFM75_RX_ADDR_P2, addr1[4]+1);
	// radio_write_register(RFM75_RX_ADDR_P2, addr1[4]+2);
	// radio_write_register(RFM75_RX_ADDR_P2, addr1[4]+3);
	// radio_write_register(RFM75_RX_ADDR_P2, addr1[4]+4);
	// radio_write_register(RFM75_EN_RXADDR, 0x3F);

	// set channel, again to avoid someone elses messages as
	// much as possible. Using a random number between 3 and 126:
	// https://www.random.org/integers/?num=1&min=3&max=126&col=5&base=10&format=html&rnd=new
	// 74
	radio_write_register(RFM75_RF_CH, 74);
}


/**
 * Returns number of bytes received.
 * buf_size must be same as RADIO_PAYLOAD_SIZE
 */
uint8_t radio_receive_data(uint8_t *buf_ptr, uint8_t buf_size)
{
	if (!radio_fifo_is_empty(&receive_fifo))
	{
		return radio_fifo_take(&receive_fifo, buf_ptr, buf_size);

	}
	return 0;
}

/**
 * Returns zero if message was added to send data queue.
 * -1 if it was busy.
 * buf_size must be same as RADIO_PAYLOAD_SIZE
 */
int8_t radio_transmit_data(const uint8_t *buf_ptr, uint8_t buf_size)
{
	if (radio_fifo_is_full(&transmit_fifo))
	{
		UART_PRINT_P("radio full\r\n");
		return -1;
	}

	radio_fifo_put(&transmit_fifo, buf_ptr, buf_size);

	// Simple way to do retransmission, send everything twice.
	// Then when receiving duplicates are ignored.
	if (!radio_fifo_is_full(&transmit_fifo))
	{
		radio_fifo_put(&transmit_fifo, buf_ptr, buf_size);
	}

	return 0;
}

uint8_t radio_fifo_get_free_space_in_tx_queue(void)
{
	return radio_fifo_get_free_space_in_queue(&transmit_fifo);
}

// For values of n not being a power of 2 this will not give an even distribution
// of numbers. lower numbers will be a bit more common than higher.
// Does not matter for this application.
uint8_t radio_pseudo_random(uint8_t n)
{
	const int16_t t = avr_systime_ms_16();
	pseudo_random_counter = pseudo_random_counter * 31 + t;
	return pseudo_random_counter % n;
}


void radio_init(void)
{
	UART_PRINT_P("radio_init\r\n");

	avr_spi_init();

	radio_fifo_init(&receive_fifo);
	radio_fifo_init(&transmit_fifo);

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


#ifndef USE_AUTO_ACK
	// Turn off Auto Ack
    radio_write_register(RFM75_EN_AA, 0);
	radio_write_register(RFM75_SETUP_RETR, 0);
#else
	//radio_write_register(RFM75_EN_AA, RFM75_EN_AA_ENAA_P0);
	//radio_write_register(RFM75_SETUP_RETR, RF24_SETUP_RETR_ARD_750|RF24_SETUP_RETR_ARC_15);
#endif

	radio_set_addr_and_channel();

	// TODO Try dynamic payload length
	// radio_write_register(RFM75_DYNPD, RFM75_DPL_P0);
	// radio_write_register(RFM75_FEATURE, RFM75_EN_DPL);


    timer_ms = avr_systime_ms_16() + 5000;
}

void radio_process(void)
{
	const int16_t t = avr_systime_ms_16();
	const int16_t d = t - timer_ms;

	radio_read_status();

	switch(state)
	{
	case 0:
		if (d >= 0)
		{
			radio_read_status();

			state++;
			counter = 0;

			timer_ms = t + 1000;
		}
		break;

	case 1:
		if (uart_get_free_space_in_write_buffer() >= (UART_TX_BUFFER_SIZE/2))
		{
			radio_log_register(counter);
			++counter;

			if (counter > 17)
			{
				counter = 0;
				state++;
				timer_ms = t + 1000;
			}
			else
			{
				timer_ms = t + 100;
			}
		}
		break;

	case 2:
		if (d >= 0)
		{
			radio_tx_mode();
			timer_ms = t + 2;
			state++;
		}
		break;

	case 3:
	{
		if (d >= 0)
		{
			uint8_t data[RADIO_PAYLOAD_SIZE] = {'H', 'e', 'l','l','o'};
			radio_send_data(data, sizeof(data));
 			timer_ms = t + 1000;
			state = 4;
		}
		break;
	}

	case 4:
		if (radio_is_max_retrans())
		{
			UART_PRINT_P("max_rt\r\n");
			radio_clear_all_flags();
			radio_flush_tx();
			timer_ms = t + 2;
			state++;
		}

		if (radio_is_data_sent())
		{
			UART_PRINT_P("sent ok\r\n");
			radio_clear_all_flags();
			timer_ms = t + 2;
			state++;
		}

		if (d>0)
		{
			UART_PRINT_P("timeout\r\n");
			radio_clear_all_flags();
			timer_ms = t + 2;
			state++;
		}
		break;

	case 5:
		if (d >= 0)
		{
			radio_rx_mode();

			// Timer is set so that no (new) transmit is done until a randomly long delay.
			timer_ms = t + 2 + radio_pseudo_random(32);
			state++;
		}
		break;

	case 6:
		if (radio_is_max_retrans())
		{
			UART_PRINT_P("max_rt (late)\r\n");
			radio_clear_max_retrans();
		}
		if (radio_is_data_sent())
		{
			UART_PRINT_P("sent ok (late)\r\n");
			radio_clear_data_sent();
		}

		if (radio_is_data_received() || (!read_rx_empty()))
		{
			//radio_log_register(RFM75_RX_PW_P0);
			uint8_t data[RADIO_PAYLOAD_SIZE] = {0};
			radio_read_rx_data(data, sizeof(data));

			radio_clear_data_received();

			// Simple way to do retransmission, send everything twice.
			// Then here when receiving duplicates are ignored.
			if (is_data_seen_before(&receive_fifo, data, RADIO_PAYLOAD_SIZE))
			{
				// ignore data already seen
				UART_PRINT_P("rr\r\n");
			}
			else
			{
				radio_fifo_put(&receive_fifo, data, RADIO_PAYLOAD_SIZE);

				// Some logging for debugging, can be removed or commented out later.
				if (uart_get_free_space_in_write_buffer() >= (UART_TX_BUFFER_SIZE/4))
				{
					UART_PRINT_P("rr ");
					for (uint8_t i=0; i<sizeof(data); ++i)
					{
						uart_print_hex8(data[i]);
					}
					uart_print_crlf();
				}
				else
				{
					uart_putchar('$');
				}
			}

		}

		if (d >= 0)
		{
			// Don't send again until after some random long time of receiving.
			if (!radio_fifo_is_empty(&transmit_fifo))
			{
				// There is data to be sent
				radio_tx_mode();
				timer_ms = t + 2;
				state++;
			}
			else
			{
				// Stay in state, no more delay.
				timer_ms = t;
			}
		}
		break;

	case 7:
		if (d >= 0)
		{
			uint8_t data[RADIO_PAYLOAD_SIZE];
			radio_fifo_take(&transmit_fifo, data, sizeof(data));
			radio_send_data(data, sizeof(data));

			timer_ms = t + 1000;
			state = 4;
		}

		break;
	default:
		if (d >= 0)
		{
			UART_PRINT_P("default\r\n");
			radio_read_status();
			timer_ms = t + 5000;
		}
		break;
	}

	if ((state != log_state) || ((status != log_status)))
	{
		UART_PRINT_P("rs ");
		uart_print_hex4(state);
		uart_putchar(' ');
		uart_print_hex8(status);
		uart_print_crlf();
		log_state = state;
		log_status = status;
	}
}


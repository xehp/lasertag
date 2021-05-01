/*
radio.c

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
#define RFM75_RF_CH 0x05
#define RFM75_STATUS 0x07
#define RFM75_RX_ADDR_P0 0x0a
#define RFM75_RX_ADDR_P1 0x0b
#define RFM75_TX_ADDR 0x10
#define RFM75_RX_PW_P0 0x11

// Bit masks for registers.
#define RFM75_CONFIG_EN_CRC_MASK 0x08
#define RFM75_CONFIG_PWR_UP_MASK 0x02
#define RFM75_STATUS_RX_DR_MASK 0x40
#define RFM75_STATUS_TX_DR_MASK 0x20
#define RFM75_STATUS_MAX_RT_MASK 0x10
#define RFM75_PRIM_RX_MASK 0x01
#define RFM75_EN_RXADDR_ERX_P0_MASK 1

#define RFM75_RF_CH_DEFAULT 2


#define radio_is_data_received() (status & RFM75_STATUS_RX_DR_MASK)
#define radio_is_max_retrans() (status & RFM75_STATUS_MAX_RT_MASK)
#define radio_is_data_sent() (status & RFM75_STATUS_TX_DR_MASK)


static uint8_t state = 0;
static int16_t timer_ms = 0;
static int16_t counter = 0;
static uint8_t status = 0;
static uint8_t log_status = 0;
static uint8_t logged_irq = 0;
static uint8_t logged_rx_pw_p0 = 0;


static void radio_copy(uint8_t *dst, const uint8_t *src, uint8_t n)
{
	while(n>0)
	{
		*dst = *src;
		dst++;
		src++;
		n--;
	}
}

void radio_fifo_init(struct Radio_fifo *fifoPtr)
{
	fifoPtr->head = 0;
	fifoPtr->tail = 0;
}

void radio_fifo_put(struct Radio_fifo *fifoPtr, const uint8_t *data, uint8_t data_len)
{
	if (radio_fifo_is_full(fifoPtr))
	{
		fifoPtr->tail = RADIO_FIFO_INC(fifoPtr->tail);
	}
	ASSERT(data_len == RADIO_PAYLOAD_SIZE);
	radio_copy(fifoPtr->buffer[fifoPtr->head].data, data, RADIO_PAYLOAD_SIZE);
	fifoPtr->head = RADIO_FIFO_INC(fifoPtr->head);
}

int8_t radio_fifo_is_full(struct Radio_fifo *fifoPtr)
{
  return (((uint8_t)RADIO_FIFO_INC(fifoPtr->head)) == fifoPtr->tail);
}

int8_t radio_fifo_is_empty(struct Radio_fifo *fifoPtr)
{
  return (fifoPtr->head == fifoPtr->tail);
}

const uint8_t radio_fifo_take(struct Radio_fifo *fifoPtr, uint8_t *buf_ptr, uint8_t buf_size)
{
	if (radio_fifo_is_empty(fifoPtr))
	{
		return 0;
	}
	ASSERT(buf_size == RADIO_PAYLOAD_SIZE);
	radio_copy(buf_ptr, fifoPtr->buffer[fifoPtr->tail].data, buf_size);
	fifoPtr->tail = RADIO_FIFO_INC(fifoPtr->tail);
	return RADIO_PAYLOAD_SIZE;
}




struct Radio_fifo receive_fifo;
struct Radio_fifo transmit_fifo;


static void log_status_change(void)
{
	uint8_t irq = RADIO_IRQ_READ();
	if (irq != logged_irq)
	{
		UART_PRINT_P("irq ");
		uart_print_hex4(irq);
		UART_PRINT_P("\r\n");
		logged_irq = irq;
	}
	if (status != log_status)
	{
		UART_PRINT_P("status ");
		uart_print_hex8(status);
		UART_PRINT_P("\r\n");
		log_status = status;
	}
	AVR_DELAY_US(15);
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
	uint8_t tmp = radio_read_register(RFM75_RX_PW_P0);
	if (tmp != logged_rx_pw_p0)
	{
		UART_PRINT_P("rx_pw_p0 ");
		uart_print_hex8(tmp);
		UART_PRINT_P("\r\n");
		logged_rx_pw_p0 = tmp;
	}

	radio_write_command(RFM75_NOP);
}

static void radio_read_rx_data(uint8_t *data, uint8_t len)
{
	UART_PRINT_P("reading\r\n");

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
	UART_PRINT_P("sending ");
	for (uint8_t i=0; i<len; ++i)
	{
		uart_print_hex8(data[i]);
	}
	UART_PRINT_P("\r\n");

	RADIO_CE_OFF();

	radio_write_command(RFM75_FLUSH_TX);
	AVR_DELAY_US(11);

	RADIO_CSN_ON();
	status = avr_spi_transfer(RFM75_W_TX_PAYLOAD);
	for (int8_t i=0; i<len; ++i)
	{
		avr_spi_transfer(data[i]);
	}
	AVR_DELAY_US(11);
	RADIO_CSN_OFF();
	AVR_DELAY_US(11);
	RADIO_CE_ON();
	AVR_DELAY_US(20);
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
	UART_PRINT_P("\r\n");
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
	UART_PRINT_P("\r\n");
}



static void radio_tx_mode(void)
{
	//UART_PRINT_P("tx mode\r\n");

	// Set CE ON only later when it is time to send (see radio_send_data).
    RADIO_CE_OFF();

    radio_flush_tx();
    radio_clear_all_flags();

    //radio_write_one_byte_register(RFM75_TX_PW, PAYLOAD_SIZE);
    radio_write_register(RFM75_CONFIG, RFM75_CONFIG_EN_CRC_MASK + RFM75_CONFIG_PWR_UP_MASK);
	log_status_change();

	//radio_write_register(RFM75_EN_AA, RFM75_EN_AA_ENAA_P0);
	//radio_write_register(RFM75_SETUP_RETR, RF24_SETUP_RETR_ARD_750|RF24_SETUP_RETR_ARC_15);
}

void radio_rx_mode(void)
{
	//UART_PRINT_P("rx mode\r\n");

	radio_clear_data_received();
	radio_flush_rx();
    radio_write_register(RFM75_RX_PW_P0, RADIO_PAYLOAD_SIZE);
    radio_write_register(RFM75_CONFIG, RFM75_CONFIG_EN_CRC_MASK + RFM75_CONFIG_PWR_UP_MASK + RFM75_PRIM_RX_MASK);
	log_status_change();
    RADIO_CE_ON();
}





static void radio_set_addr_etc(void)
{
	// TODO something random here?
	//static const uint8_t addr[5] = {0x1, 0x2, 0x3, 0x4, 0x5};
	//radio_write_register_data(RFM75_RX_ADDR_P0, (uint8_t*)addr, sizeof(addr));
	//radio_write_register_data(RFM75_TX_ADDR, (uint8_t*)addr, sizeof(addr));
	//radio_write_register(RFM75_EN_RXADDR, RFM75_EN_RXADDR_ERX_P0_MASK);

	// set channel
	// TODO Perhaps use some other channel.
	//radio_write_register(RFM75_RF_CH, RFM75_RF_CH_DEFAULT);
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
	if (!radio_fifo_is_full(&transmit_fifo))
	{
		radio_fifo_put(&transmit_fifo, buf_ptr, buf_size);
		return 0;
	}
	return -1;
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

	radio_set_addr_etc();

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
		if (d >= 0)
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
			timer_ms = t + 100;
			state++;
		}
		break;

	case 3:
	{
		if (d >= 0)
		{
			uint8_t data[RADIO_PAYLOAD_SIZE] = {'H', 'e', 'l','l','o'};
			radio_send_data(data, sizeof(data));
			timer_ms = t + 5000;
			state++;
		}
		break;
	}

	case 4:
		if (radio_is_max_retrans())
		{
			UART_PRINT_P("max_rt\r\n");
			radio_clear_all_flags();
			radio_flush_tx();
			timer_ms = t + 20;
			state++;
		}

		if (radio_is_data_sent())
		{
			UART_PRINT_P("sent ok\r\n");
			radio_clear_all_flags();
			timer_ms = t + 20;
			state++;
		}

		if (d>0)
		{
			UART_PRINT_P("timeout\r\n");
			radio_clear_all_flags();
			timer_ms = t + 20;
			state++;
		}
		break;

	case 5:
		if (d >= 0)
		{
			//UART_PRINT_P("rx state\r\n");
			radio_rx_mode();
			timer_ms = t + 20;
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

		if (radio_is_data_received())
		{
			radio_clear_data_received();

			//radio_log_register(RFM75_RX_PW_P0);
			uint8_t data[RADIO_PAYLOAD_SIZE] = {0};
			radio_read_rx_data(data, sizeof(data));

			UART_PRINT_P("received ");
			for (uint8_t i=0; i<sizeof(data); ++i)
			{
				uart_print_hex8(data[i]);
			}
			UART_PRINT_P("\r\n");

			radio_fifo_put(&receive_fifo, data, RADIO_PAYLOAD_SIZE);
		}

		if (!radio_fifo_is_empty(&transmit_fifo))
		{
			radio_tx_mode();
			timer_ms = t + 20;
			state++;
		}

		break;

	case 7:
		if (d >= 0)
		{
			uint8_t data[RADIO_PAYLOAD_SIZE];

			radio_fifo_take(&transmit_fifo, data, sizeof(data));

			radio_send_data(data, sizeof(data));

			//if (radio_fifo_is_empty(&transmit_fifo))
			{
				timer_ms = t + 5000;
				state=4;
			}
		}

		break;
	default:
		if (d >= 0)
		{
			UART_PRINT_P("default\r\n");
			radio_read_status();
			timer_ms = t + 1000;
		}
		break;
	}
}


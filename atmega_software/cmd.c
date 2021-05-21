/*
cmd.h

Provide command line functions.

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

2021-05-01 Created. /Henrik
*/

#include <stdint.h>
#include "avr_cfg.h"
#include "avr_sys.h"
#include "avr_uart.h"
#include "avr_eeprom.h"
#include "utility.h"
#include "power.h"
#include "game.h"
#include "cmd.h"

#define LOG_INT16_P(str, i) {log_int16_p(PSTR(str), i);}


char command_buffer[32+1];
uint8_t command_length = 0;

static void print_uint16(uint16_t i)
{
	char tmp[32];
	utility_lltoa(i, tmp, 10);
	uart_print(tmp);
}

static void log_int16_p(const char *pgm_addr, uint16_t i)
{
	uart_print_P(pgm_addr);
	uart_putchar(' ');
	print_uint16(i);
	uart_print_P(PSTR("\r\n"));
}

static void interpret_get(char *ptr)
{
	while(*ptr==' ')
	{
		ptr++;
	}

	switch(*ptr)
	{
		case 'd':
			LOG_INT16_P("dev", ee.device_type);
			break;
		case 'g':
		{
			LOG_INT16_P("game", game_get_state());
			break;
		}
		case 'i':
			LOG_INT16_P("id", ee.ID);
			break;
		case 'm':
			LOG_INT16_P("n", ee.microVoltsPerUnit);
			break;
		case 'n':
			LOG_INT16_P("n", ee.player_number);
			break;
		case 's':
			LOG_INT16_P("s", ee.stop_charging_battery_at_mv);
			break;
		case 'v':
		{
			LOG_INT16_P("voltage", power_get_voltage_mV());
			break;
		}
		case 'p':
		{
			LOG_INT16_P("power", power_get_state());
			break;
		}
		default:
			UART_PRINT_P("?\r\n");
			break;
	}
}

static void interpret_set(char *ptr)
{
	while(*ptr==' ')
	{
		ptr++;
	}

	switch(*ptr)
	{
		case 'd':
			ee.device_type = utility_atoll(ptr+1);
			LOG_INT16_P("dev", ee.device_type);
			break;
		case 'i':
			ee.ID = utility_atoll(ptr+1);
			LOG_INT16_P("id", ee.ID);
			break;
		case 'm':
			ee.microVoltsPerUnit = utility_atoll(ptr+1);
			LOG_INT16_P("m", ee.player_number);
			break;
		case 'n':
			ee.player_number = utility_atoll(ptr+1);
			LOG_INT16_P("n", ee.player_number);
			break;
		case 's':
			ee.stop_charging_battery_at_mv = utility_atoll(ptr+1);
			LOG_INT16_P("s", ee.stop_charging_battery_at_mv);
			break;
		default:
			UART_PRINT_P("?\r\n");
			break;
	}
}

static void interpret_command(void)
{
	char* ptr = command_buffer;

	while(*ptr==' ')
	{
		ptr++;
	}

	switch (*ptr)
	{
		case 'e':
		{
			eepromSave();
			break;
		}
		case 'g':
		{
			interpret_get(ptr+1);
			break;
		}
		case 'o':
		{
			AVR_ERROR_HANDLER_P("power off", __LINE__);
			break;
		}
		case 'r':
		{
			uart_print_P(PSTR("reboot\r\n"));
			// Do eternal loop until we get reset by WTD.
			for(;;);
			break;
		}
		case 's':
		{
			interpret_set(ptr+1);
			break;
		}
		default:
			UART_PRINT_P("?\r\n");
			break;
	}
}


void cmd_init()
{
	command_length=0;
}


void cmd_process()
{
	const int ch = uart_getchar();
	if (ch >= 0)
	{
		if (ch>=' ')
		{
			if (command_length>=sizeof(command_buffer)-1)
			{
				UART_PRINT_P("?\r\n");
				command_length=0;
			}
			else
			{
				command_buffer[command_length] = ch;
				command_length++;
			}
		}
		else if ((ch == '\n') || (ch == '\r'))
		{
			command_buffer[command_length] = 0;
			interpret_command();
			command_length = 0;
		}
		else if (ch == 0x1b)
		{
			// esc, do eternal loop until we get reset by WTD.
			// TODO Should we do RELAY_OFF() here?
			for(;;);
		}
		else
		{
			uart_print_hex8(ch);
			command_length = 0;
		}
	}
}


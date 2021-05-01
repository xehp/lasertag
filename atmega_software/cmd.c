/*
cmd.h

Provide command line functions.

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

2021-05-01 Created. /Henrik
*/

#include <stdint.h>
#include "avr_cfg.h"
#include "avr_sys.h"
#include "avr_uart.h"
#include "utility.h"
#include "power.h"
#include "cmd.h"


char command_buffer[32+1];
uint8_t command_length = 0;

static void interpret_command(void)
{
	char* ptr = command_buffer;

	if (command_length<sizeof(command_buffer))
	{
		command_buffer[command_length]=0;
	}
	else
	{
		command_buffer[sizeof(command_buffer)-1]=0;
	}

	while(*ptr==' ')
	{
		ptr++;
	}

	switch (*ptr)
	{
		case 'o':
		{
			avr_error_handler_P(PSTR("power off"), __LINE__);
			break;
		}
		case 'r':
		{
			uart_print_P(PSTR("reboot\r\n"));
			// Do eternal loop until we get reset by WTD.
			for(;;);
			break;
		}
		case 'v':
		{
			char tmp[32];
			utility_lltoa(power_get_voltage_mV(), tmp, 10);
			uart_print_P(PSTR("voltage "));
			uart_print(tmp);
			uart_print_P(PSTR(" mV\r\n"));
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
			if (command_length>=sizeof(command_buffer))
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
			interpret_command();
			command_length = 0;
		}
		else if (ch == 0x1b)
		{
			// esc, do eternal loop until we get reset by WTD.
			for(;;);
		}
		else
		{
			uart_print_hex8(ch);
			command_length = 0;
		}
	}
}


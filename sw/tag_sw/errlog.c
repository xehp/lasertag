/*
log.c

Copyright (C) 2021 Henrik Bjorkman www.eit.se/hb.

History

2021-06-30 Created by Henrik Bjorkman
*/


#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "avr_cfg.h"
#include "avr_uart.h"
#include "avr_sys.h"
#include "avr_ports.h"
#include "errlog.h"



// Remember to turn things off that must be turned of in case of error.
// That is application specific code may have to be entered here.
void errlog_P(const char *pgm_addr, uint16_t errorCode)
{
	for(;;)
	{
		uart_print_crlf();
		uart_print_P(pgm_addr);
		uart_putchar(' ');
		uart_print_hex16(errorCode);
		uart_print_crlf();
		avr_blink_debug_led(8);
		avr_delay_ms_16(1000);
		RELAY_OFF();
	}
}

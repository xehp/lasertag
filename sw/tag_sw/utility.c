/*
misc.c

Provide miscellaneous functions.

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

2021-05-01 Created reusing code from other projects. /Henrik
*/

#if defined(__AVR_ATmega328P__)
#include <avr/pgmspace.h>
#include "avr_sys.h"
#elif (defined __linux__)
#include <stdio.h>
#define PSTR(x) x
#define ERRLOG_P(str, line) printf("%s %d\n", str, line);
#else
#error
#endif

#include <stdint.h>
#include "avr_tmr2.h"
#include "avr_uart.h"
//#include <stdlib.h>
//#include <string.h>
//#include <ctype.h>
#include "msg.h"
#include "errlog.h"
#include "utility.h"


/**
 * assuming 7 bits, but if NOF_BITS in avr_tmr2.c is changed
 * then change IR_BIT_IGNORE_MASK below as needed.
 * That is if 8 bits are used set the mask to 0.
 */
#if (SOFT_UART_NOF_BITS == 7)
#define IR_BIT_IGNORE_MASK 0x80
#elif (SOFT_UART_NOF_BITS == 6)
#define IR_BIT_IGNORE_MASK 0xC0
#else
#define IR_BIT_IGNORE_MASK 0x00
#endif

static int16_t pseudo_random_counter = 0;



/* A utility function to reverse a string  */
// Used code from
// http://www.geeksforgeeks.org/implement-itoa/
static void utility_reverse(char str[], int length)
{
    int start = 0;
    int end = length -1;
    while (start < end)
    {
    	// swap(*(str+start), *(str+end));
    	char tmp=*(str+start);
    	*(str+start)=*(str+end);
    	*(str+end)=tmp;

        start++;
        end--;
    }
}


char* utility_lltoa(int64_t num, char* str, int base)
{
    int64_t i = 0;
    char isNegative = 0;

    /* Handle 0 explicitly, otherwise empty string is printed for 0 */
    if (num == 0)
    {
        str[i++] = '0';
        str[i] = '\0';
        return str;
    }

    // In standard itoa(), negative numbers are handled only with
    // base 10. Otherwise numbers are considered unsigned.
    if (num < 0 && base == 10)
    {
        isNegative = 1;
        num = -num;
    }

    // Process individual digits
    while (num != 0)
    {
        int64_t rem = num % base;
        str[i++] = (rem > 9)? (rem-10) + 'a' : rem + '0';
        num = num/base;
    }

    // If number is negative, append '-'
    if (isNegative)
        str[i++] = '-';

    str[i] = '\0'; // Append string terminator

    // Reverse the string
    utility_reverse(str, i);

    return str;
}



int64_t utility_atoll(const char* str)
{
    int64_t value = 0;

    while (*str == ' ')
    {
    	str++;
    }

    if (*str == '-')
    {
    	str++;
    	return -utility_atoll(str);
    }
    else if (*str == '+')
    {
    	str++;
	}

    while ((*str >= '0') && (*str <= '9'))
    {
    	value = value*10 + (*str -'0');
    	str++;
    }

    return value;
}

/*
      LSB first
e0 10 00000111  1  -
f0 10 00001111  1  -
e8 10 00010111  2  0
f8 10 00011111  1  -
e4 10 00100111  2  1
f4 10 00101111  2  2
ec 10 00110111  2  3
fc 10 00111111  1  -
e2 10 01000111  2  4
f2 10 01001111  2  5
ea 10 01010111  3  6          0
fa 10 01011111  2  7
e6 10 01100111  2  8
f6 10 01101111  2  9
ee 10 01110111  2  10
fe 10 01111111  1  -
e1 10 10000111  2  11
f1 10 10001111  2  12
e9 10 10010111  3  13         1
f9 10 10011111  2  14
e5 10 10100111  3  15         2
f5 10 10101111  3  16         3
ed 10 10110111  3  17         4
fd 10 10111111  2  18
e3 10 11000111  2  19
f3 10 11001111  2  20
eb 10 11010111  3  21         5
fb 10 11011111  2  22
e7 10 11100111  2  23
f7 10 11101111  2  24
ef 10 11110111  2  25
ff 10 11111111  1  -
*/

// The first codes are used for player codes, remaining are reserved for later use
// such as safe zones for resp team.
// See also translate_from_ir_code & NOF_AVAILABLE_PLAYER_CODES.
uint8_t translate_to_ir_code(uint8_t n)
{
	switch(n)
	{
	case 0: return 0xe8;
	case 1: return 0xe4;
	case 2: return 0xf4;
	case 3: return 0xec;
	case 4: return 0xe2;
	case 5: return 0xf2;
	case 6: return 0xea;
	case 7: return 0xfa;
	case 8: return 0xe6;
	case 9: return 0xf6;
	case 10: return 0xee;
	case 11: return 0xe1;
	case 12: return 0xf1;
	case 13: return 0xe9;
	case 14: return 0xf9;
	case 15: return 0xe5;
	case 16: return 0xf5;
	case 17: return 0xed;
	case 18: return 0xfd;
	case 19: return 0xe3;
	case 20: return 0xf3;
	case 21: return 0xeb;
	case 22: return 0xfb;
	case 23: return 0xe7;
	case 24: return 0xf7;
	case 25: return 0xef;
	default: break;
	}
	return 0xFF;
}

// See also translate_to_ir_code & NOF_AVAILABLE_PLAYER_CODES.
uint8_t translate_from_ir_code(uint8_t c)
{
	switch(c | /*IR_BIT_IGNORE_MASK*/ 0x80)
	{
		case 0xe8: return 0;
		case 0xe4: return 1;
		case 0xf4: return 2;
		case 0xec: return 3;
		case 0xe2: return 4;
		case 0xf2: return 5;
		case 0xea: return 6;
		case 0xfa: return 7;
		case 0xe6: return 8;
		case 0xf6: return 9;
		case 0xee: return 10;
		case 0xe1: return 11;
		case 0xf1: return 12;
		case 0xe9: return 13;
		case 0xf9: return 14;
		case 0xe5: return 15;
		case 0xf5: return 16;
		case 0xed: return 17;
		case 0xfd: return 18;
		case 0xe3: return 19;
		case 0xf3: return 20;
		case 0xeb: return 21;
		case 0xfb: return 22;
		case 0xe7: return 23;
		case 0xf7: return 24;
		case 0xef: return 25;
		default: break;
	}
	return 0xFF;
}

uint8_t translate_to_ir_pairing_code(uint8_t ch)
{
	// Currently IR sends 7 bits (not 8)
	// To distinguish from other signals the MSB (of 7) shall be zero.
	// Last bit (MSB, bit number 7 AKA the 8th) does not matter when sending, 
    // it will be received as a zero though.
	// See also NOF_AVAILABLE_PAIRING_CODES
	uint8_t c = ch % PARING_CODE_MASK;
	return c;
}


void utility_memcpy(uint8_t *dst, const uint8_t *src, uint8_t n)
{
	while(n>0)
	{
		*dst = *src;
		dst++;
		src++;
		n--;
	}
}

int8_t utility_memcmp(const int8_t* ptr1, const int8_t* ptr2, uint8_t n)
{
	while(n>0)
	{
		const int8_t d = *ptr1 - *ptr2;
		if (d)
		{
			return d;
		}
		n--;
		ptr1++;
		ptr2++;
	}
	return 0;
}

uint8_t utility_strlen(const char *str)
{
	uint8_t n = 0;
	while(*str != 0)
	{
		str++;
		n++;
	}
	return n;
}

/*struct utility_struct {
	const unsigned char* data_ptr;
	uint8_t datasize_nof_bits;
	uint8_t pos;
};*/

void utility_memclr(uint8_t *ptr, uint8_t nof_bytes)
{
	while(nof_bytes>0)
	{
		*ptr = 0;
		ptr++;
		--nof_bytes;
	}
}


static inline uint8_t get_bit(const unsigned char *ptr, uint8_t pos)
{
	return(((*(ptr+(pos >> 3)))>>(7-(pos & 7)))&1);
}

static inline void set_bit(unsigned char *ptr, uint8_t pos, uint8_t data_bit)
{
	uint8_t bitpos = 7-(pos&7);
	ptr += pos >> 3;
	if (data_bit)
	{
		*ptr |= (1<<(bitpos));
	}
	else
	{
		*ptr &= ~(1<<(bitpos));
	}
}


void utility_r_init(struct utility_r_struct *us, const unsigned char* data_ptr, uint8_t nof_bits)
{
	us->data_ptr = data_ptr;
	us->datasize_nof_bits = nof_bits;
	us->pos = 0;
}

void utility_w_init(struct utility_w_struct *us, unsigned char* data_ptr, uint8_t nof_bits)
{
	us->data_ptr = data_ptr;
	us->datasize_nof_bits = nof_bits;
	us->pos = 0;
}

int16_t utility_r_nof_bits_available(const struct utility_r_struct *urs)
{
	return urs->datasize_nof_bits - urs->pos;
}


uint16_t utility_r_take16(struct utility_r_struct *urs, uint8_t nof_bits_to_take)
{
	  uint16_t d=0;

	  if (urs->pos + nof_bits_to_take > urs->datasize_nof_bits)
	  {
		  ERRLOG_P("utility", __LINE__);
		  return(0);
	  }

	  if ((urs->pos % 8) == 0)
	  {
		  while (nof_bits_to_take >= 8)
		  {
			  d = (d << 8) | (urs->data_ptr[urs->pos / 8]);
			  urs->pos += 8;
			  nof_bits_to_take -= 8;
		  }
	  }

	  while (nof_bits_to_take > 0)
	  {
		  d = (d<<1) | get_bit(urs->data_ptr, urs->pos);
		  urs->pos++;
		  nof_bits_to_take--;
	  }

	  return(d);
}

uint64_t utility_r_take64(struct utility_r_struct *us, uint8_t nof_bits_to_take)
{
	  uint64_t d=0;

	  if (us->pos + nof_bits_to_take > us->datasize_nof_bits)
	  {
		  ERRLOG_P("utility", __LINE__);
		  return(0);
	  }

	  if ((us->pos % 8) == 0)
	  {
		  while (nof_bits_to_take >= 8)
		  {
			  d = (d << 8) | (us->data_ptr[us->pos / 8]);
			  us->pos += 8;
			  nof_bits_to_take -= 8;
		  }
	  }

	  while (nof_bits_to_take > 0)
	  {
		  d = (d<<1) | get_bit(us->data_ptr, us->pos);
		  us->pos++;
		  nof_bits_to_take--;
	  }

	  return(d);
}

void utility_w_put16(struct utility_w_struct *us, uint8_t nof_bits_to_write, uint16_t d)
{
	if (us->pos + nof_bits_to_write > us->datasize_nof_bits)
	{
		ERRLOG_P("utility", __LINE__);
		return;
	}

	if ((us->pos % 8) == 0)
	{
		while (nof_bits_to_write >= 8)
		{
			us->data_ptr[us->pos / 8] = (d >> (nof_bits_to_write-8) & 0xFF);
			us->pos += 8;
			nof_bits_to_write -= 8;
		}
	}

	while (nof_bits_to_write > 0)
	{
		set_bit(us->data_ptr, us->pos, (d >> (nof_bits_to_write-1) & 1));
		us->pos++;
		nof_bits_to_write--;
	}
}

void utility_w_put64(struct utility_w_struct *us, uint8_t nof_bits_to_write, uint64_t d)
{
	if (us->pos + nof_bits_to_write > us->datasize_nof_bits)
	{
		ERRLOG_P("utility", __LINE__);
		return;
	}

	if ((us->pos % 8) == 0)
	{
		while (nof_bits_to_write >= 8)
		{
			us->data_ptr[us->pos / 8] = (d >> (nof_bits_to_write-8) & 0xFF);
			us->pos += 8;
			nof_bits_to_write -= 8;
		}
	}

	while (nof_bits_to_write > 0)
	{
		set_bit(us->data_ptr, us->pos, (d >> (nof_bits_to_write-1) & 1));
		us->pos++;
		nof_bits_to_write--;
	}
}

uint8_t utility_decode_hex(char ch)
{
	switch(ch)
	{
		case '0': return 0;
		case '1': return 1;
		case '2': return 2;
		case '3': return 3;
		case '4': return 4;
		case '5': return 5;
		case '6': return 6;
		case '7': return 7;
		case '8': return 8;
		case '9': return 9;
		case 'A': return 0xa;
		case 'B': return 0xb;
		case 'C': return 0xc;
		case 'D': return 0xd;
		case 'E': return 0xe;
		case 'F': return 0xf;
		case 'a': return 0xa;
		case 'b': return 0xb;
		case 'c': return 0xc;
		case 'd': return 0xd;
		case 'e': return 0xe;
		case 'f': return 0xf;
	}
	// Fail
	return 0xff;
}

uint8_t utility_decode_hexstr(const char* str, uint8_t *ptr, uint8_t size)
{
	uint8_t n = 0;
	for(;;)
	{
		if (n >= size)
		{
			return n;
		}

		const uint8_t ch_h = utility_decode_hex(*str);
		if (ch_h == 0xff)
		{
			return n;
		}
		++str;
		const uint8_t ch_l = utility_decode_hex(*str);
		if (ch_l == 0xff)
		{
			return n;
		}
		++str;

		*ptr = (ch_h << 4) + ch_l;
		ptr++;
		n++;
	}
}

void print_int16(int16_t i)
{
	char tmp[32];
	utility_lltoa(i, tmp, 10);
	uart_print(tmp);
}

void print_int64(int64_t i)
{
	char tmp[32];
	utility_lltoa(i, tmp, 10);
	uart_print(tmp);
}

// For values of n not being a power of 2 this will not give an even distribution
// of numbers. lower numbers will be a bit more common than higher.
// Does not matter for this application.
uint8_t utility_pseudo_random(uint8_t n)
{
	const int16_t t = avr_systime_ms_16();
	pseudo_random_counter = pseudo_random_counter * 31 + t;
	return (n == 0) ? pseudo_random_counter : (pseudo_random_counter % n);
}


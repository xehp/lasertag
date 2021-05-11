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

#include "utility.h"


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

#if 0
/*
LSB first
00  0000 0   - not used
10  0000 1   - not used
08  0001 0   + Y0
18  0001 1   - not used
04  0010 0   + B0
14  0010 1   + Y1
0C  0011 0   + B1
1C  0011 1   - not used
02  0100 0   + Y2
12  0100 1   + B2
0A  0101 0   + Y3
1A  0101 1   + B3
06  0110 0   + Y4
16  0110 1   + B4
0E  0111 0   + Y5
1E  0111 1   - not used
01  1000 0   + B5
11  1000 1   + Y6
09  1001 0   + B6
19  1001 1   + Y7
05  1010 0   + B7
15  1010 1   + Y8
0D  1011 0   + B8
1D  1011 1   + Y9
03  1100 0   + B9
13  1100 1   + b_spawn
0B  1101 0   + y_spawn
1B  1101 1   + flagA
07  1110 0   + flagB
17  1110 1   + spare
0F  1111 0   + off
1F  1111 1   - not used
*/

enum {
	blue_team = 0,
	yellow_team = 1,
};

enum {
	b_spawn = 0x13,
	y_spawn = 0x0B,
	flagA = 0x1B,
	flagB = 0x07,
	spare_code = 0x17,
	off_code = 0x0F,
};

uint8_t blue_ir_code(uint8_t n)
{
	n = n % 10;
	switch (n)
	{
		case 0: return 0x04;
		case 1: return 0x0C;
		case 2: return 0x12;
		case 3: return 0x1A;
		case 4: return 0x16;
		case 5: return 0x01;
		case 6: return 0x09;
		case 7: return 0x05;
		case 8: return 0x0D;
		case 9: return 0x03;
	}
	return 0;
}

uint8_t yellow_ir_code(uint8_t n)
{
	n = n % 10;
	switch (n)
	{
		case 0: return 0x08;
		case 1: return 0x14;
		case 2: return 0x02;
		case 3: return 0x0A;
		case 4: return 0x06;
		case 5: return 0x0E;
		case 6: return 0x11;
		case 7: return 0x19;
		case 8: return 0x15;
		case 9: return 0x1D;
	}
	return 0;
}


uint8_t translate_to_ir_code(uint8_t team, uint8_t n)
{
	switch (team)
	{
		case blue_team: return blue_ir_code(n);
		case yellow_team: return yellow_ir_code(n);
		default: break;
	}

	return 0;
}

uint8_t translate_from_ir_to_team(uint8_t code)
{
	return code;
}

uint8_t translate_from_ir_to_number(uint8_t code)
{
	return code;
}
#endif

/*
00  0000 0   - not used
10  0000 1   - not used
08  0001 0   + 0
18  0001 1   - not used
04  0010 0   + 1
14  0010 1   + 2
0C  0011 0   + 3
1C  0011 1   - not used
02  0100 0   + 4
12  0100 1   + 5
0A  0101 0   + 6
1A  0101 1   + 7
06  0110 0   + 8
16  0110 1   + 9
0E  0111 0   + 10
1E  0111 1   - not used
01  1000 0   + 11
11  1000 1   + 12
09  1001 0   + 13
19  1001 1   + 14
05  1010 0   + 15
15  1010 1   + 16
0D  1011 0   + 17
1D  1011 1   + 18
03  1100 0   + 19
13  1100 1   + 20
0B  1101 0   + 21
1B  1101 1   + 22
07  1110 0   + 23
17  1110 1   + 24
0F  1111 0   + 25
1F  1111 1   - not used

*/

uint8_t translate_to_ir_code(uint8_t n)
{
	switch(n)
	{
	case 0: return 0x08;
	case 1: return 0x04;
	case 2: return 0x14;
	case 3: return 0x0C;
	case 4: return 0x02;
	case 5: return 0x12;
	case 6: return 0x0A;
	case 7: return 0x1A;
	case 8: return 0x06;
	case 9: return 0x16;
	case 10: return 0x0E;
	case 11: return 0x01;
	case 12: return 0x11;
	case 13: return 0x09;
	case 14: return 0x19;
	case 15: return 0x05;
	case 16: return 0x15;
	case 17: return 0x0D;
	case 18: return 0x1D;
	case 19: return 0x03;
	case 20: return 0x13;
	case 21: return 0x0B;
	case 22: return 0x1B;
	case 23: return 0x07;
	case 24: return 0x17;
	case 25: return 0x0F;
	default: break;
	}
	return 0xFF;
}

uint8_t translate_from_ir_code(uint8_t c)
{
	switch(c)
	{
	case 0x08: return 0;
	case 0x04: return 1;
	case 0x14: return 2;
	case 0x0C: return 3;
	case 0x02: return 4;
	case 0x12: return 5;
	case 0x0A: return 6;
	case 0x1A: return 7;
	case 0x06: return 8;
	case 0x16: return 9;
	case 0x0E: return 10;
	case 0x01: return 11;
	case 0x11: return 12;
	case 0x09: return 13;
	case 0x19: return 14;
	case 0x05: return 15;
	case 0x15: return 16;
	case 0x0D: return 17;
	case 0x1D: return 18;
	case 0x03: return 19;
	case 0x13: return 20;
	case 0x0B: return 21;
	case 0x1B: return 22;
	case 0x07: return 23;
	case 0x17: return 24;
	case 0x0F: return 25;
	default: break;
	}
	return 0xFF;
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

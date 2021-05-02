/*
misc.c

Provide miscellaneous functions.

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

/*
utility.h

Provide miscellaneous utility functions.

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

#ifndef UTILITY_H
#define UTILITY_H

#include <stdint.h>
#include <ctype.h>

// division rounded (at least for positive numbers), be aware of overflow
#define DIV(a,b) (((a)+((b)/2))/(b))

// division rounded up (at least for positive numbers), be aware of overflow
#define DIV_UP(a,b) (((a)+((b)-1))/(b))

// division rounded down (at least for positive numbers), be aware of overflow
#define DIV_DOWN(a,b) ((a)/(b))


// NOTE! buffer must be large enough! 32 bytes should do, not less.
char* utility_lltoa(int64_t num, char* str, int base);

int64_t utility_atoll(const char* str);

uint8_t translate_to_ir_code(uint8_t n);
uint8_t translate_from_ir_code(uint8_t c);
uint8_t translate_to_ir_pairing_code(uint8_t ch);

void utility_memcpy(uint8_t *dst, const uint8_t *src, uint8_t n);
int8_t utility_memcmp(const int8_t* ptr1, const int8_t* ptr2, uint8_t n);

uint8_t utility_strlen(const char *str);

void utility_memclr(uint8_t *ptr, uint8_t nof_bytes);

struct utility_r_struct {
	const unsigned char* data_ptr;
	uint8_t datasize_nof_bits;
	uint8_t pos;
};

void utility_r_init(struct utility_r_struct *urs, const unsigned char* data_ptr, uint8_t nof_bits);
uint16_t utility_r_take16(struct utility_r_struct *urs, uint8_t nof_bits);
uint64_t utility_r_take64(struct utility_r_struct *urs, uint8_t nof_bits);
int16_t utility_r_nof_bits_available(const struct utility_r_struct *urs);

struct utility_w_struct {
	unsigned char* data_ptr;
	uint8_t datasize_nof_bits;
	uint8_t pos;
};

void utility_w_init(struct utility_w_struct *uws, unsigned char* data_ptr, uint8_t nof_bits);
void utility_w_put16(struct utility_w_struct *uws, uint8_t nof_bits, uint16_t d);
void utility_w_put64(struct utility_w_struct *uws, uint8_t nof_bits, uint64_t d);

uint8_t utility_decode_hexstr(const char* str, uint8_t *ptr, uint8_t size);

void print_int16(int16_t i);
void print_int64(int64_t i);

uint8_t utility_pseudo_random(uint8_t n);

#endif

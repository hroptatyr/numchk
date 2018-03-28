/*** damm.c -- generic damm checker
 *
 * Copyright (C) 2014-2018 Sebastian Freundt
 *
 * Author:  Sebastian Freundt <freundt@ga-group.nl>
 *
 * This file is part of numchk.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the author nor the names of any contributors
 *    may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ***/
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "numchk.h"
#include "nifty.h"

static const uint_fast8_t damm10[10U][10U] = {
	[0] = {0, 3, 1, 7, 5, 9, 8, 6, 4, 2},
	[1] = {7, 0, 9, 2, 1, 5, 4, 8, 6, 3},
	[2] = {4, 2, 0, 6, 8, 7, 1, 3, 5, 9},
	[3] = {1, 7, 5, 0, 9, 8, 3, 4, 2, 6},
	[4] = {6, 1, 2, 3, 0, 4, 5, 9, 7, 8},
	[5] = {3, 6, 7, 4, 2, 0, 9, 5, 8, 1},
	[6] = {5, 8, 6, 9, 7, 2, 0, 1, 3, 4},
	[7] = {8, 9, 4, 5, 3, 6, 2, 0, 1, 7},
	[8] = {9, 4, 3, 8, 6, 1, 7, 2, 0, 5},
	[9] = {2, 5, 8, 1, 4, 3, 6, 7, 9, 0},
};

static const uint_fast8_t damm16[16U][16U] = {
	[0] = {0, 2, 4, 6, 8, 10, 12, 14, 3, 1, 7, 5, 11, 9, 15, 13},
	[1] = {2, 0, 6, 4, 10, 8, 14, 12, 1, 3, 5, 7, 9, 11, 13, 15},
	[2] = {4, 6, 0, 2, 12, 14, 8, 10, 7, 5, 3, 1, 15, 13, 11, 9},
	[3] = {6, 4, 2, 0, 14, 12, 10, 8, 5, 7, 1, 3, 13, 15, 9, 11},
	[4] = {8, 10, 12, 14, 0, 2, 4, 6, 11, 9, 15, 13, 3, 1, 7, 5},
	[5] = {10, 8, 14, 12, 2, 0, 6, 4, 9, 11, 13, 15, 1, 3, 5, 7},
	[6] = {12, 14, 8, 10, 4, 6, 0, 2, 15, 13, 11, 9, 7, 5, 3, 1},
	[7] = {14, 12, 10, 8, 6, 4, 2, 0, 13, 15, 9, 11, 5, 7, 1, 3},
	[8] = {3, 1, 7, 5, 11, 9, 15, 13, 0, 2, 4, 6, 8, 10, 12, 14},
	[9] = {1, 3, 5, 7, 9, 11, 13, 15, 2, 0, 6, 4, 10, 8, 14, 12},
	[10] = {7, 5, 3, 1, 15, 13, 11, 9, 4, 6, 0, 2, 12, 14, 8, 10},
	[11] = {5, 7, 1, 3, 13, 15, 9, 11, 6, 4, 2, 0, 14, 12, 10, 8},
	[12] = {11, 9, 15, 13, 3, 1, 7, 5, 8, 10, 12, 14, 0, 2, 4, 6},
	[13] = {9, 11, 13, 15, 1, 3, 5, 7, 10, 8, 14, 12, 2, 0, 6, 4},
	[14] = {15, 13, 11, 9, 7, 5, 3, 1, 12, 14, 8, 10, 4, 6, 0, 2},
	[15] = {13, 15, 9, 11, 5, 7, 1, 3, 14, 12, 10, 8, 6, 4, 2, 0},
};

static __attribute__((pure, const)) uint_fast8_t
_chex(char c)
{
	if (LIKELY((unsigned char)(c ^ '0') < 10)) {
		return (uint_fast8_t)(c ^ '0');
	} else if ((unsigned char)((c | 0x20) - 'W') < 16){
		return (uint_fast8_t)((c | 0x20) - 'W');
	}
	/* no error code */
	return (uint_fast8_t)-1;
}


nmck_t
nmck_damm10(const char *str, size_t len)
{
	uint_fast8_t prod = 0U;

	if (len < 2U) {
		/* no need to check single digits, nor no digits */
		return -1;
	}

	for (size_t i = 0U; i < len; i++) {
		uint_fast8_t c = (unsigned char)(str[i] ^ '0');

		if (UNLIKELY(c >= 10U)) {
			return -1;
		}
		prod = damm10[prod][c];
	}

	return !prod - 1;
}

void
nmpr_damm10(nmck_t s, const char *UNUSED(str), size_t UNUSED(len))
{
	if (!s) {
		fputs("Damm (dec) secured number, check passed", stdout);
	}
	return;
}

nmck_t
nmck_damm16(const char *str, size_t len)
{
	uint_fast8_t prod = 0U;

	if (len < 2U) {
		/* no need to check single digits, nor no digits */
		return -1;
	}

	for (size_t i = 0U; i < len; i++) {
		uint_fast8_t c = _chex(str[i]);

		if (UNLIKELY(c >= 16U)) {
			return -1;
		}
		prod = damm16[prod][c];
	}

	return !prod - 1;
}

void
nmpr_damm16(nmck_t s, const char *UNUSED(str), size_t UNUSED(len))
{
	if (!s) {
		fputs("Damm (hex) secured number, check passed", stdout);
	}
	return;
}
/* damm.c ends here */

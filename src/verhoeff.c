/*** verhoeff.c -- generic verhoeff checker
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

static const uint_fast8_t d5[10U][10U] = {
	[0] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9},
	[1] = {1, 2, 3, 4, 0, 6, 7, 8, 9, 5},
	[2] = {2, 3, 4, 0, 1, 7, 8, 9, 5, 6},
	[3] = {3, 4, 0, 1, 2, 8, 9, 5, 6, 7},
	[4] = {4, 0, 1, 2, 3, 9, 5, 6, 7, 8},
	[5] = {5, 9, 8, 7, 6, 0, 4, 3, 2, 1},
	[6] = {6, 5, 9, 8, 7, 1, 0, 4, 3, 2},
	[7] = {7, 6, 5, 9, 8, 2, 1, 0, 4, 3},
	[8] = {8, 7, 6, 5, 9, 3, 2, 1, 0, 4},
	[9] = {9, 8, 7, 6, 5, 4, 3, 2, 1, 0},
};

static const uint_fast8_t perm[8U][10U] = {
	[0] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9},
	[1] = {1, 5, 7, 6, 2, 8, 3, 0, 9, 4},
	[2] = {5, 8, 0, 3, 7, 9, 6, 1, 4, 2},
	[3] = {8, 9, 1, 6, 0, 4, 3, 5, 2, 7},
	[4] = {9, 4, 5, 3, 1, 2, 6, 8, 7, 0},
	[5] = {4, 2, 8, 6, 5, 7, 3, 9, 0, 1},
	[6] = {2, 7, 9, 3, 8, 0, 6, 4, 1, 5},
	[7] = {7, 0, 4, 6, 9, 1, 3, 2, 5, 8},
};


nmck_t
nmck_verhoeff(const char *str, size_t len)
{
	uint_fast8_t prod = 0U;

	for (size_t i = len, j = 0U; i > 0U; i--) {
		uint_fast8_t c = (unsigned char)(str[i - 1U] ^ '0');
		uint_fast8_t f;

		if (UNLIKELY(c >= 10U)) {
			return -1;
		}
		f = perm[j++ % 8U][c];
		prod = d5[prod][f];
	}

	return !prod - 1;
}

void
nmpr_verhoeff(nmck_t s, const char *UNUSED(str), size_t UNUSED(len))
{
	if (!s) {
		fputs("Verhoeff secured number, check passed", stdout);
	}
	return;
}

/* verhoeff.c ends here */

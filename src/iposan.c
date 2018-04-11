/*** iposan.c -- checker for IPOS registration numbers
 *
 * Copyright (C) 2017-2018 Sebastian Freundt
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
/* these are the new IPOS registration numbers,
 * see https://blog.cantab-ip.com/2014/01/20/new-format-for-singapore-ip-application-numbers-at-ipos/ */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include "numchk.h"
#include "nifty.h"

static const uint_fast8_t damm[10U][10U] = {
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

#ifdef RAGEL_BLOCK
%%{
	machine numchk;

	iposan = ("10" | "11" | "20" | "30" | "40") digit{4} digit{5} (/[P-Y]/ | check) ("-" digit{2})? %{c(iposan)} ;
}%%
#endif	/* RAGEL_BLOCK */


nmck_t
nmck_iposan(const char *str, size_t len)
{
/* this is Damm */
	uint_fast8_t prod = 0U;

	if (UNLIKELY(len < 12U || len > 15U)) {
		return -1;
	}

	with (uint_fast32_t c = *str ^ '0') {
		switch (c) {
		case 1U:
		case 2U:
		case 3U:
		case 4U:
			break;
		default:
			return -1;
		}
	}
	for (size_t i = 2U; i < 11U; i++) {
		uint_fast8_t c = (unsigned char)(str[i] ^ '0');

		if (UNLIKELY(c >= 10U)) {
			return -1;
		}
		prod = damm[prod][c];
	}
	prod ^= 'P';

	return prod << 1U ^ ((char)prod != str[11U]);
}

void
nmpr_iposan(nmck_t s, const char *str, size_t len)
{
	if (!(s & 0b1U)) {
		fputs("IPOS application number, conformant", stdout);
	} else if (s > 0 && len >= 12U) {
		fputs("IPOS application number, not conformant, should be ", stdout);
		fwrite(str, sizeof(*str), 11U, stdout);
		fputc(s >> 1 & 0x7f, stdout);
		fwrite(str + 12U, sizeof(*str), len - 12U, stdout);
	} else {
		fputs("unknown", stdout);
	}
	return;
}

/* iposan.c ends here */

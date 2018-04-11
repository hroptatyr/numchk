/*** figi.c -- checker for FIGIs
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
#include <assert.h>
#include "numchk.h"
#include "nifty.h"

#ifdef RAGEL_BLOCK
%%{
	machine numchk;

	figi = "BBG" (consonant | digit){8} (digit | check) %{c(figi)} ;
}%%
#endif	/* RAGEL_BLOCK */


nmck_t
nmck_figi(const char *str, size_t len)
{
	uint_fast32_t sum = 0U;

	if (len != 12U) {
		return -1;
	} else if (str[0U] != 'B' || str[1U] != 'B' || str[2U] != 'G') {
		/* currently only BB is registered as certified provider */
		return -1;
	}

	/* use the left 11 digits */
	for (size_t i = 0U; i < 11U; i++) {
		uint_fast32_t d;

		switch (str[i]) {
		case '0' ... '9':
			d = (str[i] ^ '0');
			break;
		case 'B':
		case 'C':
		case 'D':
		case 'F':
		case 'G':
		case 'H':
		case 'J':
		case 'K':
		case 'L':
		case 'M':
		case 'N':
		case 'P':
		case 'Q':
		case 'R':
		case 'S':
		case 'T':
		case 'V':
		case 'W':
		case 'X':
		case 'Y':
		case 'Z':
			d = 10U + (str[i] - 'A');
			break;
		default:
			return -1;
		}

		/* double every other */
		d <<= (i % 2U);
		sum += (d / 10U) + (d % 10U);
	}
	sum = 700U - sum;
	sum %= 10U;
	sum ^= '0';

	return sum << 1U ^ ((char)sum != str[len - 1U]);
}

void
nmpr_figi(nmck_t s, const char *str, size_t len)
{
	if (!(s & 0b1U)) {
		fputs("FIGI, conformant with http://www.omg.org/spec/FIGI/1.0", stdout);
	} else if (s > 0 && len == 12U) {
		fputs("FIGI, not conformant, should be ", stdout);
		fwrite(str, sizeof(*str), 11U, stdout);
		fputc(s >> 1U & 0x7fU, stdout);
	} else {
		fputs("unknown", stdout);
	}
	return;
}

/* figi.c ends here */

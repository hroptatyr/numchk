/*** vin.c -- checker for vehicle identification numbers
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
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "numchk.h"
#include "nifty.h"


nmck_t
nmck_vin(const char *str, size_t len)
{
	static uint_fast8_t t[] = " 12345678 12345 7 923456789";
	uint_fast32_t sum = 0U;

	/* common cases first */
	if (len != 17U) {
		return -1;
	}

	for (size_t i = 0; i < 8U; i++) {
		uint_fast32_t c = str[i];

		if (c >= 'A' && c <= 'Z' ||
		    c >= 'a' && c <= 'z') {
			c = t[c & 0x1fU];
		} else if ((c ^ '0') >= 10U) {
			return -1;
		}
		c ^= '0';
		sum += c * (8U - i + 9U * (i >= 7U));
	}

	for (size_t i = 9; i < 17U; i++) {
		uint_fast32_t c = str[i];

		if (c >= 'A' && c <= 'Z' ||
		    c >= 'a' && c <= 'z') {
			c = t[c & 0x1fU];
		} else if ((c ^ '0') >= 10U) {
			return -1;
		}
		c ^= '0';
		sum += c * (18U - i);
	}
	sum %= 11U;
	/* digits remain digits, 10 -> X */
	sum ^= sum < 10U ? '0' : 'R';

	return sum << 1U ^ (str[8U] != (char)sum);
}

void
nmpr_vin(nmck_t s, const char *str, size_t len)
{
	if (LIKELY(!(s & 0b1U))) {
		fputs("VIN, conformant", stdout);
	} else if (s > 0 && len > 8U) {
		fputs("VIN, not conformant, should be ", stdout);
		fwrite(str, sizeof(*str), 8U, stdout);
		fputc(s >> 1U & 0x7FU, stdout);
		fwrite(str + 9U, sizeof(*str), len - 9U, stdout);
	} else {
		fputs("unknown", stdout);
	}
	return;
}

/* vin.c ends here */

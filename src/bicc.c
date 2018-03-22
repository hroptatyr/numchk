/*** bicc.c -- checker for ISO 6346 BIC codes
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
#include <stdint.h>
#include <assert.h>
#include "numchk.h"
#include "nifty.h"

static uint_fast32_t
_calph(char x)
{
	if ((unsigned char)(x ^ '0') < 10U) {
		return (unsigned char)(x ^ '0');
	}
	switch (x) {
	case '0' ... '9':
		return x ^ '0';
	case 'V' ... 'Z':
		x++;
	case 'L' ... 'U':
		x++;
	case 'B' ... 'K':
		x++;
	case 'A':
		return x - 55U;
	default:
		break;
	}
	return -1;
}


nmck_t
nmck_bicc(const char *str, size_t len)
{
	uint_fast32_t sum = 0U;
	uint_fast32_t w = 1U;

	if (UNLIKELY(len != 11U)) {
		return -1;
	}

	/* expand the left 11 digits */
	for (size_t i = 0U; i < 4U; i++, w *= 2U) {
		uint_fast32_t c = _calph(str[i]);

		if (UNLIKELY(c > 38U)) {
			return -1;
		}
		sum += c * w;
	}
	for (size_t i = 4U; i < 10U; i++, w *= 2U) {
		uint_fast32_t c = (unsigned char)(str[i] ^ '0');

		if (UNLIKELY(c >= 10U)) {
			return -1;
		}
		sum += c * w;
	}
	sum %= 11U;
	sum %= 10U;
	sum ^= '0';
	return sum << 1U ^ sum != (unsigned char)str[10U];
}

void
nmpr_bicc(nmck_t s, const char *str, size_t len)
{
	if (!(s & 0b1U)) {
		fputs("BICC, conformant with ISO 6346:1995", stdout);
	} else if (s > 0 && len == 11U) {
		fputs("BICC, not ISO 6346 conformant, should be ", stdout);
		fwrite(str, sizeof(*str), 10U, stdout);
		fputc(s >> 1 & 0x7f, stdout);
	} else {
		fputs("unknown", stdout);
	}
	return;
}

/* bicc.c ends here */

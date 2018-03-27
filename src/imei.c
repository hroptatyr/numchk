/*** imei.c -- checker for IMEIs
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


nmck_t
nmck_imei(const char *str, size_t len)
{
/* Luhn */
	uint_fast32_t sum = 0U;
	uint_fast32_t w = 1U;
	size_t j = 0U;

	if (UNLIKELY(len < 14U || len > 18U)) {
		return -1;
	}

	for (size_t i = 0U; i < len - 1U; i++) {
		uint_fast32_t c = (unsigned char)(str[i] ^ '0');

		if (str[i] == '-') {
			continue;
		} else if (UNLIKELY(c >= 18U)) {
			return -1;
		}
		with (uint_fast32_t x = c * w) {
			sum += x;
			sum += (x >= 10U);
		}
		w ^= 3U;
		j++;
	}
	if (j > 14U) {
		return 0U;
	} else {
		sum %= 10U;
		sum = 10 - sum;
		sum %= 10U;
		sum ^= '0';
	}
	return sum << 1U ^ ((char)sum != str[len - 1U]);
}

void
nmpr_imei(nmck_t s, const char *str, size_t len)
{
	if (!(s & 0b1U) && (s >> 1U)) {
		fputs("IMEI, GSMA conformant", stdout);
	} else if (s == 0U) {
		fputs("IMEISV, GSMA conformant", stdout);
	} else if (s > 1U) {
		fputs("IMEI, not GSMA conformant, should be ", stdout);
		fwrite(str, sizeof(*str), len - 1U, stdout);
		fputc(s >> 1 & 0x7f, stdout);
	} else if (s == 1U) {
		fputs("IMEISV, not GSMA conformant", stdout);
	} else {
		fputs("unknown", stdout);
	}
	return;
}

/* imei.c ends here */

/*** kennitala.c -- checker for Iceland's kennitolur
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
nmck_kennitala(const char *str, size_t len)
{
	uint_fast32_t sum = 0U;
	size_t i = 0U;

	if (UNLIKELY(len < 10U || len > 11U)) {
		return -1;
	}

	if ((unsigned char)(str[i] ^ '0') > 7U) {
		/* birthdays are 0 to 3 and 4 to 7 */
		return -1;
	}
	/* birthdate first */
	for (; i < 6U; i++) {
		uint_fast32_t c = (unsigned char)(str[i] ^ '0');

		if (UNLIKELY(c > 10U)) {
			return -1;
		}
		/* weights are 3 2 7 6 5 4 */
		sum += c * ((7 - i) % 6U + 2U);
	}
	/* optional hyphen */
	i += str[i] == '-';

	if ((unsigned char)(str[i] ^ '0') < 2U) {
		/* random section starts at 20 */
		return -1;
	}
	for (uint_fast32_t w = 3U; i < len && w > 1U; i++, w--) {
		uint_fast32_t c = (unsigned char)(str[i] ^ '0');

		if (UNLIKELY(c >= 10U)) {
			return -1;
		}
		sum += c * w;
	}
	with (uint_fast32_t c = (unsigned char)(str[i + 1U] ^ '0')) {
		if (c && c < 8U || c >= 10U) {
			/* final digit must be 8, 9 or 0 */
			return -1;
		}
	}
	if (UNLIKELY((sum %= 11U) == 10U)) {
		/* apparently mustn't happen */
		return -1;
	} else if (sum) {
		sum = 11U - sum;
	}
	sum ^= '0';
	return sum << 1U ^ ((char)sum != str[i]);
}

void
nmpr_kennitala(nmck_t s, const char *str, size_t len)
{
	if (!(s & 0b1U)) {
		fputs("kennitala, conformant", stdout);
	} else if (s > 0 && len > 2U) {
		fputs("kennitala, not conformant, should be ", stdout);
		fwrite(str, sizeof(*str), len - 2U, stdout);
		fputc(s >> 1 & 0x7f, stdout);
		fputc(str[len - 1U], stdout);
	} else {
		fputs("unknown", stdout);
	}
	return;
}

/* kennitala.c ends here */

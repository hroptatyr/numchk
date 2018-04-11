/*** ppsn.c -- checker for National Provider Identifiers
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

#ifdef RAGEL_BLOCK
%%{
	machine numchk;

	ppsn = digit{7} (upper | check) ("/"? /[A-Z ]/)? %{c(ppsn)} ;
}%%
#endif	/* RAGEL_BLOCK */


nmck_t
nmck_ppsn(const char *str, size_t len)
{
	uint_fast32_t sum = 0U;

	if (UNLIKELY(len < 8U || len > 10U)) {
		return -1;
	}

	/* process alpha part */
	for (size_t i = 0U; i < 7U; i++) {
		uint_fast32_t c = (unsigned char)(str[i] ^ '0');

		if (UNLIKELY(c >= 10U)) {
			return -1;
		}
		sum += (8 - i) * c;
	}
	if (len > 8U) {
		size_t i = 8U;
		i += str[i] == '/';
		if (str[i] >= 'A' && str[i] < 'W') {
			sum += 9U * (str[i] - '@');
		}
	}
	sum %= 23U;
	sum += '@';

	return sum << 1U ^ ((char)sum != str[7U]);
}

void
nmpr_ppsn(nmck_t s, const char *str, size_t len)
{
	if (!(s & 0b1U)) {
		fputs("PPS No, conformant", stdout);
	} else if (s > 0 && len > 7U) {
		fputs("PPS No, not conformant, should be ", stdout);
		fwrite(str, sizeof(*str), 7U, stdout);
		fputc(s >> 1 & 0x7f, stdout);
		fwrite(str + 8U, sizeof(*str), len - 8U, stdout);
	} else {
		fputs("unknown", stdout);
	}
	return;
}

/* ppsn.c ends here */

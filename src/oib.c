/*** oib.c -- checker for Croatian OIBs
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
nmck_oib(const char *str, size_t len)
{
/* mod-11,10 */
	uint_fast32_t sum = 0U;
	size_t i = 0U;

	if (UNLIKELY(len < 11U || len > 13U)) {
		return -1;
	}
	i += str[0U] == 'H' && str[1U] == 'R';	
	i += str[0U] == 'H' && str[1U] == 'R';	
	for (size_t j = 0U; j < 10U && i < len; i++) {
		uint_fast32_t c = (str[i] ^ '0');

		if (UNLIKELY(c >= 10U)) {
			return -1;
		}
		sum += c;
		sum = (sum + 9U/*-1 mod 10*/) % 10U + 1U;
		sum *= 2U;
		sum %= 11U;
		j++;
	}
	sum = (11U - sum) % 10U;
	sum ^= '0';

	return sum << 1U ^ ((char)sum != str[len - 1U]);
}

void
nmpr_oib(nmck_t s, const char *str, size_t len)
{
	if (!(s & 0b1U)) {
		fputs("OIB, conformant", stdout);
	} else if (s > 0 && len > 1U) {
		fputs("OIB, not conformant, should be ", stdout);
		fwrite(str, sizeof(*str), len - 1U, stdout);
		fputc(s >> 1 & 0x7f, stdout);
	} else {
		fputs("unknown", stdout);
	}
	return;
}

/* oib.c ends here */

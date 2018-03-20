/*** grid.c -- checker for vehicle identification numbers
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

static inline __attribute__((pure, const)) unsigned char
_cb36(char c)
{
	if (LIKELY((unsigned char)(c ^ '0') < 10U)) {
		return (unsigned char)(c ^ '0');
	}
	/* no check for the upper bound of c */
	return (unsigned char)(c - '7');
}

static inline __attribute__((pure, const)) char
_b36c(char c)
{
	if (LIKELY((unsigned char)c < 10U)) {
		return (char)(c ^ '0');
	}
	/* no check for the upper bound of c */
	return (char)(c + '7');
}


nmck_t
nmck_grid(const char *str, size_t len)
{
	uint_fast32_t sum = 36U;
	unsigned char chk;
	char stc;
	size_t i;

	for (size_t j = i = 0U; j < 17U && i < len; i++) {
		uint_fast32_t c;
		if (str[i] == '-') {
			continue;
		} else if ((c = _cb36(str[i])) >= 36U) {
			return -1;
		}
		if ((sum += c) > 36U) {
			sum -= 36U;
		}
		sum *= 2U;
		sum %= 37U;
		j++;
	}
	if (str[i] == '-') {
		i++;
	}
	chk = _b36c(37U - sum);
	stc = str[i++];

	if (i < len) {
		return -1;
	}

	return chk << 1U | (chk != stc);
}

void
nmpr_grid(nmck_t s, const char *str, size_t len)
{
	if (LIKELY(!(s & 0b1U))) {
		fputs("GRid, conformant", stdout);
	} else if (s > 0 && len > 8U) {
		fputs("GRid, not conformant, should be ", stdout);
		fwrite(str, sizeof(*str), len - 1U, stdout);
		fputc(s >> 1U & 0x7FU, stdout);
	} else {
		fputs("unknown", stdout);
	}
	return;
}

/* grid.c ends here */

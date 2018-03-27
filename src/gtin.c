/*** gtin.c -- checker for GS1's GTINs
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

typedef enum {
	GS1_UNK,
	GTIN8,
	GTIN12,
	GTIN13,
	GTIN14,
	GSIN,
	SSCC,
	GS1_NTYPES,
} gs1_type_t;

static const char *types[] = {
	[GS1_UNK] = "GS1",
	[GTIN8] = "GTIN8",
	[GTIN12] = "GTIN12",
	[GTIN13] = "GTIN13",
	[GTIN14] = "GTIN14",
	[GSIN] = "GSIN",
	[SSCC] = "SSCC",
};


nmck_t
nmck_gtin(const char *str, size_t len)
{
/* weights are ... , 1, 3, 1, 3, 1, mod 10
 * subtracting 10 is ..., 9, 7, 9, 7, 9 */
	uint_fast32_t s1 = 0U, s2 = 0U;
	gs1_type_t t = GS1_UNK;

	switch (len) {
	case 8U:
		t++;
	case 12U:
		t++;
	case 13U:
		t++;
	case 14U:
		t++;
	case 17U:
		t++;
	case 18U:
		t++;
		break;
	default:
		return -1;
	}

	/* use the left len - 1 digits, start with  the odds */
	for (size_t i = !(len % 2U); i < len - 1U; i += 2U) {
		uint_fast32_t c = str[i] ^ '0';

		if (UNLIKELY(c >= 10U)) {
			return -1;
		}
		s1 += c;
	}
	/* this is the weight 9 */
	s1 *= 9U;

	/* evens now */
	for (size_t i = len % 2U; i < len - 1U; i += 2U) {
		uint_fast32_t c = str[i] ^ '0';

		if (UNLIKELY(c >= 10U)) {
			return -1;
		}
		s2 += c;
	}
	/* this is the weight 7 */
	s2 *= 7U;

	s1 += s2;
	s1 %= 10U;
	s1 ^= '0';
	return (s1 << 8U ^ (GS1_NTYPES - t)) << 1U ^ ((char)s1 != str[len - 1U]);
}

void
nmpr_gtin(nmck_t s, const char *str, size_t len)
{
	gs1_type_t t = (gs1_type_t)(s >> 1U & 0xfU);

	if (UNLIKELY(s < 0 || !t || t >= GS1_NTYPES)) {
		fputs("unknown", stdout);
		return;
	}

	fputs(types[t], stdout);
	if (LIKELY(!(s & 0b1U))) {
		fputs(", conformant", stdout);
	} else if (s > 0 && len > 0) {
		fputs(", not conformant, should be ", stdout);
		fwrite(str, sizeof(*str), len - 1U, stdout);
		fputc(s >> 8U >> 1U & 0x7fU, stdout);
	}
	return;
}

/* gtin.c ends here */

/*** isin.c -- checker for ISO 6166 security idenfitication numbers
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
#include <stdint.h>
#include <assert.h>
#include "numchk.h"
#include "nifty.h"

/* allowed isin country codes */
#include "isin-cc.c"

#ifdef RAGEL_BLOCK
%%{
	machine numchk;

	isin = upper{2} upnum{9} (digit | check) %{c(isin)} ;
}%%
#endif	/* RAGEL_BLOCK */


nmck_t
nmck_isin(const char *str, size_t len)
{
	uint_fast8_t buf[24U];
	size_t bsz = 0U;
	/* for the luhn check */
	uint_fast32_t dbl[2U] = {0U, 0U};
	uint_fast32_t one[2U] = {0U, 0U};
	uint_fast32_t sum;
	size_t k;

	if (UNLIKELY(len != 12U)) {
		return -1;
	} else if (!valid_cc_p(str)) {
		return -1;
	}

	/* expand the left 11 digits */
	for (size_t i = 0U; i < 11U; i++) {
		switch (str[i]) {
		case '0' ... '9':
			buf[bsz++] = (unsigned char)(str[i] ^ '0');
			break;
		case 'A' ... 'J':
			buf[bsz++] = 1U;
			buf[bsz++] = (unsigned char)(str[i] - 'A');
			break;
		case 'K' ... 'T':
			buf[bsz++] = 2U;
			buf[bsz++] = (unsigned char)(str[i] - 'K');
			break;
		case 'U' ... 'Z':
			buf[bsz++] = 3U;
			buf[bsz++] = (unsigned char)(str[i] - 'U');
			break;
		default:
			return -1;
		}
	}

	for (size_t i = k = 0U; i < bsz; i++, k ^= 1U) {
		uint_fast32_t c = 2U * buf[i];
		dbl[k] += c;
		one[k] += c >= 10U;
	}
	/* decide now which sum was the 2-weighted one */
	sum = dbl[k] / 2U + dbl[k ^ 1U] + one[k ^ 1U];
	sum = 10000U - sum;
	sum %= 10;
	sum ^= '0';

	return sum << 1U ^ ((char)sum != str[len - 1U]);
}

void
nmpr_isin(nmck_t s, const char *sr, size_t len)
{
	if (!(s & 0b1U)) {
		fputs("ISIN, conformant with ISO 6166:2013", stdout);
	} else if (s > 0 && len == 12U) {
		fputs("ISIN, not ISO 6166 conformant, should be ", stdout);
		fwrite(sr, sizeof(*sr), 11U, stdout);
		fputc(s >> 1 & 0x7fU, stdout);
	} else {
		fputs("unknown", stdout);
	}
	return;
}

/* isin.c ends here */

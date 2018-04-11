/*** isni.c -- checker for ISNIs
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

#ifdef RAGEL_BLOCK
%%{
	machine numchk;

	isni = digit{4} " "? digit{4} " "? digit{4} " "? digit{3} (digit | "X" | check) %{c(isni)} ;
}%%
#endif	/* RAGEL_BLOCK */


nmck_t
nmck_isni(const char *str, size_t len)
{
/* calculate the check digit, mod 11-2 */
	uint_fast32_t sum = 0U;

	/* common cases first */
	if (len < 16U || len > 19U) {
		return -1;
	}

	for (size_t i = 0U, j = 0U; j < 15U && i < len - 1U; i++) {
		uint_fast32_t c = (unsigned char)(str[i] ^ '0');

		if (str[i] == ' ') {
			continue;
		} else if (UNLIKELY(c >= 10U)) {
			return -1;
		}
		sum += c;
		sum *= 2U;
		j++;
	}
	/* sum + last digit would be 1 mod 11 */
	sum = 1358017U/*==12 mod 11*/ - sum;
	sum %= 11U;
	sum ^= sum < 10U ? '0' : 'R'/*0xA^'X'*/;
	return sum << 1U ^ ((char)sum != str[len - 1U]);
}

void
nmpr_isni(nmck_t s, const char *str, size_t len)
{
	if (LIKELY(!(s & 0b1U))) {
		fputs("ISNI, conformant with ISO 27729:2012", stdout);
	} else if (s > 0 && len > 0) {
		fputs("ISNI, not ISO 27729:2012 conformant, should be ", stdout);
		fwrite(str, sizeof(*str), len - 1U, stdout);
		fputc(s >> 1U & 0x7fU, stdout);
	} else {
		fputs("unknown", stdout);
	}
	return;
}

/* isni.c ends here */

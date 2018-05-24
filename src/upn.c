/*** upn.c -- unique pupil numbers
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

	upn = (upper | check) digit{12} %{c(upn)} ;
}%%
#endif	/* RAGEL_BLOCK */


nmck_t
nmck_upn(const char *str, size_t len)
{
	uint_fast8_t l[] = "ABCDEFGHJKLMNPQRTUVWXYZ";
	uint_fast32_t sum = 0U;
	uint_fast32_t c;

	/* common cases first */
	if (len != 13U) {
		return -1;
	}

	for (size_t i = 1U; i < 13U; i++) {
		c = str[i] ^ '0';

		if (UNLIKELY(c >= 10U)) {
			return -1;
		}
		sum += c * (i + 1U);
	}
	sum %= 23U;
	c = l[sum];

	return c << 1U ^ (str[0U] != (char)c);
}

void
nmpr_upn(nmck_t s, const char *str, size_t len)
{
	if (LIKELY(!(s & 0b1U))) {
		fputs("UPN, conformant", stdout);
	} else if (s > 0 && len > 1U) {
		fputs("UPN, not conformant, should be ", stdout);
		fputc(s >> 1U & 0x7fU, stdout);
		fwrite(str + 1U, sizeof(*str), len - 1U, stdout);
	} else {
		fputs("unknown", stdout);
	}
	return;
}

/* upn.c ends here */

/*** imo.c -- checker for ATO's tax file number
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
#include <assert.h>
#include "numchk.h"
#include "nifty.h"

#ifdef RAGEL_BLOCK
%%{
	machine numchk;

	imo = ("IMO" " "?)? digit{6} (digit | check) %{c(imo)} ;
}%%
#endif	/* RAGEL_BLOCK */


nmck_t
nmck_imo(const char *str, size_t len)
{
	uint_fast32_t sum = 0U;
	size_t of = 0U;

	/* common cases first */
	if (len < 7U || len > 11U) {
		return -1;
	}

	/* skip IMO ? */
	of += str[of] == 'I';
	of += str[of] == 'M';
	of += str[of] == 'O';
	of += str[of] == ' ';

	if (7U + of < len) {
		/* too many digits? */
		return -1;
	}
	for (size_t i = 0; i < 6U; i++) {
		uint_fast32_t c = str[of + i] ^ '0';

		if (UNLIKELY(c >= 10U)) {
			return -1;
		}
		sum += (7 - i) * c;
	}
	sum %= 10U;
	sum ^= '0';
	return sum << 1U ^ (str[of + 6U] != (char)sum);
}

void
nmpr_imo(nmck_t s, const char *str, size_t len)
{
	if (LIKELY(!(s & 0b1U))) {
		fputs("IMO, conformant", stdout);
	} else if (s > 0 && len > 0) {
		fputs("IMO, not conformant, should be ", stdout);
		fwrite(str, sizeof(*str), len - 1U, stdout);
		fputc(s >> 1U & 0x7FU, stdout);
	} else {
		fputs("unknown", stdout);
	}
	return;
}

/* imo.c ends here */

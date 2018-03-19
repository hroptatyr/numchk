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

typedef union {
	nmck_t s;
	struct {
		unsigned char pad;
		unsigned char chk;
	};
} isni_state_t;

static isni_state_t
calc_isni(const char *str, size_t len)
{
/* calculate the check digit, mod 11-2 */
	unsigned int sum = 0U, wgt = 2U;
	size_t i, j;

	for (j = 0U, i = len - 1U; j < 15U && i-- > 0;) {
		if ((unsigned char)str[i] <= ' ') {
			continue;
		} else if (UNLIKELY((unsigned char)(str[i] ^ '0') >= 10)) {
			return (isni_state_t){0};
		}
		sum += wgt * (unsigned char)(str[i] ^ '0');
		wgt *= 2U;
		wgt %= 11U;
		j++;
	}
	/* sum + last digit would be 1 mod 11 */
	sum--;
	sum = sum < 10 ? sum ^ '0' : 'X';
	return (isni_state_t){.chk = (unsigned char)sum};
}


nmck_t
nmck_isni(const char *str, size_t len)
{
	/* common cases first */
	if (len < 16U || len > 19U) {
		return -1;
	}

	with (isni_state_t st = calc_isni(str, len)) {
		if (!st.s) {
			return -1;
		} else if (st.chk != str[len - 1U]) {
			/* record state */
			return st.s | 1;
		}
	}
	return 0;
}

void
nmpr_isni(nmck_t s, const char *str, size_t len)
{
	isni_state_t st = {s};

	if (LIKELY(!s)) {
		fputs("ISNI, conformant with ISO 27729:2012", stdout);
	} else if (s > 0 && len > 0) {
		fputs("ISNI, not ISO 27729:2012 conformant, should be ", stdout);
		fwrite(str, sizeof(*str), len - 1U, stdout);
		fputc(st.chk, stdout);
	} else {
		fputs("unknown", stdout);
	}
	return;
}

/* isni.c ends here */

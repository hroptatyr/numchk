/*** istc.c -- checker for ISTC codes
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
#include <stdint.h>
#include "numchk.h"
#include "nifty.h"

typedef union {
	nmck_t s;
	struct {
		unsigned char len;
		unsigned char chk;
	};
} istc_state_t;

static __attribute__((pure, const)) uint_fast32_t
_chex(char c)
{
	if (LIKELY((unsigned char)(c ^ '0') < 10)) {
		return c ^ '0';
	} else if ((unsigned char)((c | 0x20) - 'W') < 16){
		return (unsigned char)((c | 0x20) - 'W');
	}
	/* no error code */
	return -1U;
}

static inline __attribute__((pure, const)) char
_hexc(char c)
{
	if (LIKELY((unsigned char)c < 10U)) {
		return (char)(c ^ '0');
	}
	/* no check for the upper bound of c */
	return (char)(c + '7');
}


nmck_t
nmck_istc(const char *str, size_t len)
{
/* calculate the check digit, this one is right to left, mod 16-3 */
	uint_fast32_t sum, wgt = 3U;
	ssize_t i = len - 1U;
	size_t j;

	/* common istces first */
	if (len < 13U || len > 19U) {
		return -1;
	}

	if ((sum = _chex(str[i])) >= 16U && !ischeck(str[i])) {
		return -1;
	} else if (str[i] == '_') {
		sum = 16U;
	}
	sum = 16U - sum;
	i--;

	i -= str[i] == '-';

	/* next up 8 hex digits */
	for (j = 0U; i >= 0U && j < 8U; j++, i--) {
		uint_fast32_t c = _chex(str[i]);

		if (UNLIKELY(c >= 16U)) {
			return -1;
		}
		sum += wgt * c;
		wgt *= 3U;
		wgt %= 16U;
	}
	if (j < 8U) {
		return -1;
	}

	i -= str[i] == '-';

	/* 4 digit year */
	with (uint_fast32_t c = (unsigned char)str[i--] ^ '0') {
		if (!c || c >= 3U) {
			/* year 3000? */
			return -1;
		}
		sum += wgt * c;
		wgt *= 3U;
		wgt %= 16U;
	}
	for (j = 1U; i >= 0U && j < 4U; j++, i--) {
		uint_fast32_t c = str[i] ^ '0';

		if (UNLIKELY(c >= 10U)) {
			return -1;
		}
		sum += wgt * c;
		wgt *= 3U;
		wgt %= 16U;
	}
	if (j < 4U) {
		return -1;
	}

	i -= str[i] == '-';

	/* now variable length */
	for (; i >= 0; i--) {
		uint_fast32_t c = _chex(str[i]);

		if (UNLIKELY(c >= 16U)) {
			return -1;
		}
		sum += wgt * c;
		wgt *= 3U;
		wgt %= 16U;
	}

	if ((sum %= 16U) || str[len - 1] == '_') {
		if (str[len - 1] != '_') {
			sum += _chex(str[len - 1]);
			sum %= 16U;
		}
		return (istc_state_t){.len = 1U, .chk = (unsigned char)sum}.s;
	}
	return 0;
}

void
nmpr_istc(nmck_t s, const char *str, size_t len)
{
	istc_state_t st = {s};

	if (LIKELY(!s)) {
		fputs("ISTC, conformant", stdout);
	} else if (s > 0 && len > 0) {
		fputs("ISTC, not conformant, should be ", stdout);
		fwrite(str, sizeof(*str), len - 1U, stdout);
		fputc(_hexc(st.chk), stdout);
	} else {
		fputs("unknown", stdout);
	}
	return;
}

/* istc.c ends here */

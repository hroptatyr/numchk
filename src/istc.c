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
#include "istc.h"

typedef union {
	unsigned int s;
	struct {
		unsigned char len;
		unsigned char chk;
	};
} istc_state_t;

static const nmck_bid_t nul_bid;
static const istc_state_t nul_state;

static __attribute__((pure, const)) uint_fast32_t
_chex(char c)
{
	if (LIKELY((unsigned char)(c ^ '0') < 10)) {
		return c ^ '0';
	} else if ((unsigned char)(c | 0x20) - 'W' < 16){
		return (unsigned char)(c | 0x20) - 'W';
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

static istc_state_t
calc_istc(const char *str, size_t len)
{
/* calculate the check digit, this one is right to left */
	static uint_fast32_t x[] = {1, 3, 9, 11};
	uint_fast32_t sum;
	ssize_t i = len - 1;
	size_t j, k = 1U;

	if ((sum = _chex(str[i--])) >= 16U) {
		return nul_state;
	}
	sum = 16U - sum;

	i -= str[i] == '-';

	/* next up 8 hex digits */
	for (j = 0U; i >= 0U && j < 8U; j++, i--) {
		uint_fast32_t c = _chex(str[i]);

		if (UNLIKELY(c >= 16U)) {
			return nul_state;
		}
		sum += x[k++] * c;
		k %= countof(x);
	}
	if (j < 8U) {
		return nul_state;
	}

	i -= str[i] == '-';

	/* 4 digit year */
	for (j = 0U; i >= 0U && j < 4U; j++, i--) {
		uint_fast32_t c = _chex(str[i]);

		if (UNLIKELY(c >= 16U)) {
			return nul_state;
		}
		sum += x[k++] * c;
		k %= countof(x);
	}
	if (j < 4U) {
		return nul_state;
	}

	i -= str[i] == '-';

	/* now variable length */
	for (; i >= 0; i--) {
		uint_fast32_t c = _chex(str[i]);

		if (UNLIKELY(c >= 16U)) {
			return nul_state;
		}
		sum += x[k++] * c;
		k %= countof(x);
	}

	if (sum %= 16U) {
		sum += _chex(str[len - 1]);
		sum %= 16U;
	}

	/* return both, length and check digit */
	return (istc_state_t){.len = 1U, .chk = (unsigned char)sum};
}


/* class implementation */
static nmck_bid_t
istc_bid(const char *str, size_t len)
{
	/* common istces first */
	if (len < 13U || len > 19U) {
		return nul_bid;
	}

	with (istc_state_t st = calc_istc(str, len)) {
		if (!st.s) {
			return nul_bid;
		} else if (st.chk >= 16U) {
			return nul_bid;
		} else if (st.chk) {
			/* record state */
			return (nmck_bid_t){31U, st.s};
		}
	}
	/* bid just any number really */
	return (nmck_bid_t){63U};
}

static int
istc_prnt(const char *str, size_t len, nmck_bid_t b)
{
	istc_state_t st = {b.state};

	if (LIKELY(!st.chk)) {
		fputs("ISTC, conformant", stdout);
	} else {
		fputs("ISTC, not conformant, should be ", stdout);
		fwrite(str, sizeof(*str), len - 1U, stdout);
		fputc(_hexc(st.chk), stdout);
	}
	return 0;
}

const struct nmck_chkr_s*
init_istc(void)
{
	static const struct nmck_chkr_s this = {
		.name = "ISTC",
		.bidf = istc_bid,
		.prntf = istc_prnt,
	};
	return &this;
}

int
fini_istc(void)
{
	return 0;
}

/* istc.c ends here */

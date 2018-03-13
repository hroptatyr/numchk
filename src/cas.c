/*** cas.c -- checker for CAS registry numbers
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
#include "cas.h"

typedef union {
	unsigned int s;
	struct {
		unsigned char len;
		unsigned char chk;
	};
} cas_state_t;

static const nmck_bid_t nul_bid;
static const cas_state_t nul_state;

static cas_state_t
calc_cas(const char *str, size_t len)
{
/* calculate the check digit, this one is right to left */
	unsigned int sum;
	size_t i = len, j = 1U;

	if ((sum = (unsigned char)(str[--i] ^ '0')) >= 10U) {
		return nul_state;
	}
	/* this was the check digit so invert */
	sum = 10U - sum;

	if (str[--i] != '-') {
		return nul_state;
	}
	with (unsigned int c = str[--i] ^ '0') {
		if (c >= 10U) {
			return nul_state;
		}
		sum += j++ * c;
	}
	with (unsigned int c = str[--i] ^ '0') {
		if (c >= 10U) {
			return nul_state;
		}
		sum += j++ * c;
	}
	/* hyphen again */
	if (str[--i] != '-') {
		return nul_state;
	}
	/* now variable length */
	for (; i > 0U;) {
		unsigned int c = str[--i] ^ '0';
		if (c >= 10U) {
			return nul_state;
		}
		sum += j++ * c;
	}

	if (sum %= 10U) {
		/* record state */
		unsigned int c = str[len - 1U] ^ '0';
		sum += c;
		sum %= 10U;
	}

	/* return both, length and check digit */
	return (cas_state_t){.len = j, .chk = (unsigned char)sum};
}


/* class implementation */
static nmck_bid_t
cas_bid(const char *str, size_t len)
{
	/* common cases first */
	if (len < 7U || len > 12U) {
		return nul_bid;
	}

	with (cas_state_t st = calc_cas(str, len)) {
		if (!st.s) {
			return nul_bid;
		} else if (st.chk >= 10U) {
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
cas_prnt(const char *str, size_t len, nmck_bid_t b)
{
	cas_state_t st = {b.state};

	if (LIKELY(!st.chk)) {
		fputs("CASRN, conformant", stdout);
	} else {
		fputs("CASRN, not conformant, should be ", stdout);
		fwrite(str, sizeof(*str), len - 1U, stdout);
		fputc(st.chk ^ '0', stdout);
	}
	return 0;
}

const struct nmck_chkr_s*
init_cas(void)
{
	static const struct nmck_chkr_s this = {
		.name = "CAS",
		.bidf = cas_bid,
		.prntf = cas_prnt,
	};
	return &this;
}

int
fini_cas(void)
{
	return 0;
}

/* cas.c ends here */

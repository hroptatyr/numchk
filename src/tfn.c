/*** tfn.c -- checker for ATO's tax file number
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
#include "tfn.h"

typedef union {
	unsigned int s;
	struct {
		unsigned char len;
		unsigned char chk;
	};
} tfn_state_t;

static const nmck_bid_t nul_bid;
static const tfn_state_t nul_state;

static tfn_state_t
calc_tfn(const char *str, size_t len)
{
/* calculate the check digit */
	unsigned int sum = 0U;
	size_t i, j;

	for (i = j = 0U; j < 9U && i < len; i++) {
		static char wgt[] = {
			1, 4, 3, 7, 5, 8, 6, 9, 10
		};
		if ((unsigned char)str[i] <= ' ') {
			continue;
		} else if (UNLIKELY((unsigned char)(str[i] ^ '0') >= 10)) {
			return nul_state;
		}
		sum += (unsigned char)(str[i] ^ '0') * wgt[j++];
	}
	if (i < len) {
		/* more? */
		return nul_state;
	} else if (j < 8U) {
		/* um, need at least 8 digits, no? */
		return nul_state;
	}

	if (sum %= 11U) {
		/* force divisibility by 11 */
		const unsigned char c = (unsigned char)(str[i - 1U] ^ '0');
		if (j == 9U) {
			sum += c;
		} else {
			sum += 2 * c + 2;
		}
		sum %= 11U;
	}

	/* return both, length and check digit */
	return (tfn_state_t){.len = j, .chk = (unsigned char)sum};
}


/* class implementation */
static nmck_bid_t
tfn_bid(const char *str, size_t len)
{
	/* common cases first */
	if (len < 8U || len > 11U) {
		return nul_bid;
	}

	with (tfn_state_t st = calc_tfn(str, len)) {
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
tfn_prnt(const char *str, size_t len, nmck_bid_t b)
{
	tfn_state_t st = {b.state};

	if (LIKELY(!st.chk)) {
		fputs("TFN, conformant", stdout);
	} else {
		fputs("TFN, not conformant, should be ", stdout);
		fwrite(str, sizeof(*str), len - 1U, stdout);
		fputc(st.chk ^ '0', stdout);
	}
	return 0;
}

const struct nmck_chkr_s*
init_tfn(void)
{
	static const struct nmck_chkr_s this = {
		.name = "TFN",
		.bidf = tfn_bid,
		.prntf = tfn_prnt,
	};
	return &this;
}

int
fini_tfn(void)
{
	return 0;
}

/* tfn.c ends here */

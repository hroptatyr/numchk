/*** isbn.c -- checker for ISBNs
 *
 * Copyright (C) 2014-2015 Sebastian Freundt
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
#include "isbn.h"


typedef union {
	unsigned int s;
	struct {
		short unsigned int pos;
		char std;
		char chk;
	};
} isbn_state_t;

static const nmck_bid_t nul_bid;
static const isbn_state_t nul_state;

static isbn_state_t
calc_isbn10(const char *str, size_t len)
{
/* calculate the check digit for an expanded ISIN */
	unsigned int sum = 0U;
	size_t i = 0U;
	char chk;

	/* use the left 9 digits, for ISBN-10 */
	for (size_t j = 0U; j < 9U && i < len; i++) {
		switch (str[i]) {
		case '-':
			/* ignore */
			continue;
		case '0' ... '9':
			sum += (10 - j++) * (str[i] ^ '0');
			break;
		default:
			return nul_state;
		}
	}
	/* check if need to skip optional - */
	if (LIKELY(i < len) && UNLIKELY(str[i] == '-')) {
		i++;
	}

	/* reduce sum mod 11 */
	if ((sum = (814U - sum) % 11U) < 10U) {
		chk = (char)(sum ^ '0');
	} else {
		chk = 'X';
	}

	/* return both, position of check digit and check digit */
	return (isbn_state_t){.pos = i, .chk = chk};
}

static isbn_state_t
calc_isbn13(const char *str, size_t len)
{
/* this is essentially the gtin process */
	unsigned int sum = 0U;
	size_t i, j;
	char chk;

	for (i = 0U, j = 0U; j < 12U && i < len; i++) {
		switch (str[i]) {
		case '-':
			/* ignore */
			continue;
		case '0' ... '9':
			sum += (str[i] ^ '0');
			if (j++ % 2U) {
				sum += (str[i] ^ '0') * 2U;
			}
			break;
		default:
			return (isbn_state_t){0U};
		}
	}
	/* check that we actually used up 12 digits */
	if (UNLIKELY(j < 12U)) {
		return nul_state;
	}
	/* check if need to skip optional - */
	if (LIKELY(i < len) && UNLIKELY(str[i] == '-')) {
		i++;
	}

	/* reduce sum mod 10 */
	chk = (char)(((400U - sum) % 10U) ^ '0');

	/* return both, position of check digit and check digit */
	return (isbn_state_t){.pos = i, .std = 3, .chk = chk};
}

static isbn_state_t
calc_chk(const char *str, size_t len)
{
/* we need a bit of guess work here */
	if (str[0U] == '9' && str[1U] == '7') {
		/* could be an isbn-13 */
		if (len >= 13U && (str[2U] == '8' || str[2U] == '9')) {
			isbn_state_t res = calc_isbn13(str, len);

			if (LIKELY(res.s)) {
				return res;
			}
			/* otherwise make sure to try ISBN-10 again */
		}
	}
	/* resort to good old ISO 2108 */
	return calc_isbn10(str, len);
}


/* class implementation */
static nmck_bid_t
isbn_bid(const char *str, size_t len)
{
	isbn_state_t st;

	/* common cases first */
	if (len < 10U || len > 17U) {
		return nul_bid;
	}

	st = calc_chk(str, len);
	if (!st.s) {
		return nul_bid;
	} else if (st.chk != str[st.pos]) {
		/* record state */
		return (nmck_bid_t){31U, st.s};
	}
	/* nul out the check digit because it passed */
	st.chk = 0;
	/* bid higher than gtin */
	return (nmck_bid_t){255U, st.s};
}

static int
isbn_prnt(const char *str, size_t UNUSED(len), nmck_bid_t b)
{
	isbn_state_t st = {b.state};

	if (LIKELY(!st.chk && st.std == 3)) {
		fputs("ISBN, conformant with ISO 2108:2005", stdout);
	} else if (LIKELY(!st.chk)) {
		fputs("ISBN, conformant with ISO 2108", stdout);
	} else {
		if (st.std == 3) {
			fputs("ISBN, not ISO 2108:2005 conformant, should be ", stdout);
		} else {
			fputs("ISBN, not ISO 2108 conformant, should be ", stdout);
		}
		fwrite(str, sizeof(*str), st.pos, stdout);
		fputc(st.chk, stdout);
	}
	return 0;
}

const struct nmck_chkr_s*
init_isbn(void)
{
	static const struct nmck_chkr_s this = {
		.name = "ISBN",
		.bidf = isbn_bid,
		.prntf = isbn_prnt,
	};
	return &this;
}

int
fini_isbn(void)
{
	return 0;
}

/* isbn.c ends here */

/*** iban.c -- checker for ISO 13616 international bank account numbers
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
#include <assert.h>
#include "numchk.h"
#include "nifty.h"
#include "iban.h"

typedef union {
	unsigned int s;
	struct {
		short unsigned int len;
		char chk[2U];
	};
} iban_state_t;

static const nmck_bid_t nul_bid;
static const iban_state_t nul_state;

#define C2I(x)			((x) - 'A')
#define BEGINNING_WITH(x)	[C2I(x)] =
#define ALLOW(x)		(1U << C2I(x))
#define AND			|

/* here we register all allowed country codes, as per
 * http://www.nordea.com/Our+services/Cash+Management/Products+and+services/IBAN+countries/908462.html */
static const uint_fast32_t cc[] = {
	BEGINNING_WITH('A')
	ALLOW('D') AND ALLOW('E') AND ALLOW('L') AND ALLOW('O') AND
	ALLOW('T') AND ALLOW('Z'),

	BEGINNING_WITH('B')
	ALLOW('A') AND ALLOW('E') AND ALLOW('F') AND ALLOW('G') AND
	ALLOW('H') AND ALLOW('I') AND ALLOW('J') AND ALLOW('R'),

	BEGINNING_WITH('C')
	ALLOW('G') AND ALLOW('H') AND ALLOW('I') AND ALLOW('M') AND
	ALLOW('R') AND ALLOW('V') AND ALLOW('Y') AND ALLOW('Z'),

	BEGINNING_WITH('D')
	ALLOW('E') AND ALLOW('K') AND ALLOW('O') AND ALLOW('Z'),

	BEGINNING_WITH('E')
	ALLOW('E') AND ALLOW('G') AND ALLOW('S'),

	BEGINNING_WITH('F')
	ALLOW('I') AND ALLOW('O') AND ALLOW('R'),

	BEGINNING_WITH('G')
	ALLOW('A') AND ALLOW('B') AND ALLOW('E') AND ALLOW('I') AND
	ALLOW('L') AND ALLOW('R') AND ALLOW('T'),

	BEGINNING_WITH('H')
	ALLOW('R') AND ALLOW('U'),

	BEGINNING_WITH('I')
	ALLOW('E') AND ALLOW('L') AND ALLOW('R') AND ALLOW('S') AND
	ALLOW('T'),

	BEGINNING_WITH('J')
	ALLOW('O'),

	BEGINNING_WITH('K')
	ALLOW('W') AND ALLOW('Z'),

	BEGINNING_WITH('L')
	ALLOW('B') AND ALLOW('I') AND ALLOW('T') AND ALLOW('U') AND
	ALLOW('V'),

	BEGINNING_WITH('M')
	ALLOW('C') AND ALLOW('D') AND ALLOW('E') AND ALLOW('G') AND
	ALLOW('K') AND ALLOW('L') AND ALLOW('R') AND ALLOW('T') AND
	ALLOW('U') AND ALLOW('Z'),

	BEGINNING_WITH('N')
	ALLOW('L') AND ALLOW('O'),

	BEGINNING_WITH('P')
	ALLOW('K') AND ALLOW('L') AND ALLOW('S') AND ALLOW('T'),

	BEGINNING_WITH('Q')
	ALLOW('A'),

	BEGINNING_WITH('R')
	ALLOW('O') AND ALLOW('S'),

	BEGINNING_WITH('S')
	ALLOW('A') AND ALLOW('E') AND ALLOW('I') AND ALLOW('K') AND
	ALLOW('M') AND ALLOW('N'),

	BEGINNING_WITH('T')
	ALLOW('L') AND ALLOW('N') AND ALLOW('R'),

	BEGINNING_WITH('U')
	ALLOW('A'),

	BEGINNING_WITH('V')
	ALLOW('G'),

	BEGINNING_WITH('X')
	ALLOW('K'),
};

static iban_state_t
calc_st(const char *str, size_t len)
{
/* calculate the check digit for an expanded ISIN */
	char buf[78U];
	size_t bsz = 0U;
	unsigned int sum = 0U;
	iban_state_t res;
	size_t j = 0U;

	/* expand string first */
	for (size_t i = 4U; i < len; i++) {
		switch (str[i]) {
		case ' ':
			continue;
		case '0' ... '9':
			buf[bsz++] = str[i];
			j++;
			break;
		case 'A' ... 'J':
			buf[bsz++] = '1';
			buf[bsz++] = (char)((str[i] - 'A') ^ '0');
			j++;
			break;
		case 'K' ... 'T':
			buf[bsz++] = '2';
			buf[bsz++] = (char)((str[i] - 'K') ^ '0');
			j++;
			break;
		case 'U' ... 'Z':
			buf[bsz++] = '3';
			buf[bsz++] = (char)((str[i] - 'U') ^ '0');
			j++;
			break;
		default:
			return nul_state;
		}
	}
	/* append the country code */
	switch (str[0U]) {
	case 'A' ... 'J':
		buf[bsz++] = '1';
		buf[bsz++] = (char)((str[0U] - 'A') ^ '0');
		break;
	case 'K' ... 'T':
		buf[bsz++] = '2';
		buf[bsz++] = (char)((str[0U] - 'K') ^ '0');
		break;
	case 'U' ... 'Z':
		buf[bsz++] = '3';
		buf[bsz++] = (char)((str[0U] - 'U') ^ '0');
		break;
	default:
		return nul_state;
	}
	switch (str[1U]) {
	case 'A' ... 'J':
		buf[bsz++] = '1';
		buf[bsz++] = (char)((str[1U] - 'A') ^ '0');
		break;
	case 'K' ... 'T':
		buf[bsz++] = '2';
		buf[bsz++] = (char)((str[1U] - 'K') ^ '0');
		break;
	case 'U' ... 'Z':
		buf[bsz++] = '3';
		buf[bsz++] = (char)((str[1U] - 'U') ^ '0');
		break;
	default:
		return nul_state;
	}
	/* and 00 */
	buf[bsz++] = '0';
	buf[bsz++] = '0';

	/* now calc first sum */
	sum = (buf[0U] ^ '0') * 10U + (buf[1U] ^ '0');
	for (size_t i = 2U; i < bsz; sum %= 97U) {
		for (const size_t n = i + 7U < bsz ? i + 7U : bsz; i < n; i++) {
			sum *= 10U;
			sum += (buf[i] ^ '0');
		}
	}
	/* this is the actual checksum */
	sum = 98U - sum;
	res.len = j + 4U;
	res.chk[0U] = (char)((sum / 10U) ^ '0');
	res.chk[1U] = (char)((sum % 10U) ^ '0');
	return res;
}


/* class implementation */
static nmck_bid_t
iban_bid(const char *str, size_t len)
{
	/* common cases first */
	if (len < 15U || len > 34U) {
		return nul_bid;
	} else if (str[0U] < 'A' || str[0U] > 'Z') {
		return nul_bid;
	} else if (str[1U] < 'A' || str[1U] > 'Z') {
		return nul_bid;
	} else if (!(cc[C2I(str[0U])] & ALLOW(str[1U]))) {
		return nul_bid;
	}

	with (iban_state_t st = calc_st(str, len)) {
		if (!st.s) {
			return nul_bid;
		} else if (str[2U] != st.chk[0U] || str[3U] != st.chk[1U]) {
			/* record state */
			return (nmck_bid_t){31U, st.s};
		}
	}
	/* bid just any number really */
	return (nmck_bid_t){63U};
}

static int
iban_prnt(const char *str, size_t len, nmck_bid_t b)
{
	iban_state_t st = {b.state};

	if (LIKELY(!b.state)) {
		fputs("IBAN, conformant with ISO 13616-1:2007", stdout);
	} else {
		fputs("IBAN, not ISO 13616-1 conformant, should be ", stdout);
		fputc(str[0U], stdout);
		fputc(str[1U], stdout);
		fputc(st.chk[0U], stdout);
		fputc(st.chk[1U], stdout);
		fwrite(str + 4U, sizeof(*str), len - 4U, stdout);
	}
	return 0;
}

const struct nmck_chkr_s*
init_iban(void)
{
	static const struct nmck_chkr_s this = {
		.name = "IBAN",
		.bidf = iban_bid,
		.prntf = iban_prnt,
	};
	return &this;
}

int
fini_iban(void)
{
	return 0;
}

/* iban.c ends here */

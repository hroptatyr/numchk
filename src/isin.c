/*** isin.c -- checker for ISO 6166 security idenfitication numbers
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
#include <stdint.h>
#include <assert.h>
#include "numchk.h"
#include "nifty.h"
#include "isin.h"

static const nmck_bid_t nul_bid;

#define C2I(x)			((x) - 'A')
#define BEGINNING_WITH(x)	[C2I(x)] =
#define ALLOW(x)		(1U << C2I(x))
#define AND			|

/* here we register all allowed country codes, as per
 * http://www.isin.net/country-codes */
static const uint_fast32_t cc[] = {
	BEGINNING_WITH('A')
	ALLOW('D') AND ALLOW('E') AND ALLOW('G') AND ALLOW('I') AND
	ALLOW('L') AND ALLOW('M') AND ALLOW('O') AND ALLOW('Q') AND
	ALLOW('R') AND ALLOW('S') AND ALLOW('T') AND ALLOW('U') AND
	ALLOW('W') AND ALLOW('X') AND ALLOW('Z'),

	BEGINNING_WITH('B')
	ALLOW('A') AND ALLOW('B') AND ALLOW('D') AND ALLOW('E') AND
	ALLOW('F') AND ALLOW('G') AND ALLOW('H') AND ALLOW('I') AND
	ALLOW('J') AND ALLOW('L') AND ALLOW('M') AND ALLOW('N') AND
	ALLOW('O') AND ALLOW('Q') AND ALLOW('R') AND ALLOW('S') AND
	ALLOW('T') AND ALLOW('V') AND ALLOW('W') AND ALLOW('Y') AND
	ALLOW('Z'),

	BEGINNING_WITH('C')
	ALLOW('A') AND ALLOW('C') AND ALLOW('D') AND ALLOW('F') AND
	ALLOW('G') AND ALLOW('H') AND ALLOW('I') AND ALLOW('K') AND
	ALLOW('L') AND ALLOW('M') AND ALLOW('N') AND ALLOW('O') AND
	ALLOW('R') AND ALLOW('U') AND ALLOW('V') AND ALLOW('W') AND
	ALLOW('X') AND ALLOW('Y') AND ALLOW('Z'),

	BEGINNING_WITH('D')
	ALLOW('E') AND ALLOW('J') AND ALLOW('K') AND ALLOW('M') AND
	ALLOW('O') AND ALLOW('Z'),

	BEGINNING_WITH('E')
	ALLOW('C') AND ALLOW('E') AND ALLOW('G') AND ALLOW('H') AND
	ALLOW('R') AND ALLOW('S') AND ALLOW('T'),

	BEGINNING_WITH('F')
	ALLOW('I') AND ALLOW('J') AND ALLOW('K') AND ALLOW('M') AND
	ALLOW('O') AND ALLOW('R'),

	BEGINNING_WITH('G')
	ALLOW('A') AND ALLOW('B') AND ALLOW('D') AND ALLOW('E') AND
	ALLOW('F') AND ALLOW('G') AND ALLOW('H') AND ALLOW('I') AND
	ALLOW('L') AND ALLOW('M') AND ALLOW('N') AND ALLOW('P') AND
	ALLOW('Q') AND ALLOW('R') AND ALLOW('S') AND ALLOW('T') AND
	ALLOW('U') AND ALLOW('W') AND ALLOW('Y'),

	BEGINNING_WITH('H')
	ALLOW('K') AND ALLOW('M') AND ALLOW('N') AND ALLOW('R') AND
	ALLOW('T') AND ALLOW('U'),

	BEGINNING_WITH('I')
	ALLOW('D') AND ALLOW('E') AND ALLOW('L') AND ALLOW('M') AND
	ALLOW('N') AND ALLOW('O') AND ALLOW('Q') AND ALLOW('R') AND
	ALLOW('S') AND	ALLOW('T'),

	BEGINNING_WITH('J')
	ALLOW('E') AND ALLOW('M') AND ALLOW('O') AND ALLOW('P'),

	BEGINNING_WITH('K')
	ALLOW('E') AND ALLOW('G') AND ALLOW('H') AND ALLOW('I') AND
	ALLOW('M') AND ALLOW('N') AND ALLOW('P') AND ALLOW('R') AND
	ALLOW('W') AND ALLOW('Y') AND ALLOW('Z'),

	BEGINNING_WITH('L')
	ALLOW('A') AND ALLOW('B') AND ALLOW('C') AND ALLOW('I') AND
	ALLOW('K') AND ALLOW('R') AND ALLOW('S') AND ALLOW('T') AND
	ALLOW('U') AND ALLOW('V') AND ALLOW('Y'),

	BEGINNING_WITH('M')
	ALLOW('A') AND ALLOW('C') AND ALLOW('D') AND ALLOW('E') AND
	ALLOW('F') AND ALLOW('G') AND ALLOW('H') AND ALLOW('K') AND
	ALLOW('L') AND ALLOW('M') AND ALLOW('N') AND ALLOW('O') AND
	ALLOW('P') AND ALLOW('Q') AND ALLOW('R') AND ALLOW('S') AND
	ALLOW('T') AND ALLOW('U') AND ALLOW('V') AND ALLOW('W') AND
	ALLOW('X') AND ALLOW('Y') AND ALLOW('Z'),

	BEGINNING_WITH('N')
	ALLOW('A') AND ALLOW('C') AND ALLOW('E') AND ALLOW('F') AND
	ALLOW('G') AND ALLOW('I') AND ALLOW('L') AND ALLOW('O') AND
	ALLOW('P') AND ALLOW('R') AND ALLOW('U') AND ALLOW('Z'),

	BEGINNING_WITH('O')
	ALLOW('M'),

	BEGINNING_WITH('P')
	ALLOW('A') AND ALLOW('E') AND ALLOW('F') AND ALLOW('G') AND
	ALLOW('H') AND ALLOW('K') AND ALLOW('L') AND ALLOW('M') AND
	ALLOW('N') AND ALLOW('R') AND ALLOW('S') AND ALLOW('T') AND
	ALLOW('W') AND ALLOW('Y'),

	BEGINNING_WITH('Q')
	ALLOW('A'),

	BEGINNING_WITH('R')
	ALLOW('E') AND ALLOW('O') AND ALLOW('S') AND ALLOW('U') AND
	ALLOW('W'),

	BEGINNING_WITH('S')
	ALLOW('A') AND ALLOW('B') AND ALLOW('C') AND ALLOW('D') AND
	ALLOW('E') AND ALLOW('G') AND ALLOW('H') AND ALLOW('I') AND
	ALLOW('J') AND ALLOW('K') AND ALLOW('L') AND ALLOW('M') AND
	ALLOW('N') AND ALLOW('O') AND ALLOW('R') AND ALLOW('S') AND
	ALLOW('T') AND ALLOW('V') AND ALLOW('X') AND ALLOW('Y') AND
	ALLOW('Z'),

	BEGINNING_WITH('T')
	ALLOW('C') AND ALLOW('D') AND ALLOW('F') AND ALLOW('G') AND
	ALLOW('H') AND ALLOW('J') AND ALLOW('K') AND ALLOW('L') AND
	ALLOW('M') AND ALLOW('N') AND ALLOW('O') AND ALLOW('R') AND
	ALLOW('T') AND ALLOW('V') AND ALLOW('W') AND ALLOW('Z'),

	BEGINNING_WITH('U')
	ALLOW('A') AND ALLOW('G') AND ALLOW('M') AND ALLOW('S') AND
	ALLOW('Y') AND ALLOW('Z'),

	BEGINNING_WITH('V')
	ALLOW('A') AND ALLOW('C') AND ALLOW('E') AND ALLOW('G') AND
	ALLOW('I') AND ALLOW('N') AND ALLOW('U'),

	BEGINNING_WITH('W')
	ALLOW('F') AND ALLOW('S'),

	BEGINNING_WITH('X')
	ALLOW('S'),

	BEGINNING_WITH('Y')
	ALLOW('E') AND ALLOW('T'),

	BEGINNING_WITH('Z')
	ALLOW('A') AND ALLOW('M') AND ALLOW('W'),
};

static char
calc_chk(const char *str, size_t len)
{
/* calculate the check digit for an expanded ISIN */
	unsigned int sum = 0U;

	for (size_t i = !(len % 2U); i < len; i += 2U) {
		int code = (str[i] ^ '0') * 2;
		sum += (code / 10) + (code % 10);
	}
	for (size_t i = (len % 2U); i < len; i += 2U) {
		int code = (str[i] ^ '0');
		sum += code;
	}
	/* sum can be at most 198, so check digit is */
	return (char)(((200U - sum) % 10U) ^ '0');
}


/* class implementation */
static nmck_bid_t
isin_bid(const char *str, size_t len)
{
	char buf[24U];
	size_t bsz = 0U;

	/* common cases first */
	if (len != 12) {
		return nul_bid;
	} else if (str[0U] < 'A' || str[0U] > 'Z') {
		return nul_bid;
	} else if (str[1U] < 'A' || str[1U] > 'Z') {
		return nul_bid;
	} else if (!(cc[C2I(str[0U])] & ALLOW(str[1U]))) {
		return nul_bid;
	}

	/* expand the left 11 digits */
	for (size_t i = 0U; i < 11; i++) {
		switch (str[i]) {
		case '0' ... '9':
			buf[bsz++] = str[i];
			break;
		case 'A' ... 'J':
			buf[bsz++] = '1';
			buf[bsz++] = (char)((str[i] - 'A') ^ '0');
			break;
		case 'K' ... 'T':
			buf[bsz++] = '2';
			buf[bsz++] = (char)((str[i] - 'K') ^ '0');
			break;
		case 'U' ... 'Z':
			buf[bsz++] = '3';
			buf[bsz++] = (char)((str[i] - 'U') ^ '0');
			break;
		default:
			return nul_bid;
		}
	}
	with (char chk = calc_chk(buf, bsz)) {
		if (chk != str[11U]) {
			/* record state but submit a bid */
			return (nmck_bid_t){63U, chk};
		}
	}
	/* bid bid bid */
	return (nmck_bid_t){255U};
}

static int
isin_prnt(const char *str, size_t len, nmck_bid_t b)
{
	if (LIKELY(!b.state)) {
		fputs("ISIN, conformant with ISO 6166:2013", stdout);
	} else {
		assert(len == 12U);
		fputs("ISIN, not ISO 6166 conformant, should be ", stdout);
		fwrite(str, sizeof(*str), 11U, stdout);
		fputc((char)b.state, stdout);
	}
	return 0;
}

const struct nmck_chkr_s*
init_isin(void)
{
	static const struct nmck_chkr_s this = {
		.name = "ISIN",
		.bidf = isin_bid,
		.prntf = isin_prnt,
	};
	return &this;
}

int
fini_isin(void)
{
	return 0;
}

/* isin.c ends here */

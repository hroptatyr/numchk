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

static const nmck_bid_t nul_bid;

static unsigned int
calc_st(const char *str, size_t len)
{
/* calculate the check digit for an expanded ISIN */
	char buf[78U];
	size_t bsz = 0U;
	unsigned int sum = 0U;

	/* expand string first */
	for (size_t i = 4U; i < len; i++) {
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
			return 0U;
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
		return 0U;
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
		return 0U;
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
	return (((sum / 10U) ^ '0') << 8U) ^ (((sum % 10U) ^ '0') << 0U);
}


/* class implementation */
static nmck_bid_t
iban_bid(const char *str, size_t len)
{
	/* common cases first */
	if (len < 15U || len > 34U) {
		return nul_bid;
	}

	with (unsigned int st = calc_st(str, len)) {
		if (!st) {
			return nul_bid;
		} else if (str[2U] != (char)((st >> 8U) & 0xffU) ||
			   str[3U] != (char)((st >> 0U) & 0xffU)) {
			/* record state */
			return (nmck_bid_t){31U, st};
		}
	}
	/* bid just any number really */
	return (nmck_bid_t){63U};
}

static int
iban_prnt(const char *str, size_t len, nmck_bid_t b)
{
	if (LIKELY(!b.state)) {
		fputs("IBAN, conformant with ISO 13616-1:2007", stdout);
	} else {
		fputs("IBAN, not ISO 13616-1 conformant, should be ", stdout);
		fputc(str[0U], stdout);
		fputc(str[1U], stdout);
		fputc(b.state >> 8U & 0xffU, stdout);
		fputc(b.state >> 0U & 0xffU, stdout);
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

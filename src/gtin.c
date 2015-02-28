/*** gtin.c -- checker for GS1's GTINs
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
#include "gtin.h"

static const nmck_bid_t nul_bid;

static char
calc_chk(const char *str, size_t len)
{
/* calculate the check digit for an expanded ISIN */
	unsigned int sum = 0U;

	/* use the left len - 1 digits, start with  the evens */
	for (size_t i = len % 2U; i < len - 1U; i += 2U) {
		if (UNLIKELY(str[i] < '0' || str[i] > '9')) {
			return '\0';
		}
		sum += 3U * (str[i] ^ '0');
	}
	/* odd numbers now */
	for (size_t i = !(len % 2U); i < len - 1U; i += 2U) {
		if (UNLIKELY(str[i] < '0' || str[i] > '9')) {
			return '\0';
		}
		sum += str[i] ^ '0';
	}

	/* sum can be at most 252, so check digit is */
	return (char)(((400U - sum) % 10U) ^ '0');
}


/* class implementation */
static nmck_bid_t
gtin_bid(const char *str, size_t len)
{
	/* common cases first */
	if (len < 8U || len > 14U) {
		return nul_bid;
	}

	with (char chk = calc_chk(str, len)) {
		if (!chk) {
			return nul_bid;
		} else if (chk != str[len - 1U]) {
			/* record state */
			return (nmck_bid_t){31U, chk};
		}
	}
	/* bid higher than isin */
	return (nmck_bid_t){63U};
}

static int
gtin_prnt(const char *str, size_t len, nmck_bid_t b)
{
	if (LIKELY(!b.state)) {
		fputs("GTIN, conformant", stdout);
	} else {
		fputs("GTIN, not conformant, should be ", stdout);
		fwrite(str, sizeof(*str), len - 1U, stdout);
		fputc((char)b.state, stdout);
	}
	return 0;
}

const struct nmck_chkr_s*
init_gtin(void)
{
	static const struct nmck_chkr_s this = {
		.name = "GTIN",
		.bidf = gtin_bid,
		.prntf = gtin_prnt,
	};
	return &this;
}

int
fini_gtin(void)
{
	return 0;
}

/* gtin.c ends here */

/*** iban.c -- checker for ISO 13616 international bank account numbers
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

typedef union {
	intptr_t s;
	struct {
		short unsigned int len;
		char chk[2U];
	};
} iban_state_t;

/* here we register all allowed country codes, as per
 * http://www.nordea.com/Our+services/Cash+Management/Products+and+services/IBAN+countries/908462.html */
#include "iban-cc.c"

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
			return (iban_state_t){};
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
		return (iban_state_t){};
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
		return (iban_state_t){};
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
	res.len = (unsigned short)(j + 4U);
	res.chk[0U] = (char)((sum / 10U) ^ '0');
	res.chk[1U] = (char)((sum % 10U) ^ '0');
	return res;
}


/* class implementation */
nmck_t
nmck_iban(const char *str, size_t len)
{
	/* common cases first */
	if (len < 15U || len > 34U) {
		return -1;
	} else if (!valid_cc_p(str)) {
		return -1;
	}

	with (iban_state_t st = calc_st(str, len)) {
		if (!st.s) {
			return -1;
		} else if (cc_len(str) != st.len) {
			return -1;
		} else if (str[2U] != st.chk[0U] || str[3U] != st.chk[1U]) {
			/* record state */
			return st.s;
		}
	}
	/* all is tickety-boo */
	return 0;
}

void
nmpr_iban(nmck_t st, const char *str, size_t len)
{
	if (LIKELY(!st)) {
		fputs("IBAN, conformant with ISO 13616-1:2007", stdout);
	} else if (st > 0) {
		iban_state_t ibst = {st};
		fputs("IBAN, not ISO 13616-1 conformant, should be ", stdout);
		fputc(str[0U], stdout);
		fputc(str[1U], stdout);
		fputc(ibst.chk[0U], stdout);
		fputc(ibst.chk[1U], stdout);
		fwrite(str + 4U, sizeof(*str), len - 4U, stdout);
	} else {
		fputs("unknown", stdout);
	}
	return;
}

/* iban.c ends here */

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

/* here we register all allowed country codes, as per
 * http://www.nordea.com/Our+services/Cash+Management/Products+and+services/IBAN+countries/908462.html */
#include "iban-cc.c"

#ifdef RAGEL_BLOCK
%%{
	machine numchk;

	_iban =
		upper{2} digit{2} (upnum | ' '){11,42} |
		upper{2} check{2} (upnum | ' '){11,42} ;
	iban = _iban %{c(iban)} ;
}%%
#endif	/* RAGEL_BLOCK */


nmck_t
nmck_iban(const char *str, size_t len)
{
	uint_fast8_t buf[78U];
	size_t bsz = 0U;
	char chk[2U];
	uint_fast32_t sum = 0U;
	size_t j = 0U;

	/* common cases first */
	if (len < 15U || len > 34U) {
		return -1;
	} else if (!valid_cc_p(str)) {
		return -1;
	}

	/* expand string first */
	for (size_t i = 4U; i < len; i++) {
		switch (str[i]) {
		case ' ':
			continue;
		case '0' ... '9':
			buf[bsz++] = (unsigned char)(str[i] ^ '0');
			j++;
			break;
		case 'A' ... 'J':
			buf[bsz++] = 1U;
			buf[bsz++] = (unsigned char)(str[i] - 'A');
			j++;
			break;
		case 'K' ... 'T':
			buf[bsz++] = 2U;
			buf[bsz++] = (unsigned char)(str[i] - 'K');
			j++;
			break;
		case 'U' ... 'Z':
			buf[bsz++] = 3U;
			buf[bsz++] = (unsigned char)(str[i] - 'U');
			j++;
			break;
		default:
			return -1;
		}
	}
	/* append the country code */
	switch (str[0U]) {
	case 'A' ... 'J':
		buf[bsz++] = 1U;
		buf[bsz++] = (unsigned char)(str[0U] - 'A');
		break;
	case 'K' ... 'T':
		buf[bsz++] = 2U;
		buf[bsz++] = (unsigned char)(str[0U] - 'K');
		break;
	case 'U' ... 'Z':
		buf[bsz++] = 3U;
		buf[bsz++] = (unsigned char)(str[0U] - 'U');
		break;
	default:
		return -1;
	}
	switch (str[1U]) {
	case 'A' ... 'J':
		buf[bsz++] = 1U;
		buf[bsz++] = (unsigned char)(str[1U] - 'A');
		break;
	case 'K' ... 'T':
		buf[bsz++] = 2U;
		buf[bsz++] = (unsigned char)(str[1U] - 'K');
		break;
	case 'U' ... 'Z':
		buf[bsz++] = 3U;
		buf[bsz++] = (unsigned char)(str[1U] - 'U');
		break;
	default:
		return -1;
	}
	/* and 00 */
	buf[bsz++] = 0U;
	buf[bsz++] = 0U;

	/* check length for country */
	if (UNLIKELY(cc_len(str) != j + 4U)) {
		return -1;
	}

	/* now calc first sum */
	sum = buf[0U] * 10U + buf[1U];
	for (size_t i = 2U; i < bsz; sum %= 97U) {
		for (const size_t n = i + 7U < bsz ? i + 7U : bsz; i < n; i++) {
			sum *= 10U;
			sum += buf[i];
		}
	}
	/* this is the actual checksum */
	sum = 98U - sum;
	chk[0U] = (char)((sum / 10U) ^ '0');
	chk[1U] = (char)((sum % 10U) ^ '0');

	/* all is tickety-boo? */
	return (chk[0U] << 8U ^ chk[1U]) << 8U ^
		(chk[0U] != str[2U] || chk[1U] != str[3U]);
}

void
nmpr_iban(nmck_t s, const char *str, size_t len)
{
	if (LIKELY(!(s & 0b1U))) {
		fputs("IBAN, conformant with ISO 13616-1:2007", stdout);
	} else if (s > 0 && len >= 4U) {
		fputs("IBAN, not ISO 13616-1 conformant, should be ", stdout);
		fputc(str[0U], stdout);
		fputc(str[1U], stdout);
		fputc(s >> 16U & 0x7fU, stdout);
		fputc(s >> 8U & 0x7fU, stdout);
		fwrite(str + 4U, sizeof(*str), len - 4U, stdout);
	} else {
		fputs("unknown", stdout);
	}
	return;
}

/* iban.c ends here */

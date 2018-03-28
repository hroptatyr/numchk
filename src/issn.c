/*** issn.c -- checker for ISSNs and ISSN GTINs
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
#include "numchk.h"
#include "nifty.h"

enum {
	ISSN_UNK,
	ISSN13,
	ISSN8,
};

static int
issn13p(const char *str, size_t len)
{
	return len >= 13U &&
		str[0U] == '9' && str[1U] == '7' && str[2U] == '7';
}


/* issns com in two variants, issn8 and issn13 */
nmck_t
nmck_issn8(const char *str, size_t len)
{
	uint_fast32_t sum = 0U;

	if (len < 8U || len > 9U) {
		return -1;
	}

	for (size_t i = 0U, j = 1U; i < len - 1U; i++) {
		uint_fast32_t c = (str[i] ^ '0');

		if (str[i] == '-') {
			/* ignore the 4th only */
			if (i == 4U) {
				continue;
			}
		} else if (UNLIKELY(c >= 10U)) {
			return -1;
		}
		sum += j++ * c;
	}
	sum %= 11U;
	sum ^= sum < 10U ? '0' : 'R'/*0xA^'X'*/;

	return (sum << 8U ^ ISSN8) << 8U ^ ((char)sum != str[len - 1U]);
}

void
nmpr_issn8(nmck_t s, const char *str, size_t len)
{
	if (LIKELY(!(s & 0b1U))) {
		fputs("ISSN, conformant with ISO 3297:1975", stdout);
	} else if (s > 0 && len > 0) {
		fputs("ISSN, not ISO 3297:1975 conformant, should be ", stdout);
		fwrite(str, sizeof(*str), len - 1, stdout);
		fputc(s >> 16U & 0x7fU, stdout);
	} else {
		fputs("unknown", stdout);
	}
	return;
}

nmck_t
nmck_issn13(const char *str, size_t len)
{
/* this is essentially the gtin process */
	uint_fast32_t sum = 0U;
	uint_fast32_t w = 1U;
	size_t j;

	if (len < 13U || len > 18U) {
		return -1;
	} else if (!issn13p(str, len)) {
		return -1;
	}

	for (size_t i = j = 0U; j < 12U && i < len; i++) {
		uint_fast32_t c = (str[i] ^ '0');

		if (str[i] == '-') {
			continue;
		} else if (UNLIKELY(c >= 10U)) {
			return -1;
		}
		sum += c * w;
		w ^= 2U;
		j++;
	}
	/* check that we actually used up 12 digits */
	if (UNLIKELY(j < 12U)) {
		return -1;
	}

	/* reduce sum mod 10 */
	sum = (((400U - sum) % 10U) ^ '0');

	return ((sum << 8U) ^ ISSN13) << 8U ^ ((char)sum != str[len - 1U]);
}

void
nmpr_issn13(nmck_t s, const char *str, size_t len)
{
	if (LIKELY(!(s & 0b1U))) {
		fputs("ISSN, conformant with ISO 3297:2007", stdout);
	} else if (s > 0 && len > 0) {
		fputs("ISSN, not ISO 3297:2007 conformant, should be ", stdout);
		fwrite(str, sizeof(*str), len - 1, stdout);
		fputc(s >> 16U & 0x7fU, stdout);
	} else {
		fputs("unknown", stdout);
	}
	return;
}

nmck_t
nmck_issn(const char *str, size_t len)
{
	return issn13p(str, len) ? nmck_issn13(str, len) : nmck_issn8(str, len);
}

void
nmpr_issn(nmck_t s, const char *str, size_t len)
{
	if (UNLIKELY(s < 0)) {
	unk:
		fputs("unknown", stdout);
	} else if (LIKELY(!(s & 0b1U))) {
		fputs("ISSN, conformant with ", stdout);
		switch (s >> 8U & 0x7fU) {
		case ISSN13:
			fputs("ISO 3297:2007", stdout);
			break;
		case ISSN8:
			fputs("ISO 3297:1975", stdout);
			break;
		default:
			goto unk;
		}
	} else if (len > 0) {
		fputs("ISSN, not ", stdout);
		switch (s >> 8U & 0x7fU) {
		case ISSN13:
			fputs("ISO 3297:2007", stdout);
			break;
		case ISSN8:
			fputs("ISO 3297:1975", stdout);
			break;
		default:
			break;
		}
		fputs(" conformant, should be ", stdout);
		fwrite(str, sizeof(*str), len - 1, stdout);
		fputc(s >> 16U & 0x7fU, stdout);
	}
	return;
}

/* issn.c ends here */

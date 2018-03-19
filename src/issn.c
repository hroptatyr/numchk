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

typedef union {
	nmck_t s;
	struct {
		unsigned char pos;
		unsigned char std;
		char chk;
	};
} issn_state_t;

enum {
	ISSN_UNK,
	ISSN13,
	ISSN8,
};

static issn_state_t
calc_issn8(const char *str, size_t len)
{
	unsigned int sum = 0U;
	size_t i = 0U;
	char chk;

	/* use the left 7 digits, for ISSN-8 */
	for (size_t j = 0U; j < 7U && i < len; i++) {
		switch (str[i]) {
		case '0' ... '9':
			sum += (8 - j++) * (str[i] ^ '0');
			break;
		case '-':
			/* ignore if 4th*/
			if (i == 4U) {
				continue;
			}
		default:
			return (issn_state_t){0};
		}
	}

	/* reduce sum mod 11 */
	if (sum %= 11U) {
		chk = (char)(sum < 10 ? sum ^ '0' : 'X');
	} else {
		chk = '0';
	}

	/* return both, position of check digit and check digit */
	return (issn_state_t){.pos = i, .std = ISSN8, .chk = chk};
}

static issn_state_t
calc_issn13(const char *str, size_t len)
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
			return (issn_state_t){0};
		}
	}
	/* check that we actually used up 12 digits */
	if (UNLIKELY(j < 12U)) {
		return (issn_state_t){0};
	}
	/* check if need to skip optional - */
	if (LIKELY(i < len) && UNLIKELY(str[i] == '-')) {
		i++;
	}

	/* reduce sum mod 10 */
	chk = (char)(((400U - sum) % 10U) ^ '0');

	/* return both, position of check digit and check digit */
	return (issn_state_t){.pos = i, .std = ISSN13, .chk = chk};
}

static int
issn13p(const char *str, size_t len)
{
	return len >= 13U &&
		str[0U] == '9' && str[1U] == '7' && str[2U] == '7';
}

static issn_state_t
calc_chk(const char *str, size_t len)
{
/* we need a bit of guess work here */
	if (issn13p(str, len)) {
		issn_state_t res = calc_issn13(str, len);

		if (LIKELY(res.std == ISSN13)) {
			return res;
		}
		/* otherwise make sure to try ISSN-8 again */
	}
	/* resort to good old ISO 3297 */
	return calc_issn8(str, len);
}


/* issns com in two variants, issn8 and issn13 */
nmck_t
nmck_issn8(const char *str, size_t len)
{
	issn_state_t st;

	if (len < 8U || len > 9U) {
		return -1;
	}

	st = calc_issn8(str, len);
	if (!st.s) {
		return -1;
	} else if (st.pos != len - 1U) {
		return -1;
	} else if (st.chk != str[st.pos]) {
		/* hand out state */
		st.pos = 1U;
	} else {
		/* we're conformant */
		st.pos = 0U;
	}
	return st.s;
}

void
nmpr_issn8(nmck_t s, const char *str, size_t len)
{
	issn_state_t st = {s};

	if (LIKELY(!st.pos)) {
		fputs("ISSN, conformant with ISO 3297:1975", stdout);
	} else if (s > 0 && len > 0) {
		fputs("ISSN, not ISO 3297:1975 conformant, should be ", stdout);
		fwrite(str, sizeof(*str), len - 1, stdout);
		fputc(st.chk, stdout);
	} else {
		fputs("unknown", stdout);
	}
	return;
}

nmck_t
nmck_issn13(const char *str, size_t len)
{
	issn_state_t st;

	if (len != 13U) {
		return -1;
	} else if (!issn13p(str, len)) {
		return -1;
	}

	st = calc_issn13(str, len);
	if (!st.s) {
		return -1;
	} else if (st.pos != len - 1U) {
		return -1;
	} else if (st.chk != str[st.pos]) {
		/* hand out state */
		st.pos = 1U;
	} else {
		st.pos = 0U;
	}
	return st.s;
}

void
nmpr_issn13(nmck_t s, const char *str, size_t len)
{
	issn_state_t st = {s};

	if (LIKELY(!st.pos)) {
		fputs("ISSN, conformant with ISO 3297:2007", stdout);
	} else if (s > 0 && len > 0) {
		fputs("ISSN, not ISO 3297:2007 conformant, should be ", stdout);
		fwrite(str, sizeof(*str), len - 1, stdout);
		fputc(st.chk, stdout);
	} else {
		fputs("unknown", stdout);
	}
	return;
}

nmck_t
nmck_issn(const char *str, size_t len)
{
	issn_state_t st;

	/* common cases first */
	if (len < 8U || len > 13U) {
		return -1;
	}

	st = calc_chk(str, len);
	if (!st.s) {
		return -1;
	} else if (st.pos != len - 1U) {
		return -1;
	} else if (st.chk != str[st.pos]) {
		/* record state */
		st.pos = 1U;
	} else {
		st.pos = 0U;
	}
	return st.s;
}

void
nmpr_issn(nmck_t s, const char *str, size_t len)
{
	issn_state_t st = {s};

	if (UNLIKELY(s < 0)) {
	unk:
		fputs("unknown", stdout);
	} else if (LIKELY(!st.pos)) {
		fputs("ISSN, conformant with ", stdout);
		switch (st.std) {
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
		switch (st.std) {
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
		fputc(st.chk, stdout);
	}
	return;
}

/* issn.c ends here */

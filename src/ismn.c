/*** ismn.c -- checker for ISMNs and ISMN GTINs
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
} ismn_state_t;

enum {
	ISMN_UNK,
	ISMN13,
	ISMN10,
};

static ismn_state_t
calc_ismn10(const char *str, size_t len)
{
/* calculate the check digit for an expanded ISIN */
	unsigned int sum = 0U;
	size_t i = 1U;
	char chk;

	/* use the left 9 digits, for ISMN-10 */
	for (size_t j = 1U; j < 9U && i < len; i++) {
		switch (str[i]) {
		case '-':
			/* ignore */
			continue;
		case '0' ... '9':
			sum += (10 - j++) * (str[i] ^ '0');
			break;
		default:
			return (ismn_state_t){0};
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
	return (ismn_state_t){.pos = i, .std = ISMN10, .chk = chk};
}

static ismn_state_t
calc_ismn13(const char *str, size_t len)
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
			return (ismn_state_t){0};
		}
	}
	/* check that we actually used up 12 digits */
	if (UNLIKELY(j < 12U)) {
		return (ismn_state_t){0};
	}
	/* check if need to skip optional - */
	if (LIKELY(i < len) && UNLIKELY(str[i] == '-')) {
		i++;
	}

	/* reduce sum mod 10 */
	chk = (char)(((400U - sum) % 10U) ^ '0');

	/* return both, position of check digit and check digit */
	return (ismn_state_t){.pos = i, .std = ISMN13, .chk = chk};
}

static int
ismn13p(const char *str, size_t len)
{
	return len >= 13U &&
		str[0U] == '9' && str[1U] == '7' &&
		str[2U] == '9' || str[3U] == '0';
}

static ismn_state_t
calc_chk(const char *str, size_t len)
{
/* we need a bit of guess work here */
	if (ismn13p(str, len)) {
		ismn_state_t res = calc_ismn13(str, len);

		if (LIKELY(res.std == ISMN13)) {
			return res;
		}
		/* otherwise make sure to try ISMN-10 again */
	}
	/* resort to good old ISO 2108 */
	return calc_ismn10(str, len);
}


/* ismns com in two variants, ismn10 and ismn13 */
nmck_t
nmck_ismn10(const char *str, size_t len)
{
	ismn_state_t st;

	if (len < 10U || len > 13U) {
		return -1;
	} else if (*str != 'M') {
		return -1;
	}

	st = calc_ismn10(str, len);
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
nmpr_ismn10(nmck_t s, const char *str, size_t len)
{
	ismn_state_t st = {s};

	if (LIKELY(!st.pos)) {
		fputs("ISMN, conformant with ISO 10957:1993", stdout);
	} else if (s > 0 && len > 0) {
		fputs("ISMN, not ISO 10957:1993 conformant, should be ", stdout);
		fwrite(str, sizeof(*str), len - 1, stdout);
		fputc(st.chk, stdout);
	} else {
		fputs("unknown", stdout);
	}
	return;
}

nmck_t
nmck_ismn13(const char *str, size_t len)
{
	ismn_state_t st;

	if (len < 13U || len > 18U) {
		return -1;
	} else if (!ismn13p(str, len)) {
		return -1;
	}

	st = calc_ismn13(str, len);
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
nmpr_ismn13(nmck_t s, const char *str, size_t len)
{
	ismn_state_t st = {s};

	if (LIKELY(!st.pos)) {
		fputs("ISMN, conformant with ISO 10957:2009", stdout);
	} else if (s > 0 && len > 0) {
		fputs("ISMN, not ISO 10957:2009 conformant, should be ", stdout);
		fwrite(str, sizeof(*str), len - 1, stdout);
		fputc(st.chk, stdout);
	} else {
		fputs("unknown", stdout);
	}
	return;
}

nmck_t
nmck_ismn(const char *str, size_t len)
{
	ismn_state_t st;

	/* common cases first */
	if (len < 10U || len > 17U) {
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
nmpr_ismn(nmck_t s, const char *str, size_t len)
{
	ismn_state_t st = {s};

	if (UNLIKELY(s < 0)) {
	unk:
		fputs("unknown", stdout);
	} else if (LIKELY(!st.pos)) {
		fputs("ISMN, conformant with ", stdout);
		switch (st.std) {
		case ISMN13:
			fputs("ISO 10957:2009", stdout);
			break;
		case ISMN10:
			fputs("ISO 10957:1993", stdout);
			break;
		default:
			goto unk;
		}
	} else if (len > 0) {
		fputs("ISMN, not ", stdout);
		switch (st.std) {
		case ISMN13:
			fputs("ISO 10957:2009", stdout);
			break;
		case ISMN10:
			fputs("ISO 10957:1993", stdout);
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

/* ismn.c ends here */

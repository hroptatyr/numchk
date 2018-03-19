/*** lei.c -- checker for ISO 17442 legal entity identifiers
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
 * OR OTHERWISE) ARLEIG IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
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

typedef union {
	nmck_t s;
	struct {
		short unsigned int pad;
		char chk[2U];
	};
} lei_state_t;

static lei_state_t
calc_st(const char *str, size_t UNUSED(len))
{
	char buf[40U];
	size_t bsz = 0U;
	unsigned int sum = 0U;
	size_t j = 0U;

	/* expand string first */
	for (size_t i = 0U; i < 18U; i++) {
		switch (str[i]) {
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
			return (lei_state_t){0};
		}
	}
	/* and 00 */
	buf[bsz++] = '0';
	buf[bsz++] = '0';

	/* now calc first sum */
	sum = (buf[0U] ^ '0') * 10U + (buf[1U] ^ '0');
	for (size_t i = 2U; i < bsz; sum %= 97U) {
		/* just so we calculate less modulos */
		for (const size_t n = i + 7U < bsz ? i + 7U : bsz; i < n; i++) {
			sum *= 10U;
			sum += (buf[i] ^ '0');
		}
	}

	/* this is the actual checksum */
	sum = 98U - sum;

	return (lei_state_t){
		.chk[0U] = (char)((sum / 10U) ^ '0'),
		.chk[1U] = (char)((sum % 10U) ^ '0')
	};
}


nmck_t
nmck_lei(const char *str, size_t len)
{
	/* common cases first */
	if (len != 20) {
		return -1;
	}

	with (lei_state_t st = calc_st(str, len)) {
		if (!st.s) {
			return -1;
		} else if (str[18U] != st.chk[0U] || str[19U] != st.chk[1U]) {
			/* record state */
			return st.s | 1;
		}
	}
	/* all's good */
	return 0;
}

void
nmpr_lei(nmck_t s, const char *str, size_t len)
{
	lei_state_t st = {s};

	if (LIKELY(!s)) {
		fputs("LEI, conformant with ISO 17442:2012", stdout);
	} else if (s > 0 && len == 20U) {
		fputs("LEI, not ISO 17442 conformant, should be ", stdout);
		fwrite(str, sizeof(*str), 18U, stdout);
		fputc(st.chk[0U], stdout);
		fputc(st.chk[1U], stdout);
	} else {
		fputs("unknown", stdout);
	}
	return;
}

/* lei.c ends here */
/*** isin.c -- checker for ISO 6166 security idenfitication numbers
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
#include <stdint.h>
#include <assert.h>
#include "numchk.h"
#include "nifty.h"

/* allowed isin country codes */
#include "isin-cc.c"

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


nmck_t
nmck_isin(const char *str, size_t len)
{
	char buf[24U];
	size_t bsz = 0U;

	if (UNLIKELY(len != 12U)) {
		return -1;
	} else if (!valid_cc_p(str)) {
		return -1;
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
			return -1;
		}
	}
	with (char chk = calc_chk(buf, bsz)) {
		if (chk != str[11U]) {
			/* record state but submit a bid */
			return chk << 1 | 1;
		}
	}
	return 0;
}

void
nmpr_isin(nmck_t st, const char *str, size_t len)
{
	if (!st) {
		fputs("ISIN, conformant with ISO 6166:2013", stdout);
	} else if (st > 0 && len == 12U) {
		fputs("ISIN, not ISO 6166 conformant, should be ", stdout);
		fwrite(str, sizeof(*str), 11U, stdout);
		fputc((char)(st >> 1), stdout);
	} else {
		fputs("unknown", stdout);
	}
	return;
}

/* isin.c ends here */
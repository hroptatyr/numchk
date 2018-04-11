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

#ifdef RAGEL_BLOCK
%%{
	machine numchk;

	lei = upnum{18} (digit{2} | check{2}) %{c(lei)} ;
}%%
#endif	/* RAGEL_BLOCK */


nmck_t
nmck_lei(const char *str, size_t len)
{
	uint_fast8_t buf[40U];
	size_t bsz = 0U;
	uint_fast32_t sum = 0U;
	char chk[2U];

	/* common cases first */
	if (len != 20U) {
		return -1;
	}

	/* expand string first */
	for (size_t i = 0U; i < 18U; i++) {
		switch (str[i]) {
		case '0' ... '9':
			buf[bsz++] = (unsigned char)(str[i] ^ '0');
			break;
		case 'A' ... 'J':
			buf[bsz++] = 1U;
			buf[bsz++] = (unsigned char)(str[i] - 'A');
			break;
		case 'K' ... 'T':
			buf[bsz++] = 2U;
			buf[bsz++] = (unsigned char)(str[i] - 'K');
			break;
		case 'U' ... 'Z':
			buf[bsz++] = 3U;
			buf[bsz++] = (unsigned char)(str[i] - 'U');
			break;
		default:
			return -1;
		}
	}
	/* and 00 */
	buf[bsz++] = 0U;
	buf[bsz++] = 0U;

	/* now calc first sum */
	sum = buf[0U] * 10U + buf[1U];
	for (size_t i = 2U; i < bsz; sum %= 97U) {
		/* just so we calculate less modulos */
		for (const size_t n = i + 7U < bsz ? i + 7U : bsz; i < n; i++) {
			sum *= 10U;
			sum += buf[i];
		}
	}

	/* this is the actual checksum */
	sum = 98U - sum;
	chk[0U] = (char)((sum / 10U) ^ '0');
	chk[1U] = (char)((sum % 10U) ^ '0');

	/* all good, is it */
	return (chk[0U] << 8U ^ chk[1U]) << 8U ^
		(chk[0U] != str[18U] || chk[1U] != str[19U]);
}

void
nmpr_lei(nmck_t s, const char *str, size_t len)
{
	if (LIKELY(!(s & 0b1U))) {
		fputs("LEI, conformant with ISO 17442:2012", stdout);
	} else if (s > 0 && len == 20U) {
		fputs("LEI, not ISO 17442 conformant, should be ", stdout);
		fwrite(str, sizeof(*str), 18U, stdout);
		fputc(s >> 16U & 0x7fU, stdout);
		fputc(s >> 8U & 0x7fU, stdout);
	} else {
		fputs("unknown", stdout);
	}
	return;
}

/* lei.c ends here */

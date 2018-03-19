/*** isan.c -- checker for ISANs
 *
 * Copyright (C) 2017-2018 Sebastian Freundt
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
#include <stdint.h>
#include "numchk.h"
#include "nifty.h"

typedef union {
	nmck_t s;
	struct {
		unsigned char pad;
		unsigned char chk[2U];
		unsigned char len;
	};
} isan_state_t;

static __attribute__((pure, const)) uint_fast32_t
_chex(char c)
{
	if (LIKELY((unsigned char)(c ^ '0') < 10U)) {
		return c ^ '0';
	} else if ((unsigned char)(c | 0x20) - 'W' < 16){
		return (unsigned char)(c | 0x20) - 'W';
	}
	/* no error code */
	return -1U;
}

static inline __attribute__((pure, const)) char
_b36c(char c)
{
	if (LIKELY((unsigned char)c < 10U)) {
		return (char)(c ^ '0');
	}
	/* no check for the upper bound of c */
	return (char)(c + '7');
}

static isan_state_t
calc_isan(const char *str, size_t len)
{
	uint_fast32_t sum = 36U;
	unsigned char chk[2U];
	unsigned char pos;
	char stc[2U];
	size_t i;

	for (size_t j = i = 0U; j < 16U && i < len; i++) {
		uint_fast32_t c;
		if (str[i] == '-') {
			continue;
		} else if ((c = _chex(str[i])) >= 16U) {
			return (isan_state_t){0};
		}
		if ((sum += c) > 36U) {
			sum -= 36U;
		}
		sum *= 2U;
		sum %= 37U;
		j++;
	}
	if (str[i] == '-') {
		i++;
	}
	chk[0U] = _b36c(37U - sum);
	stc[0U] = str[i++];
	pos = i;

	/* possibly more */
	for (size_t j = 0U; j < 8U && i < len; i++) {
		uint_fast32_t c;
		if (str[i] == '-') {
			continue;
		} else if ((c = _chex(str[i])) >= 16U) {
			return (isan_state_t){0};
		}
		if ((sum += c) > 36U) {
			sum -= 36U;
		}
		sum *= 2U;
		sum %= 37U;
		j++;
	}
	if (i < len) {
		if (str[i] == '-') {
			i++;
		}
		chk[1U] = _b36c(37U - sum);
		stc[1U] = str[i++];
	} else {
		chk[1U] = stc[1U] = '\0';
	}
	if (i < len) {
		return (isan_state_t){0};
	}

	return (isan_state_t){
		.pad = (unsigned char)(chk[0U] != stc[0U] || chk[1U] != stc[1U]),
			.chk = {chk[0U], chk[1U]}, .len = pos};
}


nmck_t
nmck_isan(const char *str, size_t len)
{
/* isan is mod 37,36 */
	size_t of = 0U;

	if (len < 16U) {
		return -1;
	}
	if (!memcmp(str, "ISAN", 4U)) {
		of += 4U;
	}
	if (_chex(str[of]) >= 16U) {
		of++;
	}

	with (isan_state_t st = calc_isan(str + of, len - of)) {
		if (!st.s) {
			break;
		}
		return st.s;
	}
	return -1;
}

void
nmpr_isan(nmck_t s, const char *str, size_t len)
{
	isan_state_t st = {s};

	if (LIKELY(!st.pad)) {
		fputs("ISAN, conformant", stdout);
	} else if (s > 0 && len > 0) {
		fputs("ISAN, not conformant, should be ", stdout);
		fwrite(str, 1, st.len - 1U, stdout);
		fputc(st.chk[0U], stdout);
		if (st.chk[1U]) {
			fwrite(str + st.len, 1, len - st.len - 1U, stdout);
			fputc(st.chk[1U], stdout);
		}
	} else {
		fputs("unknown", stdout);
	}
	return;
}

/* isan.c ends here */

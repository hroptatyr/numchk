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


nmck_t
nmck_isan(const char *str, size_t len)
{
/* isan is mod 37,36 */
	uint_fast32_t sum = 36U;
	unsigned char pos;
	char chk[2U];
	size_t i = 0U;

	if (len < 16U) {
		return -1;
	}
	if (!memcmp(str, "ISAN", 4U)) {
		i += 4U;
	}
	i += _chex(str[i]) >= 16U;

	for (size_t j = 0U; j < 16U && i < len; i++) {
		uint_fast32_t c;

		if (str[i] == '-') {
			continue;
		} else if ((c = _chex(str[i])) >= 16U) {
			return -1;
		}
		if ((sum += c) > 36U) {
			sum -= 36U;
		}
		sum *= 2U;
		sum %= 37U;
		j++;
	}
	i += str[i] == '-';
	chk[0U] = _b36c(37U - sum);
	pos = i++;

	/* possibly more */
	for (size_t j = 0U; j < 8U && i < len; i++) {
		uint_fast32_t c;

		if (str[i] == '-') {
			continue;
		} else if ((c = _chex(str[i])) >= 16U) {
			return -1;
		}
		if ((sum += c) > 36U) {
			sum -= 36U;
		}
		sum *= 2U;
		sum %= 37U;
		j++;
	}
	i += i < len && str[i] == '-';
	i += i < len;
	chk[1U] = _b36c(37U - sum);
	if (UNLIKELY(i < len)) {
		return -1;
	}

	return ((chk[0U] << 8U ^ chk[1U]) << 8U ^ pos) << 8U ^
		(chk[0U] != str[pos] || chk[1U] != str[i - 1U]);
}

void
nmpr_isan(nmck_t s, const char *str, size_t len)
{
	if (LIKELY(!(s & 0b1U))) {
		fputs("ISAN, conformant", stdout);
	} else if (s > 0 && len > 0) {
		size_t pos = s >> 8U & 0x7fU;
		fputs("ISAN, not conformant, should be ", stdout);
		fwrite(str, 1, pos, stdout);
		fputc(s >> 24U & 0x7fU, stdout);
		if (pos + 1U < len) {
			fwrite(str + pos + 1, 1, len - (pos + 1U) - 1U, stdout);
			fputc(s >> 16U & 0x7fU, stdout);
		}
	} else {
		fputs("unknown", stdout);
	}
	return;
}

/* isan.c ends here */

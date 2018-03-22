/*** euvatid.c -- checker for European VAT-ids
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
#include "numchk.h"
#include "nifty.h"


nmck_t
nmck_devatid(const char *str, size_t len)
{
	uint_fast32_t sum = 10U;
	uint_fast32_t stc;
	size_t i = 0U;

	/* common cases first */
	if (len < 9U || len > 14U) {
		return -1;
	}

	i += str[i] == 'D';
	i += str[i] == 'E';
	for (size_t j = 0U; j < 8U && i < len; i++) {
		uint_fast32_t c = (str[i] ^ '0');

		if (str[i] == ' ') {
			continue;
		} else if (UNLIKELY(c >= 10U)) {
			return -1;
		}
		sum += c;
		sum = (sum + 9U/*-1 mod 10*/) % 10U + 1U;
		sum *= 2U;
		sum %= 11U;
		j++;
	}
	sum = (11U - sum) % 10U;
	sum ^= '0';
	stc = str[i++];

	if (i < len) {
		/* more? */
		return -1;
	}

	return sum << 1U ^ (stc != sum);
}

void
nmpr_devatid(nmck_t s, const char *str, size_t len)
{
	if (LIKELY(!(s & 0b1U))) {
		fputs("German VAT-ID, conformant", stdout);
	} else if (s > 0 && len > 0) {
		fputs("German VAT-ID, not conformant, should be ", stdout);
		fwrite(str, sizeof(*str), len - 1U, stdout);
		fputc(s >> 1U & 0x7fU, stdout);
	} else {
		fputs("unknown", stdout);
	}
	return;
}

nmck_t
nmck_bevatid(const char *str, size_t len)
{
	uint_fast32_t sum = 0U;
	char chk[2U];
	size_t i = 0U;

	/* common cases first */
	if (len < 9U || len > 14U) {
		return -1;
	}

	i += str[i] == 'B';
	i += str[i] == 'E';
	for (; i < len - 2U; i++) {
		uint_fast32_t c = (str[i] ^ '0');

		if (str[i] == '.') {
			continue;
		} else if (UNLIKELY(c >= 10U)) {
			return -1;
		}
		sum *= 10U;
		sum += c;
	}
	sum %= 97U;
	sum = 97U - sum;
	chk[0U] = (char)((sum / 10U) ^ '0');
	chk[1U] = (char)((sum % 10U) ^ '0');

	return ((chk[0U] << 8U) ^ chk[1U]) << 1U ^
		(chk[0U] != str[len - 2U] || chk[1U] != str[len - 1U]);
}

void
nmpr_bevatid(nmck_t s, const char *str, size_t len)
{
	if (LIKELY(!(s & 0b1U))) {
		fputs("Belgian VAT-ID, conformant", stdout);
	} else if (s > 0 && len >= 2U) {
		uint_fast32_t c = s >> 1U;
		fputs("Belgian VAT-ID, not conformant, should be ", stdout);
		fwrite(str, sizeof(*str), len - 2U, stdout);
		fputc(c >> 8U & 0x7fU, stdout);
		fputc(c >> 0U & 0x7fU, stdout);
	} else {
		fputs("unknown", stdout);
	}
	return;
}

nmck_t
nmck_dkvatid(const char *str, size_t len)
{
	uint_fast32_t sum = 0U;
	size_t i = 0U;

	/* common cases first */
	if (len < 8U || len > 11U) {
		return -1;
	}

	i += str[i] == 'D';
	i += str[i] == 'K';
	with (uint_fast32_t c = (str[i++] ^ '0')) {
		if (UNLIKELY(c >= 10U)) {
			return -1;
		}
		sum += 2U * c;
	}
	for (size_t w = 7U; w > 0U && i < len; i++, w--) {
		uint_fast32_t c = (str[i] ^ '0');

		if (UNLIKELY(w == 1U && ischeck(str[i]))) {
			;
		} else if (UNLIKELY(c >= 10U)) {
			return -1;
		}
		sum += w * c;
	}
	if (sum %= 11U) {
		/* force divisibility by 11 */
		const unsigned char c = (unsigned char)(str[i - 1U] ^ '0');
		sum = c + (11U - sum);
		sum %= 11U;
		sum ^= '0';
	}

	return sum << 1U ^ (sum > 0);
}

void
nmpr_dkvatid(nmck_t s, const char *str, size_t len)
{
	if (LIKELY(!(s & 0b1U))) {
		fputs("Danish VAT-ID, conformant", stdout);
	} else if (s > 0 && len >= 2U) {
		fputs("Danish VAT-ID, not conformant, should be ", stdout);
		fwrite(str, sizeof(*str), len - 1U, stdout);
		fputc(s >> 1U & 0x7fU, stdout);
	} else {
		fputs("unknown", stdout);
	}
	return;
}

nmck_t
nmck_fivatid(const char *str, size_t len)
{
/* mod 11,2 */
	uint_fast32_t sum = 0U;
	size_t i = 0U;

	/* common cases first */
	if (len < 8U || len > 11U) {
		return -1;
	}

	i += str[i] == 'F';
	i += str[i] == 'I';
	for (size_t w = 7U; i < len; i++, w = (w + 11U * (w & 0b1U)) / 2U) {
		uint_fast32_t c = (str[i] ^ '0');

		if (UNLIKELY(w == 1U && ischeck(str[i]))) {
			;
		} else if (UNLIKELY(c >= 10U)) {
			return -1;
		}
		sum += w * c;
	}
	if (sum %= 11U) {
		/* force divisibility by 11 */
		const unsigned char c = (unsigned char)(str[i - 1U] ^ '0');
		sum = c + (11U - sum);
		sum %= 11U;
		sum ^= '0';
	}

	return sum << 1U ^ (sum > 0);
}

void
nmpr_fivatid(nmck_t s, const char *str, size_t len)
{
	if (LIKELY(!(s & 0b1U))) {
		fputs("Finnish VAT-ID, conformant", stdout);
	} else if (s > 0 && len >= 2U) {
		fputs("Finnish VAT-ID, not conformant, should be ", stdout);
		fwrite(str, sizeof(*str), len - 1U, stdout);
		fputc(s >> 1U & 0x7fU, stdout);
	} else {
		fputs("unknown", stdout);
	}
	return;
}

nmck_t
nmck_frvatid(const char *str, size_t len)
{
	uint_fast32_t sum = 0U;
	uint_fast32_t ccs = 0U;
	size_t i = 0U;

	/* common cases first */
	if (len < 8U || len > 17U) {
		return -1;
	}

	i += str[i] == 'F';
	i += str[i] == 'R';
	i += str[i] == ' ';
	/* next two are check digits */
	for (size_t j = 0U; j < 2U; i++) {
		uint_fast32_t c = (str[i] ^ '0');

		if (ischeck(str[i])) {
			;
		} else if (UNLIKELY(c >= 10U)) {
			return -1;
		}
		ccs *= 10U;
		ccs += c;
		j++;
	}		
	for (; i < len; i++) {
		uint_fast32_t c = (str[i] ^ '0');

		if (UNLIKELY(c >= 0x20U)) {
			return -1;
		} else if (c >= 10U) {
			continue;
		}
		sum *= 10U;
		sum += c;
	}
	sum %= 97U;
	sum *= 3U;
	sum += 12U;
	sum %= 97U;

	return sum << 1U ^ (sum != ccs);
}

void
nmpr_frvatid(nmck_t s, const char *str, size_t len)
{
	if (LIKELY(!(s & 0b1U))) {
		fputs("French VAT-ID, conformant", stdout);
	} else if (s > 0 && len >= 4U) {
		uint_fast32_t c = s >> 1U;

		fputs("French VAT-ID, not conformant, should be ", stdout);
		fwrite(str, sizeof(*str), 2U, stdout);
		fputc((c / 10U) ^ '0', stdout);
		fputc((c % 10U) ^ '0', stdout);
		fwrite(str + 4U, sizeof(*str), len - 4U, stdout);
	} else {
		fputs("unknown", stdout);
	}
	return;
}

/* euvatid.c ends here */

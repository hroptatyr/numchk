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

static inline int
isdigit(int x)
{
	return (unsigned char)(x ^ '0') < 10U;
}


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
	} else if (s > 0 && len >= 3U) {
		uint_fast32_t c = s >> 1U;
		size_t plen = 0U;

		plen += (str[0U] == 'F') && str[1U] == 'R';
		plen += (str[0U] == 'F') && str[1U] == 'R';
		plen += str[plen] == ' ';

		fputs("French VAT-ID, not conformant, should be ", stdout);
		fwrite(str, sizeof(*str), plen, stdout);
		fputc((c / 10U) ^ '0', stdout);
		fputc((c % 10U) ^ '0', stdout);
		fwrite(str + plen + 2U, sizeof(*str), len - (plen + 2U), stdout);
	} else {
		fputs("unknown", stdout);
	}
	return;
}

nmck_t
nmck_grvatid(const char *str, size_t len)
{
	uint_fast32_t sum = 0U;
	size_t i = 0U;

	/* common cases first */
	if (len < 8U || len > 11U) {
		return -1;
	}
	if (str[0U] == 'G' && str[1U] == 'R' ||
	    str[0U] == 'E' && str[1U] == 'L') {
		i += 2U;
	}
	i += str[i] == ' ';
	for (size_t j = 0U; i < len && j < 8U; i++, j++) {
		uint_fast32_t c = (str[i] ^ '0');

		sum += c;
		sum *= 2U;
	}
	sum %= 11U;
	sum %= 10U;
	sum ^= '0';

	return sum << 1U ^ ((char)sum != str[len - 1U]);
}

void
nmpr_grvatid(nmck_t s, const char *str, size_t len)
{
	if (LIKELY(!(s & 0b1U))) {
		fputs("Greek VAT-ID, conformant", stdout);
	} else if (s > 0 && len > 0U) {
		fputs("Greek VAT-ID, not conformant, should be ", stdout);
		fwrite(str, sizeof(*str), len - 1U, stdout);
		fputc(s >> 1U & 0x7fU, stdout);
	} else {
		fputs("unknown", stdout);
	}
	return;
}

nmck_t
nmck_ievatid(const char *str, size_t len)
{
	static const char r[] = "WABCDEFGHIJKLMNOPQRSTUV";
	uint_fast32_t sum = 0U, w = 8U, v = 0U;
	size_t i = 0U;

	/* common cases first */
	if (len < 8U || len > 12U) {
		return -1;
	}
	if (str[0U] == 'I' && str[1U] == 'E') {
		i += 2U;
	}
	i += str[i] == ' ';
	if (!isdigit(str[i + 0U])) {
		return -1;
	} else if (!isdigit(str[i + 1U])) {
		/* one of them very old ones */
		sum += 2U * (unsigned char)(str[i] ^ '0');
		i += 2U;
		v++;
	}
	for (; i < len - 1U && (w - v) >= 2U; i++, w--) {
		uint_fast32_t c = (str[i] ^ '0');

		if (c >= 10U) {
			return -1;
		}
		sum += (w + v) * c;
	}
	if (i + 1U < len) {
		/* 2013+ scheme */
		uint_fast32_t c;

		if (str[i] < 'A' || str[i] > 'W') {
			return -1;
		}
		c = strchr(r, str[i]) - r;
		sum += 9U * c;
		i++;
	}
	sum %= 23U;
	sum = (unsigned char)r[sum];

	return sum << 1U ^ ((char)sum != str[len - 1U]);
}

void
nmpr_ievatid(nmck_t s, const char *str, size_t len)
{
	if (LIKELY(!(s & 0b1U))) {
		fputs("Irish VAT-ID, conformant", stdout);
	} else if (s > 0 && len > 0U) {
		fputs("Irish VAT-ID, not conformant, should be ", stdout);
		fwrite(str, sizeof(*str), len - 1U, stdout);
		fputc(s >> 1U & 0x7fU, stdout);
	} else {
		fputs("unknown", stdout);
	}
	return;
}

nmck_t
nmck_itvatid(const char *str, size_t len)
{
/* luhn */
	uint_fast32_t sum = 0U;
	size_t i = 0U;

	/* common cases first */
	if (len < 11U || len > 14U) {
		return -1;
	}
	if (str[0U] == 'I' && str[1U] == 'T') {
		i += 2U;
	}
	i += str[i] == ' ';

	/* odd number of digits in total, so start weighting at 1U */
	for (size_t k = 1U; i < len - 1U; i++, k ^= 3U) {
		uint_fast32_t c = str[i] ^ '0';

		if (UNLIKELY(c >= 10U)) {
			return -1;
		}
		sum += k * c;
		sum += k * c >= 10U;
	}
	sum = 200U - sum;
	sum %= 10U;
	sum ^= '0';

	return sum << 1U ^ ((char)sum != str[len - 1U]);
}

void
nmpr_itvatid(nmck_t s, const char *str, size_t len)
{
	if (LIKELY(!(s & 0b1U))) {
		fputs("Italian VAT-ID, conformant", stdout);
	} else if (s > 0 && len > 0U) {
		fputs("Italian VAT-ID, not conformant, should be ", stdout);
		fwrite(str, sizeof(*str), len - 1U, stdout);
		fputc(s >> 1U & 0x7fU, stdout);
	} else {
		fputs("unknown", stdout);
	}
	return;
}

nmck_t
nmck_luvatid(const char *str, size_t len)
{
/* simply mod 89 */
	uint_fast32_t sum = 0U;
	uint_fast32_t chk[2U];
	size_t i = 0U;

	/* common cases first */
	if (len < 8U || len > 13U) {
		return -1;
	}
	if (str[0U] == 'L' && str[1U] == 'U') {
		i += 2U;
	}
	i += str[i] == ' ';

	/* odd number of digits in total, so start weighting at 1U */
	for (; i < len - 2U; i++) {
		uint_fast32_t c = str[i] ^ '0';

		if (str[i] == ' ') {
			continue;
		} else if (UNLIKELY(c >= 10U)) {
			return -1;
		}
		sum *= 10U;
		sum += c;
	}
	sum %= 89U;
	chk[0U] = sum / 10U ^ '0';
	chk[1U] = sum % 10U ^ '0';

	return (chk[0U] << 8U ^ chk[1U]) << 8U ^
		((char)chk[0U] != str[len - 2U] || (char)chk[1U] != str[len - 1U]);
}

void
nmpr_luvatid(nmck_t s, const char *str, size_t len)
{
	if (LIKELY(!(s & 0b1U))) {
		fputs("Luxembourgian VAT-ID, conformant", stdout);
	} else if (s > 0 && len > 1U) {
		fputs("Luxembourgian VAT-ID, not conformant, should be ", stdout);
		fwrite(str, sizeof(*str), len - 2U, stdout);
		fputc(s >> 16U & 0x7fU, stdout);
		fputc(s >> 8U & 0x7fU, stdout);
	} else {
		fputs("unknown", stdout);
	}
	return;
}

nmck_t
nmck_nlvatid(const char *str, size_t len)
{
	uint_fast32_t sum = 0U;
	size_t i = 0U;

	/* common cases first */
	if (len < 12U || len > 14U) {
		return -1;
	}
	if (str[0U] == 'N' && str[1U] == 'L') {
		i += 2U;
	}

	for (size_t j = 9U; i < len - 4U; j--, i++) {
		uint_fast32_t c = str[i] ^ '0';

		if (UNLIKELY(c >= 10U)) {
			return -1;
		}
		sum += j * c;
	}
	if (UNLIKELY(str[++i] != 'B')) {
		return -1;
	}
	if (UNLIKELY((sum %= 11U) >= 10U)) {
		/* they wouldn't hand this out, would they? */
		return -1;
	}
	sum ^= '0';

	return sum << 1U ^ ((char)sum != str[len - 4U]);
}

void
nmpr_nlvatid(nmck_t s, const char *str, size_t len)
{
	if (LIKELY(!(s & 0b1U))) {
		fputs("Dutch VAT-ID, conformant", stdout);
	} else if (s > 0 && len > 3U) {
		fputs("Dutch VAT-ID, not conformant, should be ", stdout);
		fwrite(str, sizeof(*str), len - 4U, stdout);
		fputc(s >> 1U & 0x7fU, stdout);
		fwrite(str + len - 3U, sizeof(*str), 3U, stdout);
	} else {
		fputs("unknown", stdout);
	}
	return;
}

nmck_t
nmck_atvatid(const char *str, size_t len)
{
/* pretty much luhn, twist is to subtract from 96 */
	uint_fast32_t sum = 0U;
	size_t i = 0U;

	/* common cases first */
	if (len < 9U || len > 12U) {
		return -1;
	}
	if (str[0U] == 'A' && str[1U] == 'T') {
		i += 2U;
	}
	i += str[i] == ' ';
	if (UNLIKELY(str[i++] != 'U')) {
		return -1;
	}
	for (size_t k = 1U; i < len - 1U; i++, k ^= 3U) {
		uint_fast32_t c = str[i] ^ '0';

		if (UNLIKELY(c >= 10U)) {
			return -1;
		}
		sum += k * c;
		sum += k * c >= 10U;
	}
	sum = 96U - sum;
	sum %= 10U;
	sum ^= '0';

	return sum << 1U ^ ((char)sum != str[len - 1U]);
}

void
nmpr_atvatid(nmck_t s, const char *str, size_t len)
{
	if (LIKELY(!(s & 0b1U))) {
		fputs("Austrian VAT-ID, conformant", stdout);
	} else if (s > 0 && len > 3U) {
		fputs("Austrian VAT-ID, not conformant, should be ", stdout);
		fwrite(str, sizeof(*str), len - 1U, stdout);
		fputc(s >> 1U & 0x7fU, stdout);
	} else {
		fputs("unknown", stdout);
	}
	return;
}

nmck_t
nmck_plvatid(const char *str, size_t len)
{
	static const uint_fast32_t w[] = {6U, 5U, 7U, 2U, 3U, 4U, 5U, 6U, 7U};
	uint_fast32_t sum = 0U;
	size_t i = 0U;

	/* common cases first */
	if (len < 10U || len > 13U) {
		return -1;
	}
	if (str[0U] == 'P' && str[1U] == 'L') {
		i += 2U;
	}
	i += str[i] == ' ';
	for (size_t j = 0U; j < 9U && i < len - 1U; i++, j++) {
		uint_fast32_t c = str[i] ^ '0';

		if (UNLIKELY(c >= 10U)) {
			return -1;
		}
		sum += w[j] * c;
	}
	if (UNLIKELY((sum %= 11U) >= 10U)) {
		return -1;
	}
	sum ^= '0';

	return sum << 1U ^ ((char)sum != str[len - 1U]);
}

void
nmpr_plvatid(nmck_t s, const char *str, size_t len)
{
	if (LIKELY(!(s & 0b1U))) {
		fputs("Polish VAT-ID, conformant", stdout);
	} else if (s > 0 && len > 3U) {
		fputs("Polish VAT-ID, not conformant, should be ", stdout);
		fwrite(str, sizeof(*str), len - 1U, stdout);
		fputc(s >> 1U & 0x7fU, stdout);
	} else {
		fputs("unknown", stdout);
	}
	return;
}

nmck_t
nmck_ptvatid(const char *str, size_t len)
{
	uint_fast32_t sum = 0U;
	size_t i = 0U;

	/* common cases first */
	if (len < 9U || len > 14U) {
		return -1;
	}
	if (str[0U] == 'P' && str[1U] == 'T') {
		i += 2U;
	}
	i += str[i] == ' ';

	/* snarf 8 digits */
	for (size_t j = 1U; j < 9U && i < len - 1U; i++) {
		uint_fast32_t c = str[i] ^ '0';

		if (str[i] == ' ') {
			continue;
		} else if (UNLIKELY(c >= 10U)) {
			return -1;
		}
		sum += ++j * c;
	}
	sum %= 11U;
	sum %= 10U;
	sum ^= '0';

	return sum << 1U ^ ((char)sum != str[len - 1U]);
}

void
nmpr_ptvatid(nmck_t s, const char *str, size_t len)
{
	if (LIKELY(!(s & 0b1U))) {
		fputs("Portuguese VAT-ID, conformant", stdout);
	} else if (s > 0 && len > 3U) {
		fputs("Portuguese VAT-ID, not conformant, should be ", stdout);
		fwrite(str, sizeof(*str), len - 1U, stdout);
		fputc(s >> 1U & 0x7fU, stdout);
	} else {
		fputs("unknown", stdout);
	}
	return;
}

/* euvatid.c ends here */

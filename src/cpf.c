/*** cpf.c -- checker for Brazil's Cadastro de Pessoas Fisicas
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
#include <stdint.h>
#include <assert.h>
#include "numchk.h"
#include "nifty.h"


nmck_t
nmck_cpf(const char *str, size_t len)
{
/* this is Verhoeff */
	uint_fast32_t s1 = 0U, s2 = 0U;
	size_t i = 0U;

	if (UNLIKELY(len < 11U || len > 14U)) {
		return -1;
	}

	for (uint_fast32_t j = 1U; i < len && j < 10U; i++) {
		uint_fast32_t c = (unsigned char)(str[i] ^ '0');

		if (str[i] == '.') {
			continue;
		} else if (UNLIKELY(c > 10U)) {
			return -1;
		}
		s1 += c * j++;
	}
	/* invert prod */
	s1 %= 11U;
	s1 %= 10U;

	for (uint_fast32_t j = i = 1U; i < len - 2U && j < 9U; i++) {
		uint_fast32_t c = (unsigned char)(str[i] ^ '0');

		if (str[i] == '.') {
			continue;
		} else if (UNLIKELY(c > 10U)) {
			return -1;
		}
		s2 += c * j++;
	}
	s2 += s1 * 9U;
	s2 %= 11U;
	s2 %= 10U;
	s1 ^= '0';
	s2 ^= '0';

	return (s1 << 8U ^ s2) << 1U ^
		((char)s1 != str[len - 2U] || (char)s2 != str[len - 1U]);
}

void
nmpr_cpf(nmck_t s, const char *str, size_t len)
{
	if (!(s & 0b1U)) {
		fputs("CPF, conformant", stdout);
	} else if (s > 0 && len > 2U) {
		fputs("CPF, not conformant, should be ", stdout);
		fwrite(str, sizeof(*str), len - 2U, stdout);
		fputc(s >> 9 & 0x7f, stdout);
		fputc(s >> 1 & 0x7f, stdout);
	} else {
		fputs("unknown", stdout);
	}
	return;
}

/* cpf.c ends here */

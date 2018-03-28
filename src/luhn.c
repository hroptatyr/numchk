/*** luhn.c -- generic luhn checker
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


nmck_t
nmck_luhn(const char *str, size_t len)
{
	uint_fast32_t dbl[2U] = {0U, 0U};
	uint_fast32_t one[2U] = {0U, 0U};
	uint_fast32_t sum;
	size_t k;

	if (len < 2U) {
		/* no need to check single digits, nor no digits */
		return -1;
	}

	for (size_t i = k = 0U; i < len; i++, k ^= 1U) {
		uint_fast32_t c = (unsigned char)(str[i] ^ '0');

		if (UNLIKELY(c >= 10U)) {
			return -1;
		}
		dbl[k] += 2U * c;
		one[k] += 2U * c >= 10U;
	}
	/* decide now which sum was the 2-weighted one */
	sum = dbl[k ^ 1U] / 2U + dbl[k] + one[k];
	sum %= 10U;

	return !sum - 1;
}

void
nmpr_luhn(nmck_t s, const char *UNUSED(str), size_t UNUSED(len))
{
	if (!s) {
		fputs("Luhn secured number, check passed", stdout);
	}
	return;
}

/* luhn.c ends here */

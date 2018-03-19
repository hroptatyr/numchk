/*** gtin.c -- checker for GS1's GTINs
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
		unsigned char pad;
		unsigned char len;
		unsigned char chk;
	};
} gtin_state_t;

static gtin_state_t
calc_chk(const char *str, size_t len)
{
/* calculate the check digit for an expanded ISIN */
	unsigned int sum = 0U;

	/* use the left len - 1 digits, start with  the evens */
	for (size_t i = len % 2U; i < len - 1U; i += 2U) {
		if (UNLIKELY(str[i] < '0' || str[i] > '9')) {
			return (gtin_state_t){0};
		}
		sum += 3U * (str[i] ^ '0');
	}
	/* odd numbers now */
	for (size_t i = !(len % 2U); i < len - 1U; i += 2U) {
		if (UNLIKELY(str[i] < '0' || str[i] > '9')) {
			return (gtin_state_t){0};
		}
		sum += str[i] ^ '0';
	}

	/* sum can be at most 252, so check digit is */
	return (gtin_state_t){
		.len = len,
		.chk = (char)(((400U - sum) % 10U) ^ '0')
	};
}


nmck_t
nmck_gtin(const char *str, size_t len)
{
	/* common cases first */
	if (len < 8U || len > 14U) {
		return -1;
	}

	with (gtin_state_t st = calc_chk(str, len)) {
		if (!st.chk) {
			return -1;
		}
		switch (st.len) {
		case 8:
		case 12:
		case 13:
		case 14:
			break;
		default:
			return -1;
		}
		if (st.chk != str[len - 1U]) {
			/* record state */
			return st.s | 1;
		}
		return st.s;
	}
	return -1;
}

void
nmpr_gtin(nmck_t s, const char *str, size_t len)
{
	gtin_state_t st = {s};

	if (LIKELY(!st.pad)) {
		fprintf(stdout, "GTIN%d, conformant", st.len);
	} else if (s > 0 && len > 0) {
		fprintf(stdout, "GTIN%d, not conformant, should be ", st.len);
		fwrite(str, sizeof(*str), len - 1U, stdout);
		fputc(st.chk, stdout);
	} else {
		fputs("unknown", stdout);
	}
	return;
}

/* gtin.c ends here */

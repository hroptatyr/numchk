/*** bic.c -- checker for ISO 9362 BICs
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
/* allowed BIC country codes */
#include "bic-cc.c"


nmck_t
nmck_bic(const char *str, size_t len)
{
	if (UNLIKELY(!(len == 8U || len == 11U || len == 9U || len == 12U))) {
		return -1;
	} else if (!valid_cc_p(str + 4U)) {
		return -1;
	}

	/* first 4 must be alpha */
	for (size_t i = 0U; i < 4U; i++) {
		if (!(str[i] >= 'A' && str[i] <= 'Z')) {
			return -1;
		}
	}

	for (size_t i = 6U; i < len; i++) {
		if ((unsigned char)(str[i] ^ '0') < 10U) {
			;
		} else if (!(str[i] >= 'A' && str[i] <= 'Z')) {
			return -1;
		}
	}
	return len == 9 || len == 12U;
}

void
nmpr_bic(nmck_t s, const char *UNUSED(str), size_t UNUSED(len))
{
	if (!s) {
		fputs("BIC, conformant with ISO 9362:2009", stdout);
	} else if (s > 0) {
		fputs("FIN address, Swift conformant", stdout);
	} else {
		fputs("unknown", stdout);
	}
	return;
}

/* bic.c ends here */

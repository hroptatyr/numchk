/*** credcard.c -- checker for credit card account numbers
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

typedef enum {
	ISS_UNK,
	ISS_AMEX,
	ISS_BANKCARD,
	ISS_CUP,
	ISS_DC_CB,
	ISS_DC_ENROUTE,
	ISS_DC_INTL,
	ISS_DC,
	ISS_DISC,
	ISS_INTPAY,
	ISS_INSTAP,
	ISS_JCB,
	ISS_LASER,
	ISS_MAESTRO,
	ISS_DANKORT,
	ISS_MASTER,
	ISS_SOLO,
	ISS_SWITCH,
	ISS_VISA,
	ISS_VELEC,
	ISS_UATP,
	NISSUERS,
} cc_issuer_t;

static const char *issuers[] = {
	[ISS_UNK] = "unknown issuer",
	[ISS_AMEX] = "American Express",
	[ISS_BANKCARD] = "Bankcard",
	[ISS_CUP] = "China UnionPay",
	[ISS_DC_CB] = "Diners Club Carte Blanche",
	[ISS_DC_ENROUTE] = "Diners Club enRoute",
	[ISS_DC_INTL] = "Diners Club International",
	[ISS_DC] = "Diners Club",
	[ISS_DISC] = "Discover Card",
	[ISS_INTPAY] = "InterPayment",
	[ISS_INSTAP] = "InstaPayment",
	[ISS_JCB] = "JCB",
	[ISS_LASER] = "Laser",
	[ISS_MAESTRO] = "Maestro",
	[ISS_DANKORT] = "Dankort",
	[ISS_MASTER] = "MasterCard",
	[ISS_SOLO] = "Solo",
	[ISS_SWITCH] = "Switch",
	[ISS_VISA] = "Visa",
	[ISS_VELEC] = "Visa Electron",
	[ISS_UATP] = "UATP",
};

#ifdef RAGEL_BLOCK
%%{
	machine numchk;

	credcard = (digit | " "){12,22} %{c(credcard)} ;
}%%
#endif	/* RAGEL_BLOCK */


/* this will always yield a non-0 result in order to track the issuer */
nmck_t
nmck_credcard(const char *str, size_t len)
{
	uint_fast32_t dbl[2U] = {0U, 0U};
	uint_fast32_t one[2U] = {0U, 0U};
	uint_fast32_t sum;
	cc_issuer_t iss;
	size_t k;

	/* common cases first */
	if (len < 12U || len > 19U + 3U) {
		return -1;
	}
	/* just calc checksum first and sort through issuers later */
	for (size_t i = k = 0U; i < len - 1U; i++) {
		uint_fast32_t c = (unsigned char)(str[i] ^ '0');

		if (str[i] == ' ') {
			continue;
		} else if (UNLIKELY(c >= 10U)) {
			return -1;
		}
		dbl[k & 0b1U] += 2U * c;
		one[k & 0b1U] += 2U * c >= 10U;
		k++;
	}
	/* decide now which sum was the 2-weighted one */
	k++;
	sum = dbl[(k & 0b1U) ^ 1U] / 2U + dbl[k & 0b1U] + one[k & 0b1U];
	sum = 100000U - sum;
	sum %= 10U;
	sum ^= '0';

	switch (str[0U]) {
	case '1':
		/* uatp */
		if (k == 15U) {
			iss = ISS_UATP;
			goto final;
		}
		break;

	case '2':
		/* master, diners */
		switch (str[1U]) {
		case '0':
		case '1':
			if (k == 15U) {
				iss = ISS_DC_ENROUTE;
				goto final;
			}
			break;
		case '2' ... '7':
			if (k == 16U) {
				iss = ISS_MASTER;
				goto final;
			}
			break;
		default:
			break;
		}
			
	case '3':
		/* amex, jcb, carte blanche */
		switch (str[1U]) {
		case '4':
		case '7':
			if (k == 15U) {
				iss = ISS_AMEX;
				goto final;
			}
			break;

		case '5':
			/* jcb */
			if (k == 16U &&
			    (str[2U] >= '2' && str[2U] <= '8')) {
				iss = ISS_JCB;
				goto final;
			}
			break;

		case '0':
			if (k == 14U &&
			    (str[2U] >= '0' && str[2U] <= '5')) {
				iss = ISS_DC_CB;
				goto final;
			}
			/*@fallthrough@*/
		case '6':
		case '8':
		case '9':
			if (k == 14U) {
				iss = ISS_DC_INTL;
				goto final;
			}
			break;
		default:
			break;
		}
		break;

	case '4':
		/* visa */
		if (k == 16U &&
		    (!memcmp(str, "4026", 4U) ||
		     !memcmp(str, "4175", 4U) ||
		     !memcmp(str, "4405", 4U) ||
		     !memcmp(str, "4508", 4U) ||
		     !memcmp(str, "4844", 4U) ||
		     !memcmp(str, "4913", 4U) ||
		     !memcmp(str, "4917", 4U))) {
			iss = ISS_VELEC;
			goto final;
		} else if (k == 13U || k == 16U) {
			iss = ISS_VISA;
			goto final;
		}
		break;

	case '5':
		/* master, diners */
		switch (str[1U]) {
		case '0':

		case '1' ... '5':
			if (k == 16U) {
				iss = ISS_MASTER;
				goto final;
			}
			break;
		case '6' ... '9':
		maestro:
			if (k >= 12U && k <= 19U) {
				iss = ISS_MAESTRO;
				goto final;
			}
			break;
		default:
			break;
		}
		break;

	case '6':
		/* rest */
		switch (str[1U]) {
		case '2':
			if (k >= 16U && k <= 19U) {
				iss = ISS_CUP;
				goto final;
			}
			break;
		case '0':
		case '4':
		case '5':
			if (k == 16U) {
				iss = ISS_DISC;
				goto final;
			}
			break;
		case '3':
			switch (str[2U]) {
			case '6':
				if (k >= 16U && k <= 19U) {
					iss = ISS_INTPAY;
					goto final;
				}
				break;
			case '7' ... '9':
				if (k == 16U) {
					iss = ISS_INSTAP;
					goto final;
				}
				break;
			default:
				goto maestro;
			}
			/* rest could be maestro, @fallthrough@ */
		case '1':
		case '6':
		case '7':
		case '8':
		case '9':
			goto maestro;
		default:
			break;
		}

	default:
		break;
	}
	/* we expect valid numbers to branch to the final label */
	return -1;

final:
	return (sum << 8U ^ iss) << 8U ^ ((char)sum != str[len - 1U]);
}

void
nmpr_credcard(nmck_t s, const char *str, size_t len)
{
	if (s < 0) {
	unk:
		fputs("unknown", stdout);
	} else {
		unsigned int iss = s >> 8U & 0xffU;
		if (iss < NISSUERS) {
			fputs(issuers[iss], stdout);
		} else {
			fprintf(stdout, "issuer %u", iss);
		}
		if (LIKELY(!(s & 0b1U))) {
			fputs(", conformant account number", stdout);
		} else if (len > 1U) {
			fputs(", non-conformant account number, should be ", stdout);
			fwrite(str, sizeof(*str), len - 1U, stdout);
			fputc(s >> 16U & 0x7fU, stdout);
		} else {
			goto unk;
		}
	}
	return;
}

/* credcard.c ends here */

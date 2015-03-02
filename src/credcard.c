/*** credcard.c -- checker for credit card account numbers
 *
 * Copyright (C) 2014-2015 Sebastian Freundt
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
#include "credcard.h"


typedef union {
	unsigned int s;
	struct {
		short unsigned int pos;
		unsigned char issuer;
		char chk;
	};
} cc_state_t;

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
} cc_issuer_t;

static const nmck_bid_t nul_bid;
static const cc_state_t nul_state;

static const char *issuers[] = {
	[ISS_UNK] = "unknown",
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

static char
calc_chk(const char *str, size_t len)
{
/* this one is just the numbers */
	unsigned int sum = 0U;

	/* don't check last digit */
	len--;

	for (size_t i = !(len % 2U); i < len; i += 2U) {
		static uint_fast8_t s[] = {0, 2, 4, 6, 8, 1, 3, 5, 7, 9};
		sum += s[(str[i] ^ '0')];
	}
	for (size_t i = (len % 2U); i < len; i += 2U) {
		sum += (str[i] ^ '0');
	}
	return (char)(((360U - sum) % 10U) ^ '0');
}

static cc_state_t
calc_st(const char *str, size_t len)
{
	char buf[24U];
	size_t i, j;

	for (i = 0U, j = 0U; j < sizeof(buf) && i < len; i++) {
		switch (str[i]) {
		case ' ':
			/* ignore */
			continue;
		case '0' ... '9':
			buf[j++] = str[i];
			break;
		default:
			return nul_state;
		}
	}
	if (UNLIKELY(j < 12U || j > 19U)) {
		return nul_state;
	}

	/* return both, position of check digit and check digit */
	return (cc_state_t){.pos = --i, .issuer = j, .chk = calc_chk(buf, j)};
}


/* class implementation */
static nmck_bid_t
cc_bid(const char *str, size_t len)
{
	cc_state_t st;

	/* common cases first */
	if (len < 12U || len > 19U + 3U) {
		return nul_bid;
	}
	/* just calc checksum first and sort through issuers later */
	st = calc_st(str, len);

	switch (str[0U]) {
	case '1':
		/* uatp */
		if (st.issuer == 15U) {
			st.issuer = ISS_UATP;
			goto final;
		}
		break;

	case '2':
		/* master, diners */
		switch (str[1U]) {
		case '0':
		case '1':
			if (st.issuer == 15U) {
				st.issuer = ISS_DC_ENROUTE;
				goto final;
			}
			break;
		case '2' ... '7':
			if (st.issuer == 16U) {
				st.issuer = ISS_MASTER;
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
			if (st.issuer == 15U) {
				st.issuer = ISS_AMEX;
				goto final;
			}
			break;

		case '5':
			/* jcb */
			if (st.issuer == 16U &&
			    (str[2U] >= '2' && str[2U] <= '8')) {
				st.issuer = ISS_JCB;
				goto final;
			}
			break;

		case '0':
			if (st.issuer == 14U &&
			    (str[2U] >= '0' && str[2U] <= '5')) {
				st.issuer = ISS_DC_CB;
				goto final;
			}
			/*@fallthrough@*/
		case '6':
		case '8':
		case '9':
			if (st.issuer == 14U) {
				st.issuer = ISS_DC_INTL;
				goto final;
			}
			break;
		default:
			break;
		}
		break;

	case '4':
		/* visa */
		if (st.issuer == 16U &&
		    (!memcmp(str, "4026", 4U) ||
		     !memcmp(str, "4175", 4U) ||
		     !memcmp(str, "4405", 4U) ||
		     !memcmp(str, "4508", 4U) ||
		     !memcmp(str, "4844", 4U) ||
		     !memcmp(str, "4913", 4U) ||
		     !memcmp(str, "4917", 4U))) {
			st.issuer = ISS_VELEC;
			goto final;
		} else if (st.issuer == 13U || st.issuer == 16U) {
			st.issuer = ISS_VISA;
			goto final;
		}
		break;

	case '5':
		/* master, diners */
		switch (str[1U]) {
		case '0':

		case '1' ... '5':
			if (st.issuer == 16U) {
				st.issuer = ISS_MASTER;
				goto final;
			}
			break;
		case '6' ... '9':
		maestro:
			if (st.issuer >= 12U && st.issuer <= 19U) {
				st.issuer = ISS_MAESTRO;
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
			if (st.issuer >= 16U && st.issuer <= 19U) {
				st.issuer = ISS_CUP;
				goto final;
			}
			break;
		case '0':
		case '4':
		case '5':
			if (st.issuer == 16U) {
				st.issuer = ISS_DISC;
				goto final;
			}
			break;
		case '3':
			switch (str[2U]) {
			case '6':
				if (st.issuer >= 16U && st.issuer <= 19U) {
					st.issuer = ISS_INTPAY;
					goto final;
				}
				break;
			case '7' ... '9':
				if (st.issuer == 16U) {
					st.issuer = ISS_INSTAP;
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
	return nul_bid;

final:
	if (!st.s) {
		return nul_bid;
	} else if (st.pos != len - 1U) {
		return nul_bid;
	} else if (st.chk != str[st.pos]) {
		/* record state */
		return (nmck_bid_t){31U, st.s};
	}
	/* nul out the check digit because it passed */
	st.chk = 0;
	/* bid higher than gtin? */
	return (nmck_bid_t){63U, st.s};
}

static int
cc_prnt(const char *str, size_t UNUSED(len), nmck_bid_t b)
{
	cc_state_t st = {b.state};

	fputs(issuers[st.issuer], stdout);
	if (LIKELY(!st.chk)) {
		fputs(", conformant account number", stdout);
	} else {
		fputs(", non-conformant account number, should be ", stdout);
		fwrite(str, sizeof(*str), st.pos, stdout);
		fputc(st.chk, stdout);
	}
	return 0;
}

const struct nmck_chkr_s*
init_credcard(void)
{
	static const struct nmck_chkr_s this = {
		.name = "CREDCARD",
		.bidf = cc_bid,
		.prntf = cc_prnt,
	};
	return &this;
}

int
fini_credcard(void)
{
	return 0;
}

/* credcard.c ends here */

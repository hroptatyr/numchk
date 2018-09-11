#include <string.h>
#include <stdio.h>
#include "numchk.h"
#include "nifty.h"

#define NNMCK	(64U)

#define maybe(x, y)	\
	candpr[ncand] = nmpr_##x; \
	candck[ncand] = y; \
	ncand++
#define defo(x, y)	\
	surepr[nsure] = nmpr_##x; \
	sureck[nsure] = y; \
	nsure++

#define c(x)	\
	with (nmck_t y = nmck_##x(str, len)) { \
		if (y < 0) { \
			break; \
		} else if (y & 0b1U) { \
			maybe(x, y); \
		} else { \
			defo(x, y); \
		} \
	}
#define g(x)	\
	with (nmck_t y = nmck_##x(str, len)) { \
		if (y < 0) { \
			break; \
		} else { \
			maybe(x, y); \
		} \
	}

#ifdef __INTEL_COMPILER
# pragma warning (disable:2415)
#endif  /* __INTEL_COMPILER */

static size_t ncand;
static void(*candpr[NNMCK])(nmck_t, const char*, size_t);
static nmck_t candck[NNMCK];
static size_t nsure;
static void(*surepr[NNMCK])(nmck_t, const char*, size_t);
static nmck_t sureck[NNMCK];

%%{
	machine numchk;

	upnum = digit | upper;
	lonum = digit | lower;
	check = "_" | "?";
	vowel = "A" | "E" | "I" | "O" | "U";
	consonant = upper - vowel;

	include numchk "aadhaar.c";
	include numchk "abartn.c";
	include numchk "bic.c";
	include numchk "bicc.c";
	include numchk "cas.c";
	include numchk "cpf.c";
	include numchk "credcard.c";
	include numchk "cusip.c";
	include numchk "euvatid.c";
	include numchk "figi.c";
	include numchk "grid.c";
	include numchk "gtin.c";
	include numchk "iban.c";
	include numchk "credref.c";
	include numchk "idnr.c";
	include numchk "imei.c";
	include numchk "imo.c";
	include numchk "iposan.c";
	include numchk "isan.c";
	include numchk "isbn.c";
	include numchk "isin.c";
	include numchk "ismn.c";
	include numchk "isni.c";
	include numchk "issn.c";
	include numchk "istc.c";
	include numchk "kennitala.c";
	include numchk "lei.c";
	include numchk "nhi.c";
	include numchk "nhs.c";
	include numchk "npi.c";
	include numchk "oib.c";
	include numchk "ppsn.c";
	include numchk "sedol.c";
	include numchk "tckimlik.c";
	include numchk "tfn.c";
	include numchk "titulo-eleitoral.c";
	include numchk "upn.c";
	include numchk "vin.c";
	include numchk "wkn.c";

	main :=
		iban |
		figi |
		isin |
		istc |
		cusip |
		sedol |
		lei |
		gtin |
		isbn |
		issn |
		ismn |
		credcard |
		credref |
		tfn |
		cas |
		isni |
		isan |
		imo |
		vin |
		grid |
		euvatid |
		bicc |
		bic |
		wkn |
		imei |
		kennitala |
		tckimlik |
		aadhaar |
		titulo_eleitoral |
		cpf |
		iposan |
		npi |
		abartn |
		nhi |
		ppsn |
		idnr |
		oib |
		nhs |
		upn |

		## generic checks
		digit{2,} %{g(luhn)} %{g(verhoeff)} %{g(damm10)} |
		xdigit{2,} %{g(damm16)} |

		any*;

	write data;
}%%

static int
chck1(const char *str, size_t len)
{
	const char *p = str;
	const char *const pe = str + len;
	const char *const eof = pe;
	int cs;

	%% write init;
	ncand = nsure = 0U;

	%% write exec;
	return 0;
}

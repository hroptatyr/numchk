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

	include numchk "iban.c";
	include numchk "isin.c";
	include numchk "figi.c";
	include numchk "istc.c";
	include numchk "cusip.c";
	include numchk "sedol.c";
	include numchk "lei.c";
	include numchk "gtin.c";
	include numchk "isbn.c";
	include numchk "issn.c";
	include numchk "ismn.c";
	include numchk "credcard.c";
	include numchk "tfn.c";
	include numchk "cas.c";
	include numchk "isni.c";
	include numchk "isan.c";
	include numchk "imo.c";
	include numchk "vin.c";
	include numchk "grid.c";
	include numchk "euvatid.c";
	include numchk "bicc.c";
	include numchk "bic.c";

	main :=
		iban |
		isin |
		figi |
		istc |
		cusip |
		sedol |
		lei |
		gtin |
		isbn |
		issn |
		ismn |
		credcard |
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
		upnum{6} %{c(wkn)} |
		digit{2} "-"? digit{6} "-"? digit{6} "-"? ((digit | check) | digit{2}) %{c(imei)} |
		digit{6} "-"? digit{2} (digit | check) digit %{c(kennitala)} |
		digit{9} (digit{2} | check{2}) %{c(tckimlik)} |
		digit{4} " "? digit{4} " "? digit{3} (digit | check) %{c(aadhaar)} |
		digit{4} " "? digit{4} " "? digit{2} " "? (digit{2} | check{2}) %{c(titulo_eleitoral)} |
		digit{3} "."? digit{3} "."? digit{3} "-"? (digit{2} | check{2}) %{c(cpf)} |
		("10" | "11" | "20" | "30" | "40") digit{4} digit{5} (/[P-Y]/ | check) ("-" digit{2})? %{c(iposan)} |
		digit{9} (digit | check) %{c(npi)} |
		digit{8} (digit | check) %{c(abartn)} |
		upper{3} digit{3} (digit | check) %{c(nhi)} |
		digit{7} (upper | check) ("/"? /[A-Z ]/)? %{c(ppsn)} |
		digit{2} " "? digit{3} " "? digit{3} " "? digit{2} (digit | check) %{c(idnr)} |
		"HR"? digit{10} (digit | check) %{c(oib)} |
		digit{9} (digit | check) %{c(nhs)} |

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

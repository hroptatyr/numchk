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

	action isan {c(isan)}
	action ievatid {c(ievatid)}
	action plvatid {c(plvatid)}

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
		(digit | " "){12,22} %{c(credcard)} |
		digit{2,3} " "? digit{3} " "? digit{3} %{c(tfn)} |
		digit+ "-" digit{2} "-" (digit | check) %{c(cas)} |
		digit{4} " "? digit{4} " "? digit{4} " "? digit{3} (digit | "X" | check) %{c(isni)} |
		("ISAN" (" " | "-" | ":")?)? xdigit{4} "-"? xdigit{4} "-"? xdigit{4} "-"? xdigit{4} "-"? (alnum | check) %isan |
		("ISAN" (" " | "-" | ":")?)? xdigit{4} "-"? xdigit{4} "-"? xdigit{4} "-"? xdigit{4} "-"? (alnum | check) "-"? xdigit{4} "-"? xdigit{4} "-"? (alnum | check) %isan |
		("IMO" " "?)? digit{6} (digit | check) %{c(imo)} |
		upnum{8} (digit | "X" | check) upnum{8} %{c(vin)} |
		upnum{2} "-"? upnum{5} "-"? upnum{10} "-"? (upnum | check) %{c(grid)} |
		("DE" " "?)? digit{3} " "? digit{3} " "? digit{2} (digit | check) %{c(devatid)} |
		("BE" "."?)? digit{3,4} "."? digit{3} "."? digit{1} (digit{2} | check{2}) %{c(bevatid)} |
		"DK"? digit{7} (digit | check) %{c(dkvatid)} |
		"FI"? digit{7} (digit | check) %{c(fivatid)} |
		("FR" " "?)? (digit{2} | check{2}) /[ ,]/? digit{3} /[ ,]/? digit{3} /[ ,]/? digit{3} %{c(frvatid)} |
		(("GR" | "EL") " "?)? digit{8} (digit | check) %{c(grvatid)} |
		("IE" " "?)? digit{7} upper? (upper | check) "W"? %ievatid |
		("IE" " "?)? digit (upper | "*" | "+") digit{5} (upper | check) "W"? %ievatid |
		("IT" " "?)? digit{10} (digit | check) %{c(itvatid)} |
		("LU" " "?)? digit{3} " "? digit{3} " "? (digit{2} | check{2}) %{c(luvatid)} |
		"NL"? digit{8} (digit | check) "B" digit{2} %{c(nlvatid)} |
		("AT" " "?)? "U" digit{7} (digit | check) %{c(atvatid)} |
		("PL" " "?)? digit{3} "-"? digit{3} "-"? digit{2} "-"? digit (digit | check) %plvatid |
		("PL" " "?)? digit{3} "-"? digit{2} "-"? digit{2} "-"? digit{2} (digit | check) %plvatid |
		("PT" " "?)? digit{3} " "? digit{3} " "? digit{2} (digit | check) %{c(ptvatid)} |
		("SE" " "?)? digit{9} (digit | check) digit{2} %{c(sevatid)} |
		("SI" " "?)? digit{4} " "? digit{3} (digit | check) %{c(sivatid)} |
		("ES" " "?)? "A" digit{2} " "? digit{3} " "? digit{2} (digit | check) %{c(esvatid)} |
		upper{3} ("U" | "J" | "Z") digit{6} (digit | check) %{c(bicc)} |
		upper{6} upnum{2} upnum? (upnum{3})? %{c(bic)} |
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

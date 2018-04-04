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

	main :=
		upper{2} digit{2} (upnum | ' '){11,42} %{c(iban)} |
		upper{2} check{2} (upnum | ' '){11,42} %{c(iban)} |
		upper{2} upnum{9} (digit | check) %{c(isin)} |
		"BBG" (consonant | digit){8} (digit | check) %{c(figi)} |
		xdigit{3,}'-'digit{4}'-'xdigit{8}'-'(xdigit | check) %{c(istc)} |
		xdigit{3,} digit{4} xdigit{8} (xdigit | check) %{c(istc)} |
		(alnum | "*" | "@" | "#"){8} (digit | check) %{c(cusip)} |
		(consonant | digit){6} (digit | check) %{c(sedol)} |
		upnum{18} (digit{2} | check{2}) %{c(lei)} |
		digit{7,17} (digit | check) %{c(gtin)} |
		/97[89]/ "-"? digit "-"? digit{4} "-"? digit{4} "-"? (digit | check) %{c(isbn13)} |
		digit "-"? digit{4} "-"? digit{4} "-"? (digit | "X" | check) %{c(isbn10)} |
		digit{4} "-"? digit{3} (digit | "X" | check) %{c(issn8)} |
		"977" digit{4} digit{3} "00" (digit | check) %{c(issn13)} |
		"9790" digit{8} (digit | check) %{c(ismn13)} |
		"M" "-"? digit{4} "-"? digit{4} "-"? (digit | "X" | check) %{c(ismn10)} |
		(digit | " "){12,22} %{c(credcard)} |
		digit{2,3} " "? digit{3} " "? digit{3} %{c(tfn)} |
		digit+ "-" digit{2} "-" (digit | check) %{c(cas)} |
		digit{4} " "? digit{4} " "? digit{4} " "? digit{3} (digit | "X" | check) %{c(isni)} |
		("ISAN" (" " | "-" | ":")?)? xdigit{4} "-"? xdigit{4} "-"? xdigit{4} "-"? xdigit{4} "-"? (alnum | check) %{c(isan)} |
		("ISAN" (" " | "-" | ":")?)? xdigit{4} "-"? xdigit{4} "-"? xdigit{4} "-"? xdigit{4} "-"? (alnum | check) "-"? xdigit{4} "-"? xdigit{4} "-"? (alnum | check) %{c(isan)} |
		("IMO" " "?)? digit{6} (digit | check) %{c(imo)} |
		upnum{8} (digit | "X" | check) upnum{8} %{c(vin)} |
		upnum{2} "-"? upnum{5} "-"? upnum{10} "-"? (upnum | check) %{c(grid)} |
		("DE" " "?)? digit{3} " "? digit{3} " "? digit{2} (digit | check) %{c(devatid)} |
		("BE" "."?)? digit{3,4} "."? digit{3} "."? digit{1} (digit{2} | check{2}) %{c(bevatid)} |
		"DK"? digit{7} (digit | check) %{c(dkvatid)} |
		"FI"? digit{7} (digit | check) %{c(fivatid)} |
		("FR" " "?)? (digit{2} | check{2}) /[ ,]/? digit{3} /[ ,]/? digit{3} /[ ,]/? digit{3} %{c(frvatid)} |
		(("GR" | "EL") " "?)? digit{8} (digit | check) %{c(grvatid)} |
		("IE" " "?)? digit{7} upper? (upper | check) "W"? %{c(ievatid)} |
		("IE" " "?)? digit (upper | "*" | "+") digit{5} (upper | check) "W"? %{c(ievatid)} |
		("IT" " "?)? digit{10} (digit | check) %{c(itvatid)} |
		("LU" " "?)? digit{3} " "? digit{3} " "? (digit{2} | check{2}) %{c(luvatid)} |
		"NL"? digit{8} (digit | check) "B" digit{2} %{c(nlvatid)} |
		("AT" " "?)? "U" digit{7} (digit | check) %{c(atvatid)} |
		("PL" " "?)? digit{3} "-"? digit{3} "-"? digit{2} "-"? digit (digit | check) %{c(plvatid)} |
		("PL" " "?)? digit{3} "-"? digit{2} "-"? digit{2} "-"? digit{2} (digit | check) %{c(plvatid)} |
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

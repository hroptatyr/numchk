#include <string.h>
#include <stdio.h>
#include "numchk.h"
#include "nifty.h"

#define NNMCK	(64U)

#define maybe(x)	\
	candpr[ncand] = nmpr_##x; \
	candck[ncand] = 0; \
	ncand++

#define c(x)	\
	with (nmck_t y = nmck_##x(str, len)) { \
		if (y < 0) { \
			break; \
		} else if (y & 0b1U) { \
			candpr[ncand] = nmpr_##x; \
			candck[ncand] = y; \
			ncand++; \
		} else { \
			surepr[nsure] = nmpr_##x; \
			sureck[nsure] = y; \
			nsure++; \
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

	action isin {c(isin)}
	action figi {c(figi)}
	action iban {c(iban)}
	action cusip {c(cusip)}
	action sedol {c(sedol)}
	action lei {c(lei)}
	action gtin {c(gtin)}
	action isbn10 {c(isbn10)}
	action isbn13 {c(isbn13)}
	action credcard {c(credcard)}
	action tfn {c(tfn)}
	action cas {c(cas)}
	action istc {c(istc)}
	action issn8 {c(issn8)}
	action issn13 {c(issn13)}
	action ismn10 {c(ismn10)}
	action ismn13 {c(ismn13)}
	action isni {c(isni)}
	action isan {c(isan)}
	action imo {c(imo)}
	action vin {c(vin)}
	action grid {c(grid)}
	action devatid {c(devatid)}
	action bevatid {c(bevatid)}
	action dkvatid {c(dkvatid)}
	action fivatid {c(fivatid)}
	action frvatid {c(frvatid)}
	action grvatid {c(grvatid)}
	action ievatid {c(ievatid)}
	action bicc {c(bicc)}
	action bic {c(bic)}
	action wkn {c(wkn)}
	action imei {c(imei)}
	action kennitala {c(kennitala)}
	action tckimlik {c(tckimlik)}
	action aadhaar {c(aadhaar)}
	action titulo_eleitoral {c(titulo_eleitoral)}
	action cpf {c(cpf)}
	action iposan {c(iposan)}
	action npi {c(npi)}
	action abartn {c(abartn)}
	action nhi {c(nhi)}

	main :=
		upper{2} digit{2} (upnum | ' '){11,42} %iban |
		upper{2} check{2} (upnum | ' '){11,42} %iban |
		upper{2} upnum{9} (digit | check) %isin |
		"BBG" upnum{8} (digit | check) %figi |
		xdigit{3,}'-'digit{4}'-'xdigit{8}'-'(xdigit | check) %istc |
		xdigit{3,} digit{4} xdigit{8} (xdigit | check) %istc |
		digit{7,10} (digit | check) %cusip |
		digit{6} (digit | check) %sedol |
		upnum{18} (digit{2} | check{2}) %lei |
		digit{7,13} (digit | check) %gtin |
		/97[89]/ "-"? digit "-"? digit{4} "-"? digit{4} "-"? (digit | check) %isbn13 |
		digit "-"? digit{4} "-"? digit{4} "-"? (digit | "X" | check) %isbn10 |
		digit{4} "-"? digit{3} (digit | "X" | check) %issn8 |
		"977" digit{4} digit{3} "00" (digit | check) %issn13 |
		"9790" digit{8} (digit | check) %ismn13 |
		"M" "-"? digit{4} "-"? digit{4} "-"? (digit | "X" | check) %ismn10 |
		(digit | " "){12,22} %credcard |
		digit{2,3} " "? digit{3} " "? digit{3} %tfn |
		digit+ "-" digit{2} "-" (digit | check) %cas |
		digit{4} " "? digit{4} " "? digit{4} " "? digit{3} (digit | "X" | check) %isni |
		("ISAN" (" " | "-" | ":")?)? xdigit{4} "-"? xdigit{4} "-"? xdigit{4} "-"? xdigit{4} "-"? (alnum | check) %isan |
		("ISAN" (" " | "-" | ":")?)? xdigit{4} "-"? xdigit{4} "-"? xdigit{4} "-"? xdigit{4} "-"? (alnum | check) "-"? xdigit{4} "-"? xdigit{4} "-"? (alnum | check) %isan |
		("IMO" " "?)? digit{6} (digit | check) %imo |
		upnum{8} (digit | "X" | check) upnum{8} %vin |
		upnum{2} "-"? upnum{5} "-"? upnum{10} "-"? (upnum | check) %grid |
		("DE" " "?)? digit{3} " "? digit{3} " "? digit{2} (digit | check) %devatid |
		("BE" "."?)? digit{3,4} "."? digit{3} "."? digit{1} (digit{2} | check{2}) %bevatid |
		"DK"? digit{7} (digit | check) %dkvatid |
		"FI"? digit{7} (digit | check) %fivatid |
		("FR" " "?)? (digit{2} | check{2}) /[ ,]/? digit{3} /[ ,]/? digit{3} /[ ,]/? digit{3} %frvatid |
		(("GR" | "EL") " "?)? digit{8} (digit | check) %grvatid |
		("IE" " "?)? digit{7} upper? (upper | check) "W"? %ievatid |
		("IE" " "?)? digit (upper | "*" | "+") digit{5} (upper | check) "W"? %ievatid |
		upper{3} ("U" | "J" | "Z") digit{6} (digit | check) %bicc |
		upper{6} upnum{2} upnum? (upnum{3})? %bic |
		upnum{6} %wkn |
		digit{2} "-"? digit{6} "-"? digit{6} "-"? ((digit | check) | digit{2}) %imei |
		digit{6} "-"? digit{2} (digit | check) digit %kennitala |
		digit{9} (digit{2} | check{2}) %tckimlik |
		digit{4} " "? digit{4} " "? digit{3} (digit | check) %aadhaar |
		digit{4} " "? digit{4} " "? digit{2} " "? (digit{2} | check{2}) %titulo_eleitoral |
		digit{3} "."? digit{3} "."? digit{3} "-"? (digit{2} | check{2}) %cpf |
		("10" | "11" | "20" | "30" | "40") digit{4} digit{5} (/[P-Y]/ | check) ("-" digit{2})? %iposan |
		digit{9} (digit | check) %npi |
		digit{8} (digit | check) %abartn |
		upper{3} digit{3} (digit | check) %nhi |
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

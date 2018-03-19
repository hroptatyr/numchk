#include <string.h>
#include <stdio.h>
#include "numchk.h"
#include "nifty.h"

#define NNMCK	(64U)
#define is(x)	\
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

	action isin {
		is(isin);
	}
	action figi {
		is(figi);
	}
	action iban {
		is(iban);
	}
	action cusip {
		is(cusip);
	}
	action sedol {
		is(sedol);
	}
	action lei {
		is(lei);
	}
	action gtin {
		is(gtin);
	}
	action isbn10 {
		is(isbn10);
	}
	action isbn13 {
		is(isbn13);
	}
	action credcard {
		is(credcard);
	}
	action tfn {
		is(tfn);
	}
	action cas {
		is(cas);
	}
	action istc {
		is(istc);
	}
	action issn8 {
		is(issn8);
	}
	action issn13 {
		is(issn13);
	}
	action ismn10 {
		is(ismn10);
	}
	action ismn13 {
		is(ismn13);
	}
	action isni {
		is(isni);
	}
	action isan {
		is(isan);
	}

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

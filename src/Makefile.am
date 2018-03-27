### Makefile.am
include $(top_builddir)/version.mk

AM_CFLAGS = $(EXTRA_CFLAGS)
AM_CPPFLAGS = -D_POSIX_C_SOURCE=201001L -D_XOPEN_SOURCE=700 -D_BSD_SOURCE
AM_LDFLAGS = $(XCCLDFLAGS)

bin_PROGRAMS =
noinst_PROGRAMS =
lib_LTLIBRARIES =
noinst_LIBRARIES =
noinst_LTLIBRARIES =
pkglib_LTLIBRARIES =
noinst_HEADERS =
BUILT_SOURCES =
EXTRA_DIST = $(BUILT_SOURCES)
CLEANFILES = 
SUFFIXES =

CLEANFILES += version.c
EXTRA_DIST += version.c.in

noinst_LIBRARIES += libnumchk.a
libnumchk_a_SOURCES = version.c version.h
EXTRA_libnumchk_a_SOURCES =
libnumchk_a_SOURCES += isin.c
BUILT_SOURCES += isin-cc.c
libnumchk_a_SOURCES += figi.c
libnumchk_a_SOURCES += cusip.c
libnumchk_a_SOURCES += sedol.c
libnumchk_a_SOURCES += iban.c
BUILT_SOURCES += iban-cc.c
libnumchk_a_SOURCES += lei.c
libnumchk_a_SOURCES += gtin.c
libnumchk_a_SOURCES += isbn.c
libnumchk_a_SOURCES += issn.c
libnumchk_a_SOURCES += ismn.c
libnumchk_a_SOURCES += credcard.c
libnumchk_a_SOURCES += tfn.c
libnumchk_a_SOURCES += cas.c
libnumchk_a_SOURCES += istc.c
libnumchk_a_SOURCES += isni.c
libnumchk_a_SOURCES += isan.c
libnumchk_a_SOURCES += imo.c
libnumchk_a_SOURCES += vin.c
libnumchk_a_SOURCES += grid.c
libnumchk_a_SOURCES += euvatid.c
libnumchk_a_SOURCES += bicc.c
libnumchk_a_SOURCES += bic.c
BUILT_SOURCES += bic-cc.c
libnumchk_a_SOURCES += wkn.c
libnumchk_a_SOURCES += imei.c
libnumchk_a_SOURCES += kennitala.c
libnumchk_a_SOURCES += tckimlik.c
libnumchk_a_SOURCES += aadhaar.c
libnumchk_a_SOURCES += titulo-eleitoral.c
libnumchk_a_SOURCES += cpf.c
libnumchk_a_SOURCES += iposan.c
libnumchk_a_SOURCES += npi.c
libnumchk_a_SOURCES += abartn.c
libnumchk_a_SOURCES += nhi.c
libnumchk_a_SOURCES += ppsn.c
libnumchk_a_SOURCES += idnr.c

bin_PROGRAMS += numchk
numchk_SOURCES = numchk.c numchk.h numchk.yuck numchk.rl
numchk_SOURCES += nifty.h
numchk_LDADD = libnumchk.a
BUILT_SOURCES += numchk.yucc
BUILT_SOURCES += numchk.rlc

## ccl->c
SUFFIXES += .ccl
.ccl.c:
	$(AM_V_GEN) $(top_builddir)/build-aux/cccc < $< > $@ || $(RM) -- $@

## version rules
version.c: version.c.in $(top_builddir)/.version
	$(AM_V_GEN) PATH="$(top_builddir)/build-aux:$${PATH}" \
		yuck$(EXEEXT) scmver --ignore-noscm --force -o $@ \
			--use-reference --reference $(top_builddir)/.version $<

## yuck rule
SUFFIXES += .yuck
SUFFIXES += .yucc
SUFFIXES += .re
.yuck.yucc:
	$(AM_V_GEN) PATH=$(top_builddir)/build-aux:"$${PATH}" \
		yuck$(EXEEXT) gen -o $@ $<

.rl.rlc:
	$(AM_V_GEN) PATH=$(top_builddir)/build-aux:"$${PATH}" \
		ragel -F1 -C -o $@ $<

## Create preprocessor output (debugging purposes only)
.c.i:
	$(COMPILE) -E -o $@ $<

## Create assembler output (debugging purposes only)
.c.s:
	$(COMPILE) -S -c $(AM_CFLAGS) $<

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
libnumchk_a_SOURCES += isin.c isin.h
libnumchk_a_SOURCES += figi.c figi.h
libnumchk_a_SOURCES += cusip.c cusip.h
libnumchk_a_SOURCES += sedol.c sedol.h

bin_PROGRAMS += numchk
numchk_SOURCES = numchk.c numchk.h numchk.yuck
numchk_SOURCES += nifty.h
numchk_LDADD = libnumchk.a
BUILT_SOURCES += numchk.yucc


## version rules
version.c: version.c.in $(top_builddir)/.version
	$(AM_V_GEN) PATH="$(top_builddir)/build-aux:$${PATH}" \
		yuck$(EXEEXT) scmver --ignore-noscm --force -o $@ \
			--use-reference --reference $(top_builddir)/.version $<

## yuck rule
SUFFIXES += .yuck
SUFFIXES += .yucc
.yuck.yucc:
	$(AM_V_GEN) PATH=$(top_builddir)/build-aux:"$${PATH}" \
		yuck$(EXEEXT) gen -o $@ $<

## Create preprocessor output (debugging purposes only)
.c.i:
	$(COMPILE) -E -o $@ $<

## Create assembler output (debugging purposes only)
.c.s:
	$(COMPILE) -S -c $(AM_CFLAGS) $<

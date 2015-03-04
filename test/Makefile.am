# Help the Developers and yourself. Just use the C locale and settings
# for the compilation. They can still be overriden by make LANG=<whatever>
# but that is general a not very good idea
include $(top_builddir)/version.mk

LANG = C
LC_ALL = C

AM_CFLAGS = $(EXTRA_CFLAGS)
AM_CPPFLAGS = -D_POSIX_C_SOURCE=200112L -D_XOPEN_SOURCE=600 -D_BSD_SOURCE
AM_CPPFLAGS += -DTEST

EXTRA_DIST = $(BUILT_SOURCES) $(TESTS)
TESTS =
TEST_EXTENSIONS =
BUILT_SOURCES =
dt_tests =
bin_tests =
batch_tests =
check_PROGRAMS =
CLEANFILES = $(check_PROGRAMS)

## our little helper
include clitoris.am
AM_CLIT_LOG_FLAGS = --builddir $(top_builddir)/src --verbose --keep-going

TESTS += credcard_01.clit
TESTS += credcard_02.clit
TESTS += credcard_03.clit
TESTS += credcard_04.clit
EXTRA_DIST += paypalobjects.xmpl

TESTS += iban_01.clit
TESTS += iban_02.clit
TESTS += iban_03.clit
EXTRA_DIST += nordea.xmpl

TESTS += isin_01.clit
TESTS += isin_02.clit
TESTS += isin_03.clit
EXTRA_DIST += isin.xmpl

TESTS += figi_01.clit
TESTS += figi_02.clit
EXTRA_DIST += bsym.xmpl

## Makefile.am ends here

include $(top_builddir)/version.mk

LANG = C
LC_ALL = C

ACLOCAL_AMFLAGS = -I m4

SUBDIRS =
EXTRA_DIST = $(DISTCLEANFILES)
DISTCLEANFILES =

SUBDIRS += build-aux
SUBDIRS += src
SUBDIRS += test

EXTRA_DIST += README.md

DISTCLEANFILES += version.mk
DISTCLEANFILES += .version
EXTRA_DIST += version.mk.in


## make sure .version is read-only in the dist
dist-hook:
	chmod ugo-w $(distdir)/.version

## Makefile.am ends here

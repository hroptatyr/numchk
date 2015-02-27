/*** numchk.c -- main snippet
 *
 * Copyright (C) 2014-2015 Sebastian Freundt
 *
 * Author:  Sebastian Freundt <freundt@ga-group.nl>
 *
 * This file is part of numchk.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the author nor the names of any contributors
 *    may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ***/
#if defined HAVE_CONFIG_H
# include "config.h"
#endif	/* HAVE_CONFIG_H */
#if defined HAVE_VERSION_H
# include "version.h"
#endif	/* HAVE_VERSION_H */
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "numchk.h"
#include "nifty.h"

static const struct nmck_chkr_s *chkrs[16U];
static size_t nchkrs;


static __attribute__((format(printf, 1, 2))) void
error(const char *fmt, ...)
{
	va_list vap;
	va_start(vap, fmt);
	vfprintf(stderr, fmt, vap);
	va_end(vap);
	if (errno) {
		fputs(": ", stderr);
		fputs(strerror(errno), stderr);
	}
	fputc('\n', stderr);
	return;
}


static int
proc1(const char *str, size_t len)
{
	nmck_bid_t best = {0U};
	const struct nmck_chkr_s *chkr = NULL;

	/* start the bidding */
	for (size_t i = 0U; i < nchkrs; i++) {
		nmck_bid_t x = chkrs[i]->bidf(str, len);

		if (x.bid > best.bid) {
			best = x;
			chkr = chkrs[i];
		}
	}
	fwrite(str, sizeof(*str), len, stdout);
	if (LIKELY(chkr != NULL)) {
		fputc('\t', stdout);
		if (LIKELY(chkr->prntf != NULL)) {
			chkr->prntf(str, len, best);
		} else if (LIKELY(chkr->name != NULL)) {
			fputs(chkr->name, stdout);
		} else {
			fprintf(stdout, "%p", chkr);
		}
		fputc('\n', stdout);
	} else {
		fputs("\tunknown\n", stdout);
	}
	return 0;
}


#if defined __INTEL_COMPILER
# pragma warning (disable:1419)
#endif	/* __INTEL_COMPILER */

static int
init_nmck(void)
{
	extern const struct nmck_chkr_s *init_isin(void);
	extern const struct nmck_chkr_s *init_figi(void);
	extern const struct nmck_chkr_s *init_cusip(void);
	extern const struct nmck_chkr_s *init_sedol(void);

	chkrs[nchkrs++] = init_isin();
	chkrs[nchkrs++] = init_figi();
	chkrs[nchkrs++] = init_cusip();
	chkrs[nchkrs++] = init_sedol();
	return 0;
}

static int
fini_nmck(void)
{
	extern int fini_isin(void);
	extern int fini_figi(void);
	extern int fini_cusip(void);

	fini_isin();
	fini_figi();
	fini_cusip();
	return 0;
}

#if defined __INTEL_COMPILER
# pragma warning (default:1419)
#endif	/* __INTEL_COMPILER */


#include "numchk.yucc"

int
main(int argc, char *argv[])
{
	yuck_t argi[1U];
	int rc = 0;

	if (yuck_parse(argi, argc, argv) < 0) {
		rc = 1;
		goto out;
	}

	/* right then, let's get going */
	init_nmck();

	if (!argi->nargs) {
		char *line = NULL;
		size_t llen = 0U;

#if defined HAVE_GETLINE
		for (ssize_t nrd; (nrd = getline(&line, &llen, stdin)) > 0;) {
			if (LIKELY(line[nrd - 1] == '\n')) {
				line[--nrd] = '\0';
				if (UNLIKELY(line[nrd - 1] == '\r')) {
					line[--nrd] = '\0';
				}
			}
			proc1(line, nrd);
		}
		free(line);
#elif defined HAVE_FGETLN
		while ((line = fgetln(stdin, &llen)) != NULL) {
			if (LIKELY(line[llen - 1] == '\n')) {
				line[--llen] = '\0';
			}
			proc1(line, llen);
		}
#else
		errno = 0, error("\
error: cannot read from stdin\n\
getline() or fgetln() support missing.");
#endif	/* GETLINE/FGETLN */
		if (ferror(stdin)) {
			error("\
error: reading from stdin disrupted");
		}
	} else {
		for (size_t i = 0U; i < argi->nargs; i++) {
			const char *str = argi->args[i];
			size_t len = strlen(str);

			proc1(str, len);
		}
	}

out:
	fini_nmck();
	yuck_free(argi);
	return rc;
}

/* numchk.c ends here */

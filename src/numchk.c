/*** numchk.c -- main snippet
 *
 * Copyright (C) 2014-2018 Sebastian Freundt
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

#include "numchk.rlc"


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

	if (!argi->nargs) {
		char *line = NULL;
		size_t llen = 0U;

#if defined HAVE_GETLINE
		for (ssize_t nrd; (nrd = getline(&line, &llen, stdin)) > 0;) {
			nrd -= nrd > 0 && line[nrd - 1] == '\n';
			nrd -= nrd > 0 && line[nrd - 1] == '\r';
			line[nrd] = '\0';
			proc1(line, nrd);
		}
		free(line);
#elif defined HAVE_FGETLN
		while ((line = fgetln(stdin, &llen)) != NULL) {
			llen -= llen && line[llen - 1] == '\n';
			llen -= llen && line[llen - 1] == '\r';
			line[llen] = '\0';
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

			proc1(str, strlen(str));
		}
	}

out:
	yuck_free(argi);
	return rc;
}

/* numchk.c ends here */

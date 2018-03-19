/*** numchk.h -- numchk public api
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
#if !defined INCLUDED_numchk_h_
#define INCLUDED_numchk_h_

#include <stdint.h>

/**
 * Return type for checkers, LSB 0 for perfect conformance,
 * <0 for error, and LSB 1 for non-conformant strings.
 * For conformant strings the remaining bits can be used
 * to capture state. */
typedef intptr_t nmck_t;

extern nmck_t nmck_isin(const char*, size_t);
extern void nmpr_isin(nmck_t, const char*, size_t);

extern nmck_t nmck_figi(const char*, size_t);
extern void nmpr_figi(nmck_t, const char*, size_t);

extern nmck_t nmck_cusip(const char*, size_t);
extern void nmpr_cusip(nmck_t, const char*, size_t);

extern nmck_t nmck_sedol(const char*, size_t);
extern void nmpr_sedol(nmck_t, const char*, size_t);

extern nmck_t nmck_iban(const char*, size_t);
extern void nmpr_iban(nmck_t, const char*, size_t);

extern nmck_t nmck_lei(const char*, size_t);
extern void nmpr_lei(nmck_t, const char*, size_t);

extern nmck_t nmck_gtin(const char*, size_t);
extern void nmpr_gtin(nmck_t, const char*, size_t);

extern nmck_t nmck_isbn(const char*, size_t);
extern void nmpr_isbn(nmck_t, const char*, size_t);
extern nmck_t nmck_isbn10(const char*, size_t);
extern void nmpr_isbn10(nmck_t, const char*, size_t);
extern nmck_t nmck_isbn13(const char*, size_t);
extern void nmpr_isbn13(nmck_t, const char*, size_t);

extern nmck_t nmck_credcard(const char*, size_t);
extern void nmpr_credcard(nmck_t, const char*, size_t);

extern nmck_t nmck_tfn(const char*, size_t);
extern void nmpr_tfn(nmck_t, const char*, size_t);

extern nmck_t nmck_cas(const char*, size_t);
extern void nmpr_cas(nmck_t, const char*, size_t);

extern nmck_t nmck_istc(const char*, size_t);
extern void nmpr_istc(nmck_t, const char*, size_t);


/* utils */
static inline int ischeck(int c)
{
	return ((c + 0x10) | 0x20) - 0x10 == '_';
}

#endif	/* INCLUDED_numchk_h_ */

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
 * Return type for checkers, <0 for error,
 * LSB 0 for perfect conformance and LSB 1 for non-conformant strings.
 * For conformant strings the remaining bits can be used to capture state. */
typedef intptr_t nmck_t;

/**
 * Generic Luhn check on all-digit input.
 * http://en.wikipedia.org/wiki/Luhn_algorithm */
extern nmck_t nmck_luhn(const char*, size_t);
extern void nmpr_luhn(nmck_t, const char*, size_t);

/**
 * Generic Verhoeff check on all-digit input.
 * http://en.wikipedia.org/wiki/Verhoeff_algorithm */
extern nmck_t nmck_verhoeff(const char*, size_t);
extern void nmpr_verhoeff(nmck_t, const char*, size_t);

/**
 * Generic Damm check on all-digit input.
 * http://en.wikipedia.org/wiki/Damm_algorithm
 * From http://www.md-software.de/math/DAMM_Quasigruppen.txt we're using
 * Ordnung 10: Computer-Suche, erkennt alle phonetischen Fehler */
extern nmck_t nmck_damm10(const char*, size_t);
extern void nmpr_damm10(nmck_t, const char*, size_t);
/**
 * Generic Damm check on all-xdigit input.
 * http://en.wikipedia.org/wiki/Damm_algorithm
 * From http://www.md-software.de/math/DAMM_Quasigruppen.txt we're using
 * Ordnung 16: x*y:=2x+y mit Rechnung in GF(2^4) */
extern nmck_t nmck_damm16(const char*, size_t);
extern void nmpr_damm16(nmck_t, const char*, size_t);

/**
 * Check for ISO 6166 conformant identifiers, alphanumeric input.
 * http://en.wikipedia.org/wiki/International_Securities_Identification_Number
 * http://www.iso.org/standard/44811.html */
extern nmck_t nmck_isin(const char*, size_t);
extern void nmpr_isin(nmck_t, const char*, size_t);

/**
 * Check for OMG conformant FIGIs, alphanumeric input
 * http://en.wikipedia.org/wiki/Financial_Instrument_Global_Identifier
 * http://openfigi.com/ */
extern nmck_t nmck_figi(const char*, size_t);
extern void nmpr_figi(nmck_t, const char*, size_t);

/**
 * Check for ABA's and S&P's CUSIP identifier, alphanumeric input.
 * http://en.wikipedia.org/wiki/CUSIP */
extern nmck_t nmck_cusip(const char*, size_t);
extern void nmpr_cusip(nmck_t, const char*, size_t);

/**
 * Check for LSE's SEDOL identifier, alphanumeric input.
 * http://en.wikipedia.org/wiki/SEDOL
 * http://www.londonstockexchange.com/products-and-services/reference-data/sedol-master-file/documentation/documentation.htm */
extern nmck_t nmck_sedol(const char*, size_t);
extern void nmpr_sedol(nmck_t, const char*, size_t);

/**
 * Check for ISO 13616 conformant bank account number, alphanumeric input.
 * http://en.wikipedia.org/wiki/International_Bank_Account_Number
 * http://www.iso.org/standard/41031.html */
extern nmck_t nmck_iban(const char*, size_t);
extern void nmpr_iban(nmck_t, const char*, size_t);

/**
 * Check for ISO 17442 conformant legal entity identifiers, alphanumeric input.
 * http://en.wikipedia.org/wiki/Legal_Entity_Identifier
 * http://www.iso.org/standard/59771.html */
extern nmck_t nmck_lei(const char*, size_t);
extern void nmpr_lei(nmck_t, const char*, size_t);

/**
 * Check for GS1's GTINs, GSINs and SSCCs, numeric input.
 * http://en.wikipedia.org/wiki/Global_Trade_Item_Number
 * http://www.gs1.org/services/how-calculate-check-digit-manually */
extern nmck_t nmck_gtin(const char*, size_t);
extern void nmpr_gtin(nmck_t, const char*, size_t);

/**
 * Check for ISO 2108 conformant book numbers, numeric input.
 * http://en.wikipedia.org/wiki/International_Standard_Book_Number
 * http://www.iso.org/standard/36563.html */
extern nmck_t nmck_isbn(const char*, size_t);
extern void nmpr_isbn(nmck_t, const char*, size_t);
extern nmck_t nmck_isbn10(const char*, size_t);
extern void nmpr_isbn10(nmck_t, const char*, size_t);
extern nmck_t nmck_isbn13(const char*, size_t);
extern void nmpr_isbn13(nmck_t, const char*, size_t);

extern nmck_t nmck_credcard(const char*, size_t);
extern void nmpr_credcard(nmck_t, const char*, size_t);

/**
 * Check for the ATO's tax file number, numeric input.
 * http://en.wikipedia.org/wiki/Tax_file_number */
extern nmck_t nmck_tfn(const char*, size_t);
extern void nmpr_tfn(nmck_t, const char*, size_t);

/**
 * Check for CAS's registry numbers, numeric input.
 * http://en.wikipedia.org/wiki/CAS_Registry_Number
 * http://support.cas.org/content/counter */
extern nmck_t nmck_cas(const char*, size_t);
extern void nmpr_cas(nmck_t, const char*, size_t);

/**
 * Check for ISO 21047 conformant text codes, hex-numeric input.
 * http://en.wikipedia.org/wiki/International_Standard_Text_Code */
extern nmck_t nmck_istc(const char*, size_t);
extern void nmpr_istc(nmck_t, const char*, size_t);

/**
 * Check for ISO 3297 conformant serial numbers, numeric input.
 * http://en.wikipedia.org/wiki/International_Standard_Serial_Number */
extern nmck_t nmck_issn(const char*, size_t);
extern void nmpr_issn(nmck_t, const char*, size_t);
extern nmck_t nmck_issn8(const char*, size_t);
extern void nmpr_issn8(nmck_t, const char*, size_t);
extern nmck_t nmck_issn13(const char*, size_t);
extern void nmpr_issn13(nmck_t, const char*, size_t);

/**
 * Check for ISO 10957 conformant music number, numeric input.
 * http://en.wikipedia.org/wiki/International_Standard_Music_Number */
extern nmck_t nmck_ismn(const char*, size_t);
extern void nmpr_ismn(nmck_t, const char*, size_t);
extern nmck_t nmck_ismn10(const char*, size_t);
extern void nmpr_ismn10(nmck_t, const char*, size_t);
extern nmck_t nmck_ismn13(const char*, size_t);
extern void nmpr_ismn13(nmck_t, const char*, size_t);

/**
 * Check for ISO 27729 conformant name identifier, numeric input.
 * http://en.wikipedia.org/wiki/International_Standard_Name_Identifier */
extern nmck_t nmck_isni(const char*, size_t);
extern void nmpr_isni(nmck_t, const char*, size_t);

/**
 * ISO 15706 conformant audiovisual number, hex-numeric input.
 * http://en.wikipedia.org/wiki/International_Standard_Audiovisual_Number */
extern nmck_t nmck_isan(const char*, size_t);
extern void nmpr_isan(nmck_t, const char*, size_t);

/**
 * Check for IMO's vessel number, numeric input.
 * http://en.wikipedia.org/wiki/IMO_number */
extern nmck_t nmck_imo(const char*, size_t);
extern void nmpr_imo(nmck_t, const char*, size_t);

/**
 * Check for vehicle identification numbers, alphanumeric input.
 * http://en.wikipedia.org/wiki/Vehicle_identification_number */
extern nmck_t nmck_vin(const char*, size_t);
extern void nmpr_vin(nmck_t, const char*, size_t);

/**
 * Check for the global release identifier, alphanumeric input.
 * http://en.wikipedia.org/wiki/Global_Release_Identifier */
extern nmck_t nmck_grid(const char*, size_t);
extern void nmpr_grid(nmck_t, const char*, size_t);

/**
 * Check for BIC's container number, alphanumeric input.
 * http://www.bic-code.org/ */
extern nmck_t nmck_bicc(const char*, size_t);
extern void nmpr_bicc(nmck_t, const char*, size_t);

/**
 * Check for VAT identity numbers of EU member states, alphanumeric input.
 * http://en.wikipedia.org/wiki/VAT_identification_number#European_Union_VAT_identification_numbers */
extern nmck_t nmck_euvatid(const char*, size_t);
extern void nmpr_euvatid(nmck_t, const char*, size_t);
extern nmck_t nmck_devatid(const char*, size_t);
extern void nmpr_devatid(nmck_t, const char*, size_t);
extern nmck_t nmck_bevatid(const char*, size_t);
extern void nmpr_bevatid(nmck_t, const char*, size_t);
extern nmck_t nmck_dkvatid(const char*, size_t);
extern void nmpr_dkvatid(nmck_t, const char*, size_t);
extern nmck_t nmck_fivatid(const char*, size_t);
extern void nmpr_fivatid(nmck_t, const char*, size_t);
extern nmck_t nmck_frvatid(const char*, size_t);
extern void nmpr_frvatid(nmck_t, const char*, size_t);
extern nmck_t nmck_grvatid(const char*, size_t);
extern void nmpr_grvatid(nmck_t, const char*, size_t);
extern nmck_t nmck_ievatid(const char*, size_t);
extern void nmpr_ievatid(nmck_t, const char*, size_t);
extern nmck_t nmck_itvatid(const char*, size_t);
extern void nmpr_itvatid(nmck_t, const char*, size_t);
extern nmck_t nmck_luvatid(const char*, size_t);
extern void nmpr_luvatid(nmck_t, const char*, size_t);
extern nmck_t nmck_nlvatid(const char*, size_t);
extern void nmpr_nlvatid(nmck_t, const char*, size_t);
extern nmck_t nmck_atvatid(const char*, size_t);
extern void nmpr_atvatid(nmck_t, const char*, size_t);
extern nmck_t nmck_plvatid(const char*, size_t);
extern void nmpr_plvatid(nmck_t, const char*, size_t);
extern nmck_t nmck_ptvatid(const char*, size_t);
extern void nmpr_ptvatid(nmck_t, const char*, size_t);
extern nmck_t nmck_sevatid(const char*, size_t);
extern void nmpr_sevatid(nmck_t, const char*, size_t);
extern nmck_t nmck_sivatid(const char*, size_t);
extern void nmpr_sivatid(nmck_t, const char*, size_t);
extern nmck_t nmck_esvatid(const char*, size_t);
extern void nmpr_esvatid(nmck_t, const char*, size_t);

/**
 * Check for SWIFT's business identifiers, alphanumeric input.
 * http://en.wikipedia.org/wiki/ISO_9362 */
extern nmck_t nmck_bic(const char*, size_t);
extern void nmpr_bic(nmck_t, const char*, size_t);

/**
 * Check for WM Daten's Wertpapierkennummer, alphanumeric input.
 * http://en.wikipedia.org/wiki/Wertpapierkennnummer */
extern nmck_t nmck_wkn(const char*, size_t);
extern void nmpr_wkn(nmck_t, const char*, size_t);

/**
 * Check for mobile equipment identity numbers, numeric input.
 * http://en.wikipedia.org/wiki/International_Mobile_Equipment_Identity */
extern nmck_t nmck_imei(const char*, size_t);
extern void nmpr_imei(nmck_t, const char*, size_t);

/**
 * Check for Iceland's personal identification number, numeric input.
 * http://en.wikipedia.org/wiki/Kennitala */
extern nmck_t nmck_kennitala(const char*, size_t);
extern void nmpr_kennitala(nmck_t, const char*, size_t);

/**
 * Check for Turkey's personal identification number, numeric input.
 * http://en.wikipedia.org/wiki/Turkish_Identification_Number */
extern nmck_t nmck_tckimlik(const char*, size_t);
extern void nmpr_tckimlik(nmck_t, const char*, size_t);

/**
 * Check for India's personal identification number, numeric input.
 * http://en.wikipedia.org/wiki/Aadhaar */
extern nmck_t nmck_aadhaar(const char*, size_t);
extern void nmpr_aadhaar(nmck_t, const char*, size_t);

/**
 * Check for Brazil's personal identification number, numeric input.
 * http://pt.wikipedia.org/wiki/T%C3%ADtulo_eleitoral */
extern nmck_t nmck_titulo_eleitoral(const char*, size_t);
extern void nmpr_titulo_eleitoral(nmck_t, const char*, size_t);

/**
 * Check for Brazil's taxpayer identification number, numeric input.
 * http://en.wikipedia.org/wiki/Cadastro_de_Pessoas_F%C3%ADsicas */
extern nmck_t nmck_cpf(const char*, size_t);
extern void nmpr_cpf(nmck_t, const char*, size_t);

/**
 * Check for IPOS's application numbers, alphanumeric input.
 * http://blog.cantab-ip.com/blog/2014/01/20/new-format-for-singapore-ip-application-numbers-at-ipos/ */
extern nmck_t nmck_iposan(const char*, size_t);
extern void nmpr_iposan(nmck_t, const char*, size_t);

/**
 * Check for CMS's national provider identifier, numeric input.
 * http://en.wikipedia.org/wiki/National_Provider_Identifier */
extern nmck_t nmck_npi(const char*, size_t);
extern void nmpr_npi(nmck_t, const char*, size_t);

/**
 * Check for ABA's routing transit number, numeric input.
 * http://en.wikipedia.org/wiki/ABA_routing_transit_number */
extern nmck_t nmck_abartn(const char*, size_t);
extern void nmpr_abartn(nmck_t, const char*, size_t);

/**
 * Check for New Zealand's national health index, numeric input.
 * http://en.wikipedia.org/wiki/NHI_Number */
extern nmck_t nmck_nhi(const char*, size_t);
extern void nmpr_nhi(nmck_t, const char*, size_t);

/**
 * Check for Ireland's personal public service number, numeric input.
 * http://en.wikipedia.org/wiki/Personal_Public_Service_Number */
extern nmck_t nmck_ppsn(const char*, size_t);
extern void nmpr_ppsn(nmck_t, const char*, size_t);

/**
 * Check for Germany's taxpayer identification number, numeric input.
 * http://en.wikipedia.org/wiki/National_identification_number#Germany */
extern nmck_t nmck_idnr(const char*, size_t);
extern void nmpr_idnr(nmck_t, const char*, size_t);

/**
 * Check for Croatia's personal identification number, numeric input.
 * http://en.wikipedia.org/wiki/Personal_identification_number_(Croatia) */
extern nmck_t nmck_oib(const char*, size_t);
extern void nmpr_oib(nmck_t, const char*, size_t);

/**
 * Check for the UK's national health service number, numeric input.
 * http://en.wikipedia.org/wiki/NHS_number */
extern nmck_t nmck_nhs(const char*, size_t);
extern void nmpr_nhs(nmck_t, const char*, size_t);


/* utils */
/**
 * Auxiliary character class predicate in the style if ctype.h's isX(). */
static inline int ischeck(int c)
{
	return ((c + 0x10) | 0x20) - 0x10 == '_';
}

#endif	/* INCLUDED_numchk_h_ */

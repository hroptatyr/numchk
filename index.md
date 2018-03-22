---
title: numchk
project: numchk
layout: project
latest: https://github.com/hroptatyr/numchk/releases/download/v0.1.0/numchk-0.1.0.tar.xz
description: numchk, like file(1) for numbers
---

numchk
======

The numchk tool is a command-line tool that identifies self-consistent
numbers and checks their correctness.  In short, it is like `file(1)`
for numbers with check digits.

The focus is on fast bulk processing of identifiers arising primarily in
the financial sector.  Unlike similar libraries numchk only provides
syntactic and first level semantic checks (mostly check digits and
country code validation) in order to identify and disambiguate the
input.  Deep semantic verification, aspects of formatting and
canonicalisation are left to its competitors.


Red tape
--------
+ dependencies: C99, POSIX, [Ragel][1]
+ licensed under BSD3c


Resources
---------
+ homepage: <http://www.fresse.org/numchk/>
+ github:   <https://github.com/hroptatyr/numchk>
+ issues:   <https://github.com/hroptatyr/numchk/issues>
+ download: <https://github.com/hroptatyr/numchk/releases>


Supported checks
----------------

As of version 0.1 the following numbers can be identified and checked:

+ FIGIs (formerly known as Bloomberg Global Identifiers)
+ ISINs (according to ISO 6166:2013)
+ IBANs (according to ISO 13616-1:2007)
+ CUSIPs
+ SEDOLs
+ credit card account numbers

and many more.


Examples
--------

numchk comes with a no-frills UI that does what you'd expect:

    $ numchk JP3946600008
    JP3946600008	ISIN, conformant with ISO 6166:2013
    $ numchk BBG000XDNRC6 BBG005D215R6
    BBG000XDNRC6	FIGI, conformant with http://www.omg.org/spec/FIGI/1.0
    BBG005D215R6	FIGI, not conformant, should be BBG005D215R7
    $

Input from stdin is of course allowed:

    $ numchk <<EOF
    378282246310005
    3566 0020 2036 0505
    5901234123457
    978-3-16-148410-0
    EOF
    378282246310005	American Express, conformant account number
    3566 0020 2036 0505	JCB, conformant account number
    5901234123457	GTIN, conformant
    978-3-16-148410-0	ISBN, conformant with ISO 2108:2005
    $


Further Reading
---------------
+ [Wikipedia on check digits](https://en.wikipedia.org/wiki/Check_digit)


Similar projects
----------------
+ [python-stdnum][2]


  [1]: http://www.colm.net/open-source/ragel/
  [2]: https://arthurdejong.org/python-stdnum/


<!--
  Local variables:
  mode: auto-fill
  fill-column: 72
  filladapt-mode: t
  End:
-->

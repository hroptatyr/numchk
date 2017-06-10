numchk
======

The numchk tool is a command-line tool that identifies self-consistent
numbers and checks their correctness.  In short, it is like `file(1)`
for numbers.

The focus is on fast bulk processing of identifiers arising primarily in
the financial sector.  The design, inspired by [libarchive][1], allows for
easy extension.

Red tape
--------
+ no external dependencies
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
+ GTINs
+ ISBNs


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


  [1]: http://libarchive.org/

<!--
  Local variables:
  mode: auto-fill
  fill-column: 72
  filladapt-mode: t
  End:
-->

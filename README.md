numchk
======

The numchk tool is a command-line tool that identifies self-consistent
numbers and checks their correctness.  In short, it is like `file(1)`
for numbers.

The focus is on fast bulk processing of identifiers arising primarily in
the financial sector.  Unlike similar libraries numchk only provides
syntactic and first level semantic checks (mostly check digits and
country code validation) in order to identify and disambiguate the
input.  Deep semantic verification, aspects of formatting and
canonicalisation are left to its competitors.


Red tape
--------
+ dependencies: C99, POSIX, [Ragel][1] (for git builds)
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


Motivation
----------

The project arose as monitoring link in a daily tool chain that collects
security master data from different sources and prepares the result for
database insertion.  For possibly obvious reasons the integrity check
was not to be conducted during or after the database import.  Databases
in general aren't exactly known for its string manipulation or numerical
capabilities.  Besides, we needed the security identifiers to be
classified so we could extract information by ISIN, by FIGI, etc.

Luckily those identifiers can all be integrity-checked, i.e. just from
trying to verify the check digit you can distinguish ISINs from FIGIs.
And of course there are plenty of ready-to-use libraries, snippets or
command line tools that do the job, or are there not?

Unfortunately, most (all?) of them are written in Python, Javascript or
PHP, languages notorious for their slow string processing!  Throw in the
phenomenon that calculating check digits is one of the first projects
for most people, possibly right after or at least soon after Hello
World, and you know why checking and classifying 10 million security
identifiers is a job that takes an hour or two.  Did you know that a day
only has 24 hours?

There were two options:
- going horizontal (NVDA and INTC probably teamed up with the check
  digit mafia to sell you more xPU power)
- doing things properly

We chose the latter, or you would be reading another one of those
stories about how a modern stack with a modern language can bring you
back the speed of a hand-coded MC68000 assembler program whilst being
10000 times more maintainable (whatever that means and assuming you're a
Kubernetes expert already).  The upshot is: Our security identifier
checking and classification now takes an acceptable 12 to 15 seconds.

We then thought the whole tool would be more useful if it could detect
and verify all sorts of numbers and identifiers.


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

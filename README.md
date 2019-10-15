# Pareto Algebra Library 

What is the Pareto Algebra Library?

The Pareto Algebra Library is a library that implements Pareto Algebra [1].
You can find more information about Pareto Algebra and the library
at <http://www.es.ele.tue.nl/pareto>

It uses the libraries:

- [CUDD library](https://github.com/ivmai/cudd) of Fabio Somenzi (BSD license) 
- GBDD library of Marcus Nilsson (GPL license, no longer available online)

As such this library is available only under the GPL license.

------------------------------------------------------------------

## Installation and Use

How do I use / install it?

### Under Linux / gcc

See [README_LINUX.md](README_LINUX.md)

### Under Windows / Visual Studio

See [README_WIN.md](README_WIN.md)

### Using the library

No information available yet.

------------------------------------------------------------------

## Documentation

Is there any documentation for the Pareto Calculator?

There is currently no documentation for the library.
However, doxygen documentation is generated. It can be found after the project has been built in the directory `doc/codedoc/` as `index.html`

Additional information  can be found at http://www.es.ele.tue.nl/pareto.

------------------------------------------------------------------

What license is used for the Pareto Calculator?

The software is licensed under the GNU Public License. See the
file LICENSE for the exact software license.
GPL is adopted because a part of the software, the symbolic implementation
of the algebra uses the GBDD library, which is a GPL library.
In case you are interested in a version of the calculator with
a more liberal license, excluding the symbolic implementation, please contact
us.

------------------------------------------------------------------

## CONTACT INFORMATION

Marc Geilen <m.c.w.geilen@tue.nl>

## References

[1] M.C.W. Geilen, T. Basten, B.D. Theelen, R.H.J.M. Otten. [An Algebra of Pareto Points.](https://dl.acm.org/citation.cfm?id=1366010)
   Fundamenta Informaticae. 78(1):35-74, 2007. (Special issue with best papers of ACSD 2005.) IOS Press.

[2] M.C.W. Geilen, T. Basten. [A Calculator for Pareto Points](https://ieeexplore.ieee.org/document/4211810). in Proceedings of the 10th Design, Automation and Test in Europe Conference and Exhibition (DATE 2007) 16-20 April 2007, Nice, France. Institute of Electrical and Electronics Engineers, Piscataway, New Jersey, pp. 285-291, DOI: 10.1109/DATE.2007.364605


[2] H. Shojaei, T. Basten, M.C.W. Geilen, P. Stanley-Marbell, [SPaC: A symbolic Pareto Calculator](https://dl.acm.org/citation.cfm?id=1450176). in Proceedings of the International Conference on Hardware-Software Codesign and System Synthesis, CODES+ISSS 2008. Association for Computing Machinery, LNCS l., pp. 179-184.

# Pareto Calculator 

What is the Pareto Calculator?

The Pareto Calculator is a library that implements Pareto Algebra [1].
You can find more information about Pareto Algebra and the calculator
at <http://www.es.ele.tue.nl/pareto>

It uses the [CUDD library](https://github.com/ivmai/cudd) of Fabio Somenzi (BSD license) and the GBDD library of Marcus Nilsson (GPL license, no longer available online)

As such this library is available only under the GPL license.

------------------------------------------------------------------



## Installation and Use

How do I use / install it?

### Under Linux / gcc

1. clone cudd version 3.0.0 from git hub in the folder ParetoAlgebraLib with the command.
   `git clone --branch cudd-3.0.0 https://github.com/ivmai/cudd.git`
   You can also download the zip archive and unpack it.
   The CUDD `README` file, e.g., should then reside at `ParetoAlgebraLib/cudd/README`.
2. In the cudd directory type the following commands
   - `aclocal`,
   - `automake`
   - `./configure` and then
   - `make`

   It should have produce the static library `ParetoCalculatorLib/cudd/cudd/.libs/libcudd.a`
3. In the main `ParetoCalculatorLib` directory type
   - `cmake .` and then type
   - `make`


### Under Linux / gcc

There are some cmake files...

TBD

### Using the library

TBD.
------------------------------------------------------------------

## Documentation

Is there any documentation for the Pareto Calculator?

Documentation for the library can be found at http://www.es.ele.tue.nl/pareto.

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

[1] M.C.W. Geilen, T. Basten, B.D. Theelen, R.H.J.M. Otten. An Algebra of Pareto Points.
   Fundamenta Informaticae. 78(1):35-74, 2007. (Special issue with best papers of ACSD 2005.)  IOS Press.
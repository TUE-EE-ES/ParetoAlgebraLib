# Pareto Calculator 

What is the Pareto Calculator?

The Pareto Calculator is a library that implements Pareto Algebra [1].
You can find more information about Pareto Algebra and the calculator
at <http://www.es.ele.tue.nl/pareto>

------------------------------------------------------------------

## Installation and Use

How do I use / install it?

1. Download the file "cudd231.zip" to your computer from <https://people.eecs.berkeley.edu/~alanmi/research/extra/softPorts.htm> (Previously it was available from <http://web.cecs.pdx.edu/~alanmi/research/soft/ports/cudd231.zip>)
The original version of CUDD is distributed and supported by Fabio Somenzi <Fabio@Colorado.EDU>, <http://vlsi.colorado.edu/~fabio/> and the Windows port is distributed and supported by Alan Mishchenko, <http://www.eecs.berkeley.edu/~alanmi/>

2. Unzip the file into the `VC_Project\Pareto_Calculator\gbdd` directory and rename `cudd123` to `cudd`.
    The file `cudd.dsw` should now be located in the folder structure
    as: `paretocalc-2.0.1\VC_Project\Pareto_Calculator\gbdd\cudd\cudd.dsw`

3. Open `cudd.dsw` in Microsoft Visual Studio (File -> Open Solution...) and let it do its updates and conversions

4. In the file `cuddObj.h`, comment out line 33 (`#include <iostream.h>`) and uncomment lines 34 and 35:

``` c++
#include <iostream>
```

and

``` c++
using namespace std;
```

5. Compile the release version of the library by selecting the appropriate configuration (`Release`) from the drop-down list at the top of the window.

6. Close Visual Studio

7. Open `Pareto_Calculator.sln` in Microsoft Visual Studio (`File -> Open Solution...`)

8. Compile release versions of the project by selecting the appropriate configuration (`Release`) from the drop-down list at the top of the window.

This project also uses the Windows port of modified GBDD package of which the original version is distributed by Marcus Nilsson, <http://www.regularmodelchecking.com/software/>
who bears no responsibility for this port.
Applications developed and tested under UNIX GBDD, can be compiled and used on Windows with the Windows and modified release of GBDD. This version of GBDD has some extra features for supporting
non-integer values by BDD.

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

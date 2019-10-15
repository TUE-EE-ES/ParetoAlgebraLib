# Installation under Linux / gcc

Building the library requires that common build tools, including `automake` and `cmake` are installed on the system.

1. If you haven't already done so, clone the git

   `git clone https://github.com/TUE-FitOpTiVis/ParetoAlgebraLib.git`

2. clone cudd version 3.0.0 from git hub in the folder ParetoAlgebraLib with the command.

   `git clone --branch cudd-3.0.0 https://github.com/ivmai/cudd.git`

   You can also download the zip archive and unpack it.
   The CUDD `README` file, e.g., should then reside at `ParetoAlgebraLib/cudd/README`.
3. In the cudd directory type the following commands
   - `aclocal`
   - `automake`
   - `./configure`
   - `make`

   This should have produced the static library `ParetoCalculatorLib/cudd/cudd/.libs/libcudd.a`
4. In the main `ParetoCalculatorLib` directory, issue the following commands
   - `cmake .` and then type
   - `make`

   The the Pareto Calculator static library has been built in  `src/libParetoCalculator.a` and also three example applications in `examples`.


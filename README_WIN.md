# Installation under Windows / Visual studio

1. If you haven't already done so, clone the git

   `git clone https://github.com/TUE-FitOpTiVis/ParetoAlgebraLib.git`

2. Clone CUDD version 3.0.0 from GitHub in the folder `ParetoAlgebraLib` with the command.

   `git clone --branch cudd-3.0.0 https://github.com/ivmai/cudd.git`

   You can also download the zip archive and unpack it.
   The CUDD `README` file, e.g., should then reside at `ParetoAlgebraLib/cudd/README`.
3. Open the vs solution `VisualStudio/Pareto_Calculator.sln` and build it in the desired configuration (`Debug` or `Release`). Static libraries and executables are built in the `ParetoAlgebraLib\VisualStudio\Debug` or `ParetoAlgebraLib\VisualStudio\Release` folder.


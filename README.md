# Overview

Dirac is a command-line calculator app for polynomials of [Dirac matrices](https://en.wikipedia.org/wiki/Gamma_matrices).
Many problems in high-energy physics require simplification of such polynomials.
Dirac app converts them to linear combinations of the unit matrix,
$\gamma^\mu$, $\sigma^{\mu\nu}$, $\gamma^5\gamma^\mu$, and $\gamma^5$.
The result is printed in LaTeX format.

# Project structure
The application source code resides in the 'src' folder. The 'lib' folder contains code libraries used by the implementation.
The 'examples' folder containsis for usage examples (see the [Examples section](#examples)).

# Dependencies

The main app is written in C++20, so it requires a C++20-capable compiler.
CMake minimal version 3.0 is used as the build system.
Ruby 3 language interpreter, ``open3'' Ruby gem, and LaTeX installation with `pdflatex` are required to run example scripts.

This app uses [Eigen](https://eigen.tuxfamily.org) for linear algebra.
Eigen library is shipped with the code in the DIRAC_ROOT/lib/eigen folder.

# Build
Assuming that you start from the project root folder

```console
mkdir build
cd build
cmake ..
make
```
This produces 'dirac' executable in the build folder.
You can also use the build script build.sh that does the steps above. 
Command line options given to the script are passed to cmake.

# Usage
From the folder containing the dirac executable

```console
./dirac [options]
```
The behavior depends on the options provided.
If `-e` option is provided, followed by an expression, the app computes the expression,
prints the result to the output and exits:
```console
./dirac -e "\gamma_\mu\gamma_\nu"
\eta_{\nu\mu}   - I\eta_{\mu\omega_{1}}\eta_{\nu\omega_{2}}\sigma^{\omega_{1}\omega_{2}}
```
Note the quotes around the expression: the terminal would eat backslashes otherwise.
This is mostly useful when scripting (see the Examples section). If no expression is provided via command line,
the app starts an interactive shell.

The rest of command line options set various variables affecting the executable's behavior.
They are documented in the next section, as well as expression syntax.

# The interactive shell

There are three types of input expressions recognized by the interactive shell.

## Quit-expression
```console
dirac:> quit
```
Terminates the application

## Set-expression
```console
dirac:> #set <var-name> <var-value>
```
Sets environment variable identified by `var-name` to `var-value`.
The variables affect application behavior.

### Environment variables and their meanings

#### mode
Arithmetic mode. Possible values: `float` and `rational`. Default is rational.
Equivalent command line option `-m`.

```console
dirac:> #set mode float
dirac:> \gamma5\sigma^{\mu\nu}
-0.500000I{\epsilon^{\mu\nu}}_{\omega_{1}\omega_{2}}\sigma^{\omega_{1}\omega_{2}}
dirac:> #set mode rational
dirac:> \gamma5\sigma^{\mu\nu}
 - \frac{I}{2}{\epsilon^{\mu\nu}}_{\omega_{1}\omega_{2}}\sigma^{\omega_{1}\omega_{2}}
```
The mode variable also affects input parsing. Floating point numeric values are acceptable in float mode 
and are considered errors when the mode is rational.

The rational arithmetic implementation is the simplest possible. It does not e.g. handle overflows.
This was a conscious decision to avoid adding library dependencies. 
A more robust implementation may be added in the future if there is user demand for it.
Please inform me of any real world cases where overflows are a concern.

#### line_terms
Number of terms per output line. Possible values: integers or `inf` (meaning 'infinity'). Default: `inf`.
When this variable is set to a nonzero integer constant, LaTeX line breaks (as if inside 'split' environment) are inserted
after each `line_terms` terms.

```console
dirac:> #set line_terms 2
dirac:> \gamma_\kappa\gamma_\lambda\gamma_\mu\gamma_\nu
&\eta_{\lambda\kappa}\eta_{\nu\mu} + \eta_{\mu\lambda}\eta_{\nu\kappa}  -\\
&-\eta_{\mu\kappa}\eta_{\nu\lambda} + \left[ - I\eta_{\kappa\omega_{1}}\eta_{\lambda\omega_{2}}\eta_{\nu\mu}  -\right.\\
&\left. - I\eta_{\kappa\omega_{1}}\eta_{\mu\lambda}\eta_{\nu\omega_{2}} + I\eta_{\kappa\omega_{1}}\eta_{\mu\omega_{2}}\eta_{\nu\lambda} + \right.\\
&\left.+I\eta_{\kappa\lambda}\eta_{\omega_{1}\nu}\eta_{\omega_{2}\mu}   - I\eta_{\kappa\mu}\eta_{\omega_{1}\nu}\eta_{\omega_{2}\lambda} + \right.\\
&\left.+I\eta_{\kappa\nu}\eta_{\omega_{1}\mu}\eta_{\omega_{2}\lambda}\right]\sigma^{\omega_{1}\omega_{2}}   - I\epsilon_{\lambda\mu\nu\kappa}\gamma^5
dirac:> #set line_terms inf
dirac:> \gamma_\kappa\gamma_\lambda\gamma_\mu\gamma_\nu
\eta_{\lambda\kappa}\eta_{\nu\mu} + \eta_{\mu\lambda}\eta_{\nu\kappa}  -\eta_{\mu\kappa}\eta_{\nu\lambda} + \left[ - I\eta_{\kappa\omega_{1}}\eta_{\lambda\omega_{2}}\eta_{\nu\mu}   - I\eta_{\kappa\omega_{1}}\eta_{\mu\lambda}\eta_{\nu\omega_{2}} + I\eta_{\kappa\omega_{1}}\eta_{\mu\omega_{2}}\eta_{\nu\lambda} + I\eta_{\kappa\lambda}\eta_{\omega_{1}\nu}\eta_{\omega_{2}\mu}   - I\eta_{\kappa\mu}\eta_{\omega_{1}\nu}\eta_{\omega_{2}\lambda} + I\eta_{\kappa\nu}\eta_{\omega_{1}\mu}\eta_{\omega_{2}\lambda}\right]\sigma^{\omega_{1}\omega_{2}}   - I\epsilon_{\lambda\mu\nu\kappa}\gamma^5
```
Equivalent command line option: `-l`.

#### dummy
Dummy index template. Possible values: string literals. 
Greek letters in LaTeX notation are good choice.
Default is `\omega`.
Equivalent command line option: `-d`.
```console
dirac:> \gamma_\mu\gamma_\nu
\eta_{\nu\mu}   - I\eta_{\mu\omega_{1}}\eta_{\nu\omega_{2}}\sigma^{\omega_{1}\omega_{2}}
dirac:> #set dummy \sigma
dirac:> \gamma_\mu\gamma_\nu
\eta_{\nu\mu}   - I\eta_{\mu\sigma_{1}}\eta_{\nu\sigma_{2}}\sigma^{\sigma_{1}\sigma_{2}}
```

#### apply_symmetry
Controls whether the coefficient terms at $\sigma^{\mu\nu}$ are merged in the output by taking into account $\sigma$'s antisymmetry.
Possible values: `true` or `false`. Default is `true`. Command line equivalent: `-s`.
```console
dirac:> #set apply_symmetry false
dirac:> \gamma_\mu\gamma_\nu
\eta_{\nu\mu} + \left[ - \frac{I}{2}\eta_{\mu\omega_{1}}\eta_{\nu\omega_{2}} + \frac{I}{2}\eta_{\mu\omega_{2}}\eta_{\nu\omega_{1}}\right]\sigma^{\omega_{1}\omega_{2}}
dirac:> #set apply_symmetry true
dirac:> \gamma_\mu\gamma_\nu
\eta_{\nu\mu}   - I\eta_{\mu\omega_{1}}\eta_{\nu\omega_{2}}\sigma^{\omega_{1}\omega_{2}}
```
## Math-expression
All input lines that are neither quit-expressions nor set-expressions are considered computable math. 
The dirac application tries to parse and compute them.
Math syntax is LaTeX-like with some differences.
A valid expression consists of
- literals: alphanumeric sequences preceded by `\`. e.g. `\gamma`;
- integer or floating-point numbers;
- arithmetic operators `+,-,*,/`;
- subscript `_`;
- superscript `^`;
- brackets `{...}` (round or square brackets are not recognized for the sake of implementation simplicity).

Subscript `[head]_[tail]` and superscript `[head]^[tail]` are tensorial expressions. 
Unlike LaTeX, multiple non-bracketed subscripts and superscripts to a single head are possible,
but multi-level are not: `\eta_\mu_\nu` is valid input while `\gamma^{\eta_{\mu\nu}}` is not.

Literals' interpretation depends on their position in the input. 
Literals inside the tail of a tensorial expression are interpreted verbatim as tensor index labels.
Otherwise, only a limited number of special literals is recognized:
- `\I` - imaginary unit;
- `\gamma` - Dirac gamma-matrix;
- `\sigma` - Dirac sigma-matrix;
- `\gamma5` - $\gamma^5$ matrix;
- `\eta` - Minkowski metric;
- `\delta` - Kronecker delta;
- `\epsilon` - Levi-Civita symbol.

The app does not perform any validation of tensorial expression consistency 
save for checking that all basic tensors have correct index counts at computation stage.

# Examples

The folder DIRAC_ROOT/examples currently contains three Ruby sources.

common.rb contains some possibly reusable Ruby code facilitating command-line invocation of the dirac app from Ruby.

tests.rb implements a number of command-line app tests. In addition, it demonstrates how scripting can be used
to create formatted documents from dirac app output. 
Usage:
```console
ruby tests.rb /path/to/dirac/executable [output_name]
```
The script feeds a number of test inputs to dirac executable specified via command line,
collects results into a .tex file and invokes pdflatex on the latter.
If output_name is specified, the LaTeX name is output_name.tex and PDF is output_name.pdf (.tex extension if appended if not specified).
Otherwise, the default name tests.tex is used.

fierz6.rb simplifies a number of products of five basis matrices that occur in derivation of 6th-order Fierz identities. Usage:
```console
ruby fierz6.rb /path/to/dirac/executable
```
This produces fierz6.tex and fierz6.pdf documents with the results.

# Papers

arXiv: [Dirac: a command-line γ-matrix calculator](https://arxiv.org/abs/2301.09512)

In addition to the user guide above, the paper outlines motivation behind the software and mathematical approach to computation. 
Please cite if you use dirac in your research.

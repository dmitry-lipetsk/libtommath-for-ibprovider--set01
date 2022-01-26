# libtommath-for-ibprovider--set01
Clone of libtommath library

This library contains the direct copy of reworked version of LibTomMath library (free math library for work with large numbers) used by IBProvider.

License: WTFPL.

Original source code was obtained from Firebird SQL Server (v3): https://github.com/FirebirdSQL/firebird/tree/B3_0_Release/extern/libtommath

Home repository of LibTomMath: https://github.com/libtom/libtommath

The primary changes:
- Conversion to C++ code
- Added the asserts
- Correction of mistakes

The main target of this code:
- Education and Research

You may use this code as addtional source for research and preparation of original LibTomMath for using in your projects.

If you want to directly use THIS code in your project, you need to correct the paths to headers.

I created multiple tests for THIS code but they are part of my internal "Unit Test System for IBProvider" and won't publish. Sorry.

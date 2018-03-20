# Amalgamation

This tool merges c source files to create the amalgamation.


To merge files just create a file including your .c files.

merge.txt

```c

#include "source1.c"
#include "source2.c"
...

```

tool merge.txt out.c

The header files will be included (just once) automatically.
pragma once directive are removed on expation.

Tips:

* Make unique function/variables names even for internal linkage (static)

## Similar tools

https://github.com/svaarala/duktape/blob/master/tools/combine_src.py

https://github.com/vinniefalco/Amalgamate

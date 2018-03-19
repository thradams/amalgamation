# amalgamation

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


Tips:

* Don' t create static functions with repeted names.
* pragma once directive is removed.


# sintable.h

This is the output of [SinTableGen](https://github.com/NoWare-Development/sintablegen).

This header file contains 65536 sin values.

To get sin/cos out of some float you can do it like this:
``` c
#include "sintable.h"

// Get value's sin
float get_sin(float a) {
    return SIN_TABLE[(int)(a * 10430.378F) & (unsigned short)0xFFFF];
}

// Get value's cos
float get_cos(float a) {
    return SIN_TABLE[(int)(a * 10430.378F + 16384.0F) & (unsigned short)0xFFFF];
}
```

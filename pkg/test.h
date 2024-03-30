#pragma once
#include <stdio.h>

#define ASSERT(cond)                                                   \
    if (!(cond))                                                       \
    {                                                                  \
        fprintf(stderr, "\033[31mTest failed at line %s:%d:\033[0m\n", \
                __FILE__, __LINE__);                                   \
        return 1;                                                      \
    }

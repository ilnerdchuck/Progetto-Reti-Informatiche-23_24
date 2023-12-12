#pragma once
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#define ASSERT(cond)                                                   \
    if (!(cond))                                                       \
    {                                                                  \
        fprintf(stderr, "\033[31mTest failed at line %s:%d:\033[0m\n", \
                __FILE__, __LINE__);                                   \
        return 1;                                                      \
    }

// fmt_Sprintf return an heap allocated formatted string
char *fmt_Sprintf(const char *format, ...)
{
    va_list args;
    va_start(args, format);

    // Use vsnprintf with a null buffer to calculate the required length
    int length = vsnprintf(NULL, 0, format, args);
    va_end(args);

    // Allocate memory for the string
    char *result = (char *)malloc(length + 1); // +1 for the null terminator
    if (!result)
    {
        return NULL;
    }

    // Actually print the string
    va_start(args, format);
    vsnprintf(result, length + 1, format, args);
    va_end(args);

    return result;
}

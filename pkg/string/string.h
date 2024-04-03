#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

int get_usr_pwd(const char* buff, char* usr, char* pwd){
    
    sscanf(buff, "%s %s", usr, pwd);
    return 0; 
}

// fmt_Sprintf return an heap allocated formatted string
char* fmt_Sprintf(const char *format, ...)
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

int get_usr_pwd_port(const char* buff, char* usr, char* pwd, int* cs_port){
    
    sscanf(buff, "%s %s %d", usr, pwd, cs_port);
    return 0; 
}

int strmalloc(char** dst, const char* src) {
    if(!src){
        return -1;
    }
    *dst = malloc(strlen(src)+1);
    // while (*src)
    //     *dst++ = *src++;
    // *dst = '\0';
    strcpy(*dst, src);
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

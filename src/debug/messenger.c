#include "messenger.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

// debug.h implementation
#define BUFFER_LENGTH 512


// debug_log function
int debug_log (DebugSeverity severity, const char *file, const char *func, const int line, const char *format, ...) {

    char bufferString1[BUFFER_LENGTH]; // bufferstring preallocated for stack storage
    char bufferString2[BUFFER_LENGTH]; // I need 2

    va_list args;
    va_start (args, format);

    // messege severity prefixes
    const char *messegeSeverity[] = {
        "\033[1;31m[FATAL]: \033[0m",
        "\033[1;33m[ERROR]: \033[0m",
	    "\033[1;96m[VALIDATION]: \033[0m",
        "\033[1;32m[INFO]: \033[0m",
        "\033[1;35m[DEBUG]: \033[0m"
    };

    // pupulate bufferstring 1 with messege and args and all that
    vsnprintf (bufferString1, BUFFER_LENGTH, format, args);

    snprintf (
        bufferString2,
        BUFFER_LENGTH,
        ">>> %s:%s->%i\n\t%s%s\n",
        file,
        func,
        line, 
        messegeSeverity[severity],
        bufferString1
    );

    // TODO fancy output
    printf ("%s", bufferString2);
    va_end (args);

    return 0;
    
}

#include "messenger.h"
#include <stdarg.h>
#include <stdio.h>

// debug.h implementation
#define BUFFER_LENGTH 2048
#define BUFFER_LENGTH_FINAL BUFFER_LENGTH + 128

// debug_log function
int debug_log(DebugSeverity severity, const char *file, const char *func,
    const int line, const char *format, ...)
{

    static char bufferString1[BUFFER_LENGTH];
    static char bufferString2[BUFFER_LENGTH_FINAL];
    va_list args;
    va_start(args, format);

    // messege severity prefixes
    const char *messegeSeverity[DEBUG_TYPE_MAX] = {
        "\033[1;31m[FATAL]: \033[0m",      // fatal
        "\033[1;33m[ERROR]: \033[0m",      // error
        "\033[1;33m[WARNING]: \033[0m",    // warning
        "\033[1;96m[VALIDATION]: \033[0m", // validation
        "\033[1;32m[INFO]: \033[0m",       // info
        "\033[1;35m[DEBUG]: \033[0m"       // debug
    };

    // print error messege
    vsnprintf(bufferString1, BUFFER_LENGTH, format, args);

    uint16_t strlen = snprintf(bufferString2, // output
        BUFFER_LENGTH,                        // max length
        ">>> %s (%s:%i)\n\t%s%s\n",           // format string
        func,                                 // function name
        file,                                 // filename
        line,                                 // line number
        messegeSeverity[severity],            // messege severity prefix
        bufferString1                         // error messege
    );

    if (strlen >= BUFFER_LENGTH_FINAL)
    {
        log_warning(
            "Debug called on line %i:%s overflowed buffer.", line, file);
    }

    // TODO log to file
    printf("%s", bufferString2);
    va_end(args);

    return 0;
}

#ifndef __GNUC__
#warning Function may result in undefined behaivour passing NULL 'm' value
#endif
int unix_log_perror(
    const char *file, const char *func, const char *line, const char *m, ...)
{

    if (!file || !func || !line)
        return SPIRIT_FAILURE;
    // define the prefix
    const char messegePrefix[] = "\033[1;34m[PERROR]: \033[0m Msg: ";
    const char messegeSuffix[] = " | Error";

    u32 fileLen = strlen (file);
    u32 funcLen = strlen (func);
    u32 lineLen = strlen (line);

    u32 mLen;
    m && (mLen = strlen(m));

    // add 2 chars to make room for newline and tab
    u32 length = fileLen + funcLen + lineLen + mLen;
    length += sizeof(messegePrefix);
    length += sizeof(messegeSuffix);
    length += 10; // hardcoded number of extra chars in format (eg. ">>>")

    if (m)
        length += sizeof(messegeSuffix);

    va_list args;
    va_start(args, m);

    char consolidatedMessege[1024];
    if (snprintf(consolidatedMessege, 1024, m, args) > 1024)
    {
        log_error("string length overflowed buffer!");
    }

    va_end(args);

    char prefix[length];
    snprintf(prefix,
        length,
        ">>> %s (%s:%s)\n\t%s%s%s",
        func,
        file,
        line,
        messegePrefix,
        consolidatedMessege,
        messegeSuffix);

    perror(prefix);

    return SPIRIT_SUCCESS;
}
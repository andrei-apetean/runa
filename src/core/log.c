#include <stdarg.h>
#include <stdio.h>

#include "log.h"

FILE* log_file;

void print(enum severity severity, const char* fmt, ...)
{
    // TODO: API to open log_file;
    log_file = stdout;
    const char* prefixes[] = {
        "[crt] ",
        "[err] ",
        "[wrn] ",
        "[inf] ",
        "[dbg] ",
    };
    va_list args;
    va_start(args, fmt);
    fprintf(log_file, "%s", prefixes[severity]);
    fprintf(log_file, fmt, args);
    fflush(log_file);
    va_end(args);
}


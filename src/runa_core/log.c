#include <stdarg.h>
#include <stdio.h>

#include "log.h"

void print(enum severity severity, const char* fmt, ...) {
    // todo: api to open log_file;
    const char* prefixes[] = {
        "[crt] ",
        "[err] ",
        "[wrn] ",
        "[inf] ",
        "[dbg] ",
    };
    va_list args;
    va_start(args, fmt);
    fprintf(stdout, "%s", prefixes[severity]);
    vfprintf(stdout, fmt, args);
    fflush(stdout);
    va_end(args);
}


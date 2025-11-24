#ifndef LOG_H
#define LOG_H

enum severity 
{
    log_critical = 0,
    log_error,
    log_warning,
    log_info,
    log_debug
};

void print(enum severity severity, const char* fmt, ...);

#define logcrt(fmt, ...) print(log_critical, (fmt), ##__VA_ARGS__);
#define logerr(fmt, ...) print(log_error, (fmt), ##__VA_ARGS__);
#define logwrn(fmt, ...) print(log_warning, (fmt), ##__VA_ARGS__);
#define loginf(fmt, ...) print(log_info, (fmt), ##__VA_ARGS__);
#define logdbg(fmt, ...) print(log_debug, (fmt), ##__VA_ARGS__);

#endif /* LOG_H */

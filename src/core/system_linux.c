#ifdef __linux__

#include <sys/time.h>
#include <unistd.h>

#include "system.h"

#define ONE_MILLION 1000000

uint64_t timestamp(void)
{
    struct timeval val;
    gettimeofday(&val, NULL);
    return (uint64_t)val.tv_usec + (uint64_t)val.tv_sec * ONE_MILLION;
}

void ssleep(uint64_t us) 
{
    usleep(us);
}

#endif // __linux__

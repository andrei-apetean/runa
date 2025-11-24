#include "core.h"
#include "log.h"
#include "system.h"

int running = 0;
uint64_t last = 0;
uint64_t elapsed = 0;
uint64_t start = 0;


int init()
{
    loginf("Initializing...\n");
    logcrt("Test critical\n");
    logerr("Test error\n");
    logwrn("Test warning\n");
    loginf("Test info\n");
    logdbg("Test debug\n");
    return 0;
}


int loop()
{
    running = 1;
    int i = 0;
    while (running) {
        start = timestamp();
        loginf("start: %zu\n", start);
        loginf("end: %zu\n", last);
        loginf("elapsed: %zu\n", elapsed);
        i++;
        if (i > 10){
            running = 0;
        }
        ssleep(10);
        last = timestamp();
        elapsed = last - start;

    }
    return 0;
}


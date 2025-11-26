#include "core.h"
#include "log.h"
#include "rvulkan.h"
#include "system.h"
#include "time.h"

int running = 0;
uint64_t last = 0;
uint64_t elapsed = 0;
uint64_t start = 0;
struct syscon syscon;
struct rvulkan vk;

int init()
{
    loginf("Initializing...\n");
    int err = 0;
    err = system_connect(&syscon);
    if (err) {
        logerr("Failed to connect to the system!\n");
        return err;
    }

    err = init_vulkan(&syscon, &vk);
    if (err) {
        logerr("Failed to initialize vulkan!\n");
        return err;
    }

    loginf("Initialzation successful!\n");
    return err;
}

int loop()
{
    running = 1;
    volatile int i = 0;
    while (running) {
        start = timestamp();
        system_read_events(&syscon);
        i++;
        if (i > 10){
            running = 0;
        }
        ssleep(1);
        last = timestamp();
        elapsed = last - start;
        float dt = elapsed * 0.001f;
        loginf("dt: %.3f\n", dt);
        loginf("elapsed: %zu\n", elapsed);
    }
    return 0;
}


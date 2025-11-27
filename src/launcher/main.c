
#include "core.h"
#include "log.h"
#include "system.h"
#include "vulkan.h"

int running = 0;
struct syscon syscon;
struct rvulkan vk;

int main() {
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
    volatile int i = 0;
    while (running) {
        system_read_events(&syscon);
        loop();
        i++;
        if (i > 10){
            running = 0;
        }
    }
}

#include "system_linux.c"

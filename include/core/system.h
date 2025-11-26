#ifndef SYSTEM_H
#define SYSTEM_H

#include <stdint.h>

#include "core.h"

enum surface_api {
    SURFACE_API_WL = 0,
    SURFACE_API_COUNT,
};

int system_connect(struct syscon* connection);
void system_read_events(struct syscon* connection);
void system_disconnect(struct syscon* connection);
enum surface_api system_surface_api();
#endif /* SYSTEM_H */

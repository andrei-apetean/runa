#ifndef SYSTEM_H
#define SYSTEM_H

#include <stdint.h>

#include "core.h"

int system_connect(struct syscon* connection);
void system_read_events(struct syscon* connection);
void system_disconnect(struct syscon* connection);
#endif /* SYSTEM_H */

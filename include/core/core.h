#ifndef CORE_H
#define CORE_H

#include <stdint.h>

#define container_of(ptr, type, member) \
    ((type *)((char *)(ptr) - offsetof(type, member)))

#define count_of(arr) (sizeof(arr)/(sizeof(arr[0])))

int init();
int loop();

#endif /* CORE_H */

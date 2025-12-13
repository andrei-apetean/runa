#ifndef RUNA_H
#define RUNA_H

#include <stdint.h>

#define container_of(ptr, type, member) \
    ((type *)((char *)(ptr) - offsetof(type, member)))

#define count_of(arr) (sizeof(arr)/(sizeof(arr[0])))

struct runa {
    int (*log)(const char*, ...);
};

struct runa_itf {
    int  (*on_init)(struct runa_itf*);
    void (*on_update)(struct runa_itf*);
    void (*on_terminate)(struct runa_itf*);
};

struct runa_itfd {
    struct runa_itf* (*load)(void*, const struct runa* const);
    int              size_bytes;
    char             name[56];
};

void load_itfd(struct runa_itfd* descriptor);

#endif /* RUNA_H */


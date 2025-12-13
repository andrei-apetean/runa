#ifndef CORE_H
#define CORE_H

#include <stdint.h>
#include "runa/runa.h"

struct module {
    struct runa_itfd descriptor;
    struct runa_itf* itf;
    uintptr_t        handle;
    uintptr_t        mem;
};

struct rt {
    uint64_t       (*timestamp)(void);
    void           (*idle)(uint64_t);
    int            (*log)(const char*, ...);
    int            (*read_events)(void);
    int            modcount;
    struct module* modules;
    struct runa    runa;
};

int  init(struct rt* rt);
void terminate(struct rt* rt);
void loop(struct rt* rt);

#endif /* CORE_H */

#include <stdlib.h>
#include <string.h>

#include "runa_core/core.h"

static int check_rt(struct rt* rt)
{
    if (!rt)              return -1;
    if (!rt->timestamp)   return -1;
    if (!rt->idle)        return -1;
    if (!rt->log)         return -1;
    if (!rt->read_events) return -1;
    return 0;
}

int init( struct rt* rt)
{
    int err = check_rt(rt);
    if (err) return err;
    rt->runa = (struct runa){
        .log = rt->log,
    };

    for (int i = 0; i < rt->modcount; i++) {
        struct module* m = &rt->modules[i];
        if (!m->descriptor.load) continue; /* TODO: remove from list */; 

        void* mem = 0; 
        if (m->descriptor.size_bytes > 0 ) {
            mem = malloc(m->descriptor.size_bytes);
            memset(mem, 0, m->descriptor.size_bytes);
            if (!mem) continue; /* TODO: remove from list */;
        }
        m->mem = (uintptr_t)mem;
        rt->log("Loading module: %s...\n", m->descriptor.name);
        uint64_t start = rt->timestamp();
        m->itf = m->descriptor.load(mem, &rt->runa);
        uint64_t now = rt->timestamp();
        float dt = (now - start) * 0.001f;
        rt->log("Done (%.3f us)\n", m->descriptor.name, dt);
    }

    for (int i = 0; i < rt->modcount; i++) {
        rt->modules[i].itf->on_init(rt->modules[i].itf);
    }
    return 0;
}

void terminate(struct rt* rt)
{
    for (int i = 0; i < rt->modcount; i++) {
        struct module* m = &rt->modules[i];
        rt->log("Unloading module: %s...\n", m->descriptor.name);
        m->itf->on_terminate(m->itf);
        void* mem = (void*)m->mem;
        free(mem);
    }
}

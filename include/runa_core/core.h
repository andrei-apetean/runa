#ifndef CORE_H
#define CORE_H

#include <stdint.h>

#define container_of(ptr, type, member) \
    ((type *)((char *)(ptr) - offsetof(type, member)))

#define count_of(arr) (sizeof(arr)/(sizeof(arr[0])))

struct module {
    int id;
};

struct module_info {
    int size_bytes;
    char name[64];
};

struct rt {
    uint64_t (*timestamp)();
    void     (*sleep)(uint64_t);
    int      (*log)(const char*, ...);
    int      (*read_events)();
};

struct ini {
    void* (*dlopen)(const char*);
    void* (*dlsym)(void*, const char*);
    void* (*dlclose)(void*, const char*);
};

int init(struct ini* ini);
void terminate();
void loop(struct rt* rt);

#endif /* CORE_H */

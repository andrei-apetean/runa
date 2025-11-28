
#include "core.h"

uint64_t last = 0;
uint64_t elapsed = 0;
uint64_t start = 0;

void loop(struct rt* rt)
{
   start = rt->timestamp();
   rt->read_events();
   rt->sleep(1);
   last = rt->timestamp();
   elapsed = last - start;
   float dt = elapsed * 0.001f;
   rt->log("dt: %.3f\n", dt);
   rt->log("elapsed: %zu\n", elapsed);
}

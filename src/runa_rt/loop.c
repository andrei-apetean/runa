
#include "core.h"
#include "log.h"
#include "time.h"

uint64_t last = 0;
uint64_t elapsed = 0;
uint64_t start = 0;

void loop()
{
   start = timestamp();
   ssleep(1);
   last = timestamp();
   elapsed = last - start;
   float dt = elapsed * 0.001f;
   loginf("dt: %.3f\n", dt);
   loginf("elapsed: %zu\n", elapsed);
}


#include "core.h"

int main() {
    int err = init();
    if (err) {
        return -1;
    }
    return loop();
}


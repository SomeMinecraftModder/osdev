#include "../kernel/panic.h"
#include "assert.h"

void assert(int retval) {
    if (!retval) {
        PANIC(__FILE__, __LINE__, "Assertion failed!");
    }
}

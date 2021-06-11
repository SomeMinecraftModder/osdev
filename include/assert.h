#ifndef ASSERT_H
#define ASSERT_H

#include "../kernel/panic.h"

#ifdef NDEBUG
    #define assert(x) (void)0
#else
    #define assert(x) \
        ((void)((x) || (PANIC(__FILE__, __LINE__, "Assertion failed."), 0)))
#endif

#if __STDC_VERSION__ >= 201112L
    #define static_assert _Static_assert
#endif

#endif

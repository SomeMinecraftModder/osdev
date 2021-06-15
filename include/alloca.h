#ifndef _ALLOCA_H
#define _ALLOCA_H

#include <stddef.h>

#ifdef alloca
    #undef alloca
#endif

void *alloca(size_t __size);

#ifdef __GNUC__
    #define alloca(size) __builtin_alloca(size)
#endif

#endif

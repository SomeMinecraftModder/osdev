#ifndef SETJMP_H
#define SETJMP_H

typedef intptr_t jmp_buf[5];

#define setjmp  __builtin_setjmp
#define longjmp __builtin_longjmp

#endif

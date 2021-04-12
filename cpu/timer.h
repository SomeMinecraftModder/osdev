#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

extern uint32_t secboot;
extern uint32_t minboot;
extern uint32_t hourboot;

void init_timer(uint32_t freq);
void sleep(uint32_t sec);

#endif

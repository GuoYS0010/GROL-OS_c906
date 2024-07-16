#ifndef __TIMER_H__
#define __TIMER_H__
#include "platform.h"
#include "types.h"
#include "riscv.h"
#include "printf.h"

#define TIMER_INTERVAL CLINT_TIMEBASE_FREQ

/* load timer interval(in ticks) for next timer interrupt.*/
void timer_load(int interval);

void timer_init();

void timer_handler();


#endif
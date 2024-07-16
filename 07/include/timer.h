#ifndef __TIMER_H__
#define __TIMER_H__
#include "platform.h"
#include "types.h"
#include "riscv.h"
#include "printf.h"
#include "task.h"

#define TIMER_INTERVAL (CLINT_TIMEBASE_FREQ / CLINT_SLICE_PER_SEC)



#define SOFTWARE_NOTHING                0
#define SOFTWARE_KERNEL_SWITCH          1
#define SOFTWARE_TASK_CONTROLLER_FLAG   2
#define SOFTWARE_USER_SWITCH_FLAG       3
#define SOFTWARE_USER_KILLMYSELF        4

//in flag 1, the arg means the context we want to switch



void schedule(void);
/* load timer interval(in ticks) for next timer interrupt.*/
void timer_load(int interval);

void timer_init();

void timer_handler();

void newpriority();

uint64_t get_time();
void get_time_str(char* time);

void change_slice(int slice);
void clear_slice();

void software_handler();
void call_software_interrupt(uint8_t flag, uint64_t arg);
void reset_software_interrupt();

#endif
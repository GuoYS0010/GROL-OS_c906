#include "timer.h"
extern void switch_to(struct context *next);
static uint64_t TICKS_PER_SLICE = 60;
static uint64_t _tick = 0;
static uint64_t _slice = 0;

static uint8_t software_interrupt_flag = 0;
static uint64_t software_interrupt_arg;
/* load timer interval(in ticks) for next timer interrupt.*/
void timer_load(int interval)
{
	/* each CPU has a separate source of timer interrupts. */
	int id = r_mhartid();
    reg_t MTIME = r_time();
	w_mtimecmp(MTIME + interval);
	}

void timer_init()
{
	/*
	 * On reset, mtime is cleared to zero, but the mtimecmp registers 
	 * are not reset. So we have to init the mtimecmp manually.
	 */
	timer_load(TIMER_INTERVAL);

	/* enable machine-mode timer interrupts. */
	w_mie(r_mie() | MIE_MTIE);

	/* enable machine-mode global interrupts. */
	w_mstatus(r_mstatus() | MSTATUS_MIE);
	printf("timer init!!\n\r");
}
uint64_t get_time(){
    return _tick / CLINT_SLICE_PER_SEC;
}

void get_time_str(char* time){
    int index = 0;
    uint64_t tick_sec = _tick / CLINT_SLICE_PER_SEC;
    uint8_t tmp = (tick_sec / 3600);
    time[0] = (tmp % 100)/ 10 + '0'; 
    time[1] = tmp % 10 + '0'; 
    time[2] = ':';
    tmp = (tick_sec % 3600)/60;
    time[3] = tmp / 10 + '0'; 
    time[4] = tmp % 10 + '0'; 
    time[5] = ':';
    tmp = tick_sec % 60;
    time[6] = tmp / 10 + '0'; 
    time[7] = tmp % 10 + '0'; 

}
void timer_handler() 
{
	_tick++;
	_slice++;
    // #ifdef MYPRINT
	// printf("tick: %d\n\r", _tick);
    // #endif

	timer_load(TIMER_INTERVAL);
	char timestr[9];
	get_time_str(timestr);
	timestr[8] = 0;
    #ifdef MYPRINT
	//printf("%s\n\r", timestr);
    #endif
	if (_slice == TICKS_PER_SLICE){
		printf("\n\rcurrent slice and TICKS_PER_SLICE:%d,%d,\n\r", _slice, TICKS_PER_SLICE);
		schedule();
	}
}

void change_slice(int slice){TICKS_PER_SLICE = slice;}
void clear_slice(){_slice = 0;}

void software_handler(){
	uint8_t flag = software_interrupt_flag;
	uint64_t arg = software_interrupt_arg;
	reset_software_interrupt();
	struct context* pmycontext = r_mscratch();
	switch(flag){
		case SOFTWARE_NOTHING:
			printf("nothing happend\n\r");
			break;
		case SOFTWARE_KERNEL_SWITCH:
			#ifdef MYPRINT
			printf("kernel switch to task controller\n\r");
			printf("arg%p\n\r", arg);
			#endif
			schedule();
			break;
		case SOFTWARE_TASK_CONTROLLER_FLAG:
			//printf("task controller calling\n\r");
			switch_to((uint64_t) arg);
			break;
		case SOFTWARE_USER_SWITCH_FLAG:
			printf("user want to switch\n\r");
			schedule();
			break;
		case SOFTWARE_USER_KILLMYSELF:
			pmycontext->flags = 0;
			newpriority();
			schedule();
			break;
		default:
			printf("UNKNOWN SOFTWARE INTERRUPT!!\n\r");
			break;
	}
}


void call_software_interrupt(uint8_t flag, uint64_t arg){
	software_interrupt_flag = flag;
	software_interrupt_arg = arg;
	reg_t hartid = r_mhartid();
	w_msip(hartid, 1);
}
void reset_software_interrupt(){
	software_interrupt_flag = 0;
	software_interrupt_arg = 0;
	reg_t hartid = r_mhartid();
	w_msip(hartid, 0);
}

#include "timer.h"

static uint64_t _tick = 0;

static uint64_t get_time(){
    return _tick;
}

static void get_time_str(char* time){
    int index = 0;
    uint8_t tmp = (_tick / 3600);
    time[0] = (tmp % 100)/ 10 + '0'; 
    time[1] = tmp % 10 + '0'; 
    time[2] = ':';
    tmp = (_tick % 3600)/60;
    time[3] = tmp / 10 + '0'; 
    time[4] = tmp % 10 + '0'; 
    time[5] = ':';
    tmp = _tick % 60;
    time[6] = tmp / 10 + '0'; 
    time[7] = tmp % 10 + '0'; 

}

/* load timer interval(in ticks) for next timer interrupt.*/
void timer_load(int interval)
{
	/* each CPU has a separate source of timer interrupts. */
	int id = r_mhartid();
    reg_t MTIME;
    get_MTIME(MTIME);
	MTIME += interval;
	*(uint32_t*)CLINT_MTIMECMPL(id) = (uint32_t)(MTIME & 0xffffffff) ;
	*(uint32_t*)CLINT_MTIMECMPH(id) = (uint32_t)((MTIME >> 32) & 0xffffffff) ;
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

void timer_handler() 
{
	_tick++;
    #ifdef MYPRINT
	printf("tick: %d\n\r", _tick);
    #endif

	timer_load(TIMER_INTERVAL);
	char timestr[9];
	get_time_str(timestr);
	timestr[8] = 0;
    #ifdef MYPRINT
	printf("%s\n\r", timestr);
    #endif
}

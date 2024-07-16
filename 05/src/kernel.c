#include "os.h"

/*
 * Following functions SHOULD be called ONLY ONE time here,
 * so just declared here ONCE and NOT included in file os.h.
 */


void start_kernel(void)
{
	uart_init();
	uart_puts("Hello, GROL_OS!\n\r");//this should be replaced with graph later
	#ifdef MYPRINT
	printf("mtvec:%p\n\r", r_mtvec());
	printf("mstatus:%p\n\r", r_mstatus());
		printf("mcause:%p\n\r", r_mcause());
		printf("mtval:%p\n\r", r_mtval());
		printf("mepc:%p\n\r", r_mepc());
	print_printftest();
	#endif
	trap_init();
	plic_init();
	#ifdef MYPRINT
	printf("mtvec:%p\n\r", r_mtvec());
	printf("mstatus:%p\n\r", r_mstatus());
	printf("mie:%p\n\r", r_mie());
		printf("mcause:%p\n\r", r_mcause());
	//trap_test();
	#endif

	page_init();
	print_heapinit();
	#ifdef MYPRINT
	page_test();
	#endif
	print_multitasktest();
	sched_init();

	os_main();

	schedule();

	uart_puts("Would not go here!\n\r");

	while (1) {}; // stop here!
}


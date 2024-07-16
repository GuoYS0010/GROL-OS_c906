
int uart_putc(char ch);
void uart_init();
void uart_puts(char *s);
void start_kernel(void)
{

	uart_init();
	while(1){
	uart_puts("Hello, GROL-OS!\n\r");
	uart_puts("Goodbye, GROL-OS!\n\r");

	}
}


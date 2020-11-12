#include "contty.h"
#include "serial.h"

int con_init(void)
{
	return 0;
}

void con_putchar(int c)
{
	ser_putchar(c);
}

int con_getchar(void)
{
	return ser_getchar();
}

#include <stdio.h>
#include "asm.h"

void panic(const char *fmt, ...)
{
	disable_intr();

	printf("~~~~~ rpikern panic ~~~~~\n");
	printf("TODO: reg dump\n");

	for(;;) halt_cpu();
}

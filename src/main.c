#include "config.h"

#include <string.h>
#include <stdint.h>
#include "asm.h"
#include "serial.h"

void dbgled(int x);
void exit(int x);

int main(void)
{
	disable_intr();

	dbgled(2);

	init_serial(115200);
	ser_printstr("starting rpkern\n");

	for(;;) {
		int c = ser_getchar();
		if(c == -1) {
			ser_printstr("error!\n");
		} else {
			ser_printstr("got: ");
			ser_putchar(c);
			ser_putchar('\r');
			ser_putchar('\n');
		}
	}

	return 0;
}

void panic(void)
{
	dbgled(1);
	exit(0);
}

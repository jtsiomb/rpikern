#include "config.h"

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include "asm.h"
#include "rpi.h"
#include "contty.h"
#include "serial.h"
#include "video.h"

void dbgled(int x);

static void cmdrun(char *cmd);

int main(void)
{
	int lastnl = 0;
	static char cmdbuf[256];
	static int cmdend;

	rpi_init();
	init_serial(115200);
	con_init();

	printf("Detected raspberry pi %d, I/O base: %x\n", rpi_model, rpi_iobase);

	video_init();

	printf("Going interactive\n");
	for(;;) {
		int c = getchar();

		switch(c) {
		case '\r':
		case '\n':
			if(!lastnl) {
				ser_printstr("\r\n");
				cmdbuf[cmdend] = 0;
				cmdend = 0;
				cmdrun(cmdbuf);
			}
			lastnl = 1;
			break;

		case -1:
			lastnl = 0;
			printf("error!\n");
			break;

		default:
			lastnl = 0;
			ser_putchar(c);
			if(cmdend < sizeof cmdbuf) {
				cmdbuf[cmdend++] = c;
			}
		}
	}

	return 0;
}

static void cmdrun(char *cmd)
{
	char *ptr, *args;

	while(*cmd && isspace(*cmd)) cmd++;
	ptr = cmd;
	while(*ptr && !isspace(*ptr)) ptr++;
	*ptr = 0;
	args = ptr + 1;

	if(strcmp(cmd, "help") == 0) {
		printf("help not implemented yet\n");
	} else if(strcmp(cmd, "ver") == 0) {
		printf("rpikern version 0.0\n");
	} else {
		printf("Unknown command: %s\n", cmd);
	}
}

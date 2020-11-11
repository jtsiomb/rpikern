#include "config.h"

#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include "asm.h"
#include "serial.h"
#include "video.h"

void dbgled(int x);
void exit(int x);

static void cmdrun(char *cmd);

int main(void)
{
	int lastnl = 0;
	static char cmdbuf[256];
	static int cmdend;

	init_serial(115200);
	ser_printstr("Starting rpikern\n");

	video_init();

	ser_printstr("Going interactive\n");
	for(;;) {
		int c = ser_getchar();

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
			ser_printstr("error!\n");
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

void panic(void)
{
	ser_printstr("PANIC!\n");
	exit(0);
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
		ser_printstr("help not implemented yet\n");
	} else if(strcmp(cmd, "ver") == 0) {
		ser_printstr("rpikern version 0.0\n");
	} else {
		ser_printstr("Unknown command: ");
		ser_printstr(cmd);
		ser_printstr("\n");
	}
}

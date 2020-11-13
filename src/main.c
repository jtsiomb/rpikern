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

void reboot(void);

static void cmdrun(char *cmd);

int main(void)
{
	int lastnl = 0;
	static char cmdbuf[256];
	static int cmdend;

	rpi_init();
	/*init_serial(115200); done in rpi_init now for early debugging */
	con_init();

	printf("Detected raspberry pi %d, I/O base: %x\n", rpi_model, rpi_iobase);
	printf("Main RAM base: %x, size: %u bytes\n", rpi_mem_base, rpi_mem_size);
	printf("Video RAM base: %x, size: %u bytes\n", rpi_vmem_base, rpi_vmem_size);

	video_init();

	printf("Going interactive\n");
	for(;;) {
		while(ser_pending()) {
			int c = getchar();

			switch(c) {
			case '\r':
			case '\n':
				if(!lastnl) {
					printf("\r\n");
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
				putchar(c);
				if(cmdend < sizeof cmdbuf) {
					cmdbuf[cmdend++] = c;
				}
			}
		}

		/*video_update(1);*/
	}

	return 0;
}

void reboot(void)
{
	printf("Rebooting ...\n");
	rpi_reboot();
}

static void cmdrun(char *cmd)
{
	static int cur_x, cur_y;
	char *ptr, *args;

	while(*cmd && isspace(*cmd)) cmd++;
	ptr = cmd;
	while(*ptr && !isspace(*ptr)) ptr++;
	*ptr = 0;
	args = ptr + 1;

	if(strcmp(cmd, "reboot") == 0) {
		reboot();

	} else if(strcmp(cmd, "down") == 0) {
		printf("scroll down\n");
		cur_y += 10;
		video_scroll(cur_x, cur_y);

	} else if(strcmp(cmd, "up") == 0) {
		printf("scroll up\n");
		cur_y -= 10;
		video_scroll(cur_x, cur_y);

	} else if(strcmp(cmd, "help") == 0) {
		printf("help not implemented yet\n");
	} else if(strcmp(cmd, "ver") == 0) {
		printf("rpikern version 0.0\n");
	} else {
		printf("Unknown command: %s\n", cmd);
	}
}

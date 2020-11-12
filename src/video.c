#include "config.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "rpi.h"
#include "video.h"
#include "mem.h"

/* needs to by 16-byte aligned, because the address we send over the mailbox
 * interface, will have its 4 least significant bits masked off and taken over
 * by the mailbox id
 */
static uint8_t propbuf[64] __attribute__((aligned(16)));

int video_init(void)
{
	return 0;
}

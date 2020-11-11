#include "config.h"
#include <string.h>
#include <stdint.h>
#include "video.h"
#include "serial.h"
#include "mem.h"

#define MBOX_READ_REG	(*(volatile uint32_t*)(IO_BASE | 0xb880))
#define MBOX_POLL_REG	(*(volatile uint32_t*)(IO_BASE | 0xb890))
#define MBOX_SENDER_REG	(*(volatile uint32_t*)(IO_BASE | 0xb894))
#define MBOX_STATUS_REG	(*(volatile uint32_t*)(IO_BASE | 0xb898))
#define MBOX_CFG_REG	(*(volatile uint32_t*)(IO_BASE | 0xb89c))
#define MBOX_WRITE_REG	(*(volatile uint32_t*)(IO_BASE | 0xb8a0))

#define MBOX_STAT_WRBUSY	0x80000000
#define MBOX_STAT_RDBUSY	0x40000000

struct vc_fbinfo {
	uint32_t phys_width, phys_height;
	uint32_t virt_width, virt_height;
	uint32_t pitch;			/* filled by videocore */
	uint32_t depth;
	uint32_t x, y;
	void *addr;		/* filled by videocore */
	uint32_t size;	/* filled by videocore */
};

void mbox_write(int mbox, uint32_t msg);
uint32_t mbox_read(int mbox);

static struct vc_fbinfo fbinf __attribute__((aligned(16)));

int video_init(void)
{
	memset(&fbinf, 0, sizeof fbinf);
	fbinf.phys_width = fbinf.virt_width = 1024;
	fbinf.phys_height = fbinf.virt_height = 600;
	fbinf.depth = 32;
	fbinf.x = fbinf.y = 0;

	mbox_write(1, MEM_BUS_COHERENT(&fbinf));
	if(mbox_read(1) != 0) {
		ser_printstr("Failed to initialize display\n");
		return -1;
	}

	ser_printstr("Video init successful\n");
	memset(fbinf.addr, 0, fbinf.size);
	return 0;
}

void mbox_write(int mbox, uint32_t msg)
{
	while(MBOX_STATUS_REG & MBOX_STAT_WRBUSY);
	MBOX_WRITE_REG = (msg & 0xfffffff0) | mbox;
}

uint32_t mbox_read(int mbox)
{
	uint32_t msg;

	do {
		while(MBOX_STATUS_REG & MBOX_STAT_RDBUSY);
		msg = MBOX_READ_REG;
	} while((msg & 0xf) != mbox);

	return msg & 0xfffffff0;
}

#include <stdio.h>
#include <stdarg.h>
#include "rpi.h"
#include "sysctl.h"
#include "asm.h"
#include "serial.h"
#include "debug.h"

#define IOREG(offs)		(*(volatile uint32_t*)(rpi_iobase | offs))

/* System timer */
#define STM_CTL_REG		IOREG(0x3000)
#define STM_STAT_REG	STM_CTL_REG
#define STM_LCNT_REG	IOREG(0x3004)
#define STM_HCNT_REG	IOREG(0x3008)
#define STM_CMP0_REG	IOREG(0x300c)
#define STM_CMP1_REG	IOREG(0x3010)
#define STM_CMP2_REG	IOREG(0x3014)
#define STM_CMP3_REG	IOREG(0x3018)

#define STMCTL_M0		1
#define STMCTL_M1		2
#define STMCTL_M2		4
#define STMCTL_M3		8

/* TIMER */
#define TM_LOAD_REG		IOREG(0xb400)
#define TM_VALUE_REG	IOREG(0xb404)
#define TM_CTL_REG		IOREG(0xb408)
#define TM_ICLR_REG		IOREG(0xb40c)
#define TM_IRAW_REG		IOREG(0xb410)
#define TM_IMSK_REG		IOREG(0xb414)
#define TM_RELOAD_REG	IOREG(0xb418)
#define TM_PREDIV_REG	IOREG(0xb41c)
#define TM_COUNT_REG	IOREG(0xb420)

#define TMCTL_23BIT		0x000002
#define TMCTL_DIV16		0x000004
#define TMCTL_DIV256	0x000008
#define TMCTL_DIV1		0x00000c
#define TMCTL_IEN		0x000020
#define TMCTL_EN		0x000080
#define TMCTL_DBGHALT	0x000100
#define TMCTL_CNTEN		0x000200

#define TMCTL_PRESCALER(x)	(((uint32_t)(x) & 0xff) << 16)

/* watchdog */
#define PM_RSTC_REG		IOREG(0x10001c)
#define PM_WDOG_REG		IOREG(0x100024)

#define PM_PASSWD			0x5a000000
#define PMRSTC_WRCFG_FULL_RESET	0x00000020
#define PMRSTC_WRCFG_CLEAR		0xffffffcf

/* MAILBOX */
#define MBOX_READ_REG	IOREG(0xb880)
#define MBOX_POLL_REG	IOREG(0xb890)
#define MBOX_SENDER_REG	IOREG(0xb894)
#define MBOX_STATUS_REG	IOREG(0xb898)
#define MBOX_CFG_REG	IOREG(0xb89c)
#define MBOX_WRITE_REG	IOREG(0xb8a0)

/* the full bit is set when there's no space to append messages */
#define MBOX_STAT_FULL	0x80000000
/* the empty bit is set when there are no pending messages to be read */
#define MBOX_STAT_EMPTY	0x40000000

static int detect(void);

int rpi_model;
uint32_t rpi_iobase;
uint32_t rpi_mem_base, rpi_mem_size, rpi_vmem_base, rpi_vmem_size;


/* needs to by 16-byte aligned, because the address we send over the mailbox
 * interface, will have its 4 least significant bits masked off and taken over
 * by the mailbox id
 */
static char propbuf[256] __attribute__((aligned(16)));
static struct rpi_prop *wrprop, *rdprop;


void rpi_init(void)
{
	struct rpi_prop *prop;

	if((rpi_model = detect()) == -1) {
		for(;;) halt_cpu();
	}

	init_serial(115200);

	/* The model detected by detect is not accurate, get the correct board model
	 * through the mailbox property interface if possible.
	 * Also, detect the amount of CPU and GPU memory available.
	 */

	rpi_prop(RPI_TAG_GETREV);
	rpi_prop(RPI_TAG_GETRAM);
	rpi_prop(RPI_TAG_GETVRAM);
	if(rpi_prop_send() != -1) {
		//hexdump(propbuf, sizeof propbuf);

		while((prop = rpi_prop_next())) {
			switch(prop->id) {
			case RPI_TAG_GETREV:
				printf("board revision: %x\n", prop->data[0]);
				/* TODO: guess rpi model based on board revision */
				break;

			case RPI_TAG_GETRAM:
				rpi_mem_base = prop->data[0];
				rpi_mem_size = prop->data[1];
				break;

			case RPI_TAG_GETVRAM:
				rpi_vmem_base = prop->data[0];
				rpi_vmem_size = prop->data[1];
				break;

			default:
				break;
			}
		}
	}
}

static int detect(void)
{
	int i, j;
	uint32_t tm0, tm1;
	static uint32_t base[] = {0x20000000, 0x3f000000, 0xfe000000};

	for(i=0; i<3; i++) {
		rpi_iobase = base[i];
		tm0 = STM_LCNT_REG;
		for(j=0; j<256; j++) {
			tm1 = STM_LCNT_REG;
		}
		if(tm0 != tm1) {
			return i + 1;
		}
	}
	return -1;
}

void rpi_reboot(void)
{
	mem_barrier();
	PM_WDOG_REG = PM_PASSWD | 1;
	PM_RSTC_REG = PM_PASSWD | (PM_RSTC_REG & PMRSTC_WRCFG_CLEAR) | PMRSTC_WRCFG_FULL_RESET;
	for(;;) halt_cpu();
}

void rpi_mbox_send(int chan, uint32_t msg)
{
	mem_barrier();
	while(MBOX_STATUS_REG & MBOX_STAT_FULL);
	MBOX_WRITE_REG = (msg & 0xfffffff0) | chan;
	mem_barrier();
}

uint32_t rpi_mbox_recv(int chan)
{
	uint32_t msg;
	mem_barrier();
	do {
		while(MBOX_STATUS_REG & MBOX_STAT_EMPTY);
		msg = MBOX_READ_REG;
	} while((msg & 0xf) != chan);
	mem_barrier();
	return msg & 0xfffffff0;
}

int rpi_mbox_pending(int chan)
{
	mem_barrier();
	return (MBOX_STATUS_REG & MBOX_STAT_EMPTY) == 0;
}


static struct {
	int id, req_len, resp_len;
} taginfo[] = {
	{RPI_TAG_GETMODEL, 0, 4},
	{RPI_TAG_GETREV, 0, 4},
	{RPI_TAG_GETRAM, 0, 8},
	{RPI_TAG_GETVRAM, 0, 8},
	{RPI_TAG_SETCLOCK, 4, 8},
	{RPI_TAG_ALLOCFB, 4, 8},
	{RPI_TAG_RELEASEFB, 0, 0},
	{RPI_TAG_BLANKSCR, 4, 4},
	{RPI_TAG_SETFBPHYS, 8, 8},
	{RPI_TAG_SETFBVIRT, 8, 8},
	{RPI_TAG_SETFBDEPTH, 4, 4},
	{RPI_TAG_GETFBPITCH, 0, 4},
	{RPI_TAG_SETFBOFFS, 8, 8},
	{RPI_TAG_GETFBOFFS, 0, 8},
	{0, 0, 0}
};

void rpi_prop(int id, ...)
{
	int i, req_len = 0, resp_len = 0;
	va_list ap;

	if(!wrprop) {
		wrprop = (struct rpi_prop*)(propbuf + sizeof(struct rpi_prop_header));
	}

	for(i=0; taginfo[i].id; i++) {
		if(taginfo[i].id == id) {
			req_len = taginfo[i].req_len;
			resp_len = taginfo[i].resp_len;
			break;
		}
	}

	wrprop->id = id;
	wrprop->size = resp_len;
	wrprop->res = 0;

	va_start(ap, id);
	for(i=0; i<resp_len >> 2; i++) {
		if(i < req_len >> 2) {
			wrprop->data[i] = va_arg(ap, uint32_t);
		} else {
			wrprop->data[i] = 0;
		}
	}
	va_end(ap);

	wrprop = RPI_PROP_NEXT(wrprop);
}

int rpi_prop_send(void)
{
	struct rpi_prop_header *hdr = (struct rpi_prop_header*)propbuf;
	uint32_t addr = (uint32_t)propbuf;
	uint32_t size;

	/* terminate with null tag */
	wrprop->id = 0;

	size = (char*)wrprop - propbuf + 4;

	hdr->size = (size + 15) & 0xfffffff0;
	hdr->res = 0;

	wrprop = rdprop = 0;

	/* clean and invalidate cache, otherwise VC will see stale data */
	sysctl_dcache_clean_inval(addr, hdr->size);

	rpi_mbox_send(RPI_MBOX_PROP, addr);
	while(rpi_mbox_recv(RPI_MBOX_PROP) != addr);

	sysctl_dcache_clean_inval(addr, hdr->size);

	if(hdr->res != 0x80000000) {
		printf("Property request failed: %x\n", hdr->res);
		return -1;
	}
	return 0;
}

struct rpi_prop *rpi_prop_next(void)
{
	struct rpi_prop *res;
	if(!rdprop) {
		rdprop = (struct rpi_prop*)(propbuf + sizeof(struct rpi_prop_header));
	}

	if(rdprop->id == 0) {
		res = rdprop = 0;
	} else {
		res = rdprop;
		rdprop->size = rdprop->res & 0x7fffffff;
		rdprop = RPI_PROP_NEXT(rdprop);
	}
	return res;
}

struct rpi_prop *rpi_prop_find(int id)
{
	struct rpi_prop *prop = (struct rpi_prop*)(propbuf + sizeof(struct rpi_prop_header));

	while(prop->id) {
		prop->size = prop->res & 0x7fffffff;
		if(prop->id == id) {
			return prop;
		}
		prop = RPI_PROP_NEXT(prop);
	}
	return 0;
}
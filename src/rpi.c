#include "rpi.h"
#include "asm.h"

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
uint32_t rpi_memsize, rpi_vc_memsize;

void rpi_init(void)
{
	if((rpi_model = detect()) == -1) {
		for(;;) halt_cpu();
	}

	/* TODO */
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


void rpi_mbox_send(int chan, uint32_t msg)
{
	while(MBOX_STATUS_REG & MBOX_STAT_FULL);
	MBOX_WRITE_REG = (msg & 0xfffffff0) | chan;
}

uint32_t rpi_mbox_recv(int chan)
{
	uint32_t msg;
	do {
		while(MBOX_STATUS_REG & MBOX_STAT_EMPTY);
		msg = MBOX_READ_REG;
	} while((msg & 0xf) != chan);
	return msg & 0xfffffff0;
}

int rpi_mbox_pending(int chan)
{
	return (MBOX_STATUS_REG & MBOX_STAT_EMPTY) == 0;
}

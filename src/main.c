/* mailbox registers (MB 0: input  1: output)
 * function | MB 0 | MB 1
 *  rd/wr   | 00   | 20      ( upper 28: data, lower 4: channel )
 *  peek    | 10   | 30
 *  sender  | 14   | 34
 *  status  | 18   | 38
 *  config  | 1c   | 3c
 *
 * channel 1: framebuffer
 * channel 8: request
 *
 * read: read status reg loop while empty flag is set
 * write: read status loop while full flag is set
 */
#include <string.h>
#include <stdint.h>

#ifdef RPI1
#define IOBASEADDR	0x20000000
#else
#define IOBASEADDR	0x3f000000
#endif

#define phys2bus(addr)	((addr) | 0x40000000)
#define bus2phys(addr)	((addr) & 0x3fffffff)

#define IOREG_ADDR(x)	(IOBASEADDR | (x))
#define REG_MB_READ		*((volatile uint32_t*)IOREG_ADDR(0xb880))
#define REG_MB_STAT		*((volatile uint32_t*)IOREG_ADDR(0xb898))
#define REG_MB_WRITE	*((volatile uint32_t*)IOREG_ADDR(0xb8a0))

#define MB_STAT_FULL	0x80000000
#define MB_STAT_EMPTY	0x40000000

#define MB_CHAN_FRAMEBUF	1
#define MB_CHAN_PROP		8

#define PROP_CODE_REQ	0
#define PROP_RESP_OK	0x80000000

#define PROP_TAG_END		0
#define PROP_TAG_BLANKSCR	0x40002

int prop_blankscr(int onoff);

uint32_t mb_read(int chan);
void mb_write(int chan, uint32_t val);

int main(void)
{
	prop_blankscr(1);

	return 0;
}

static uint32_t propbuf[64] __attribute__((aligned(16)));

int prop_blankscr(int onoff)
{
	uint32_t *pb = propbuf;

	*pb++ = 0;
	*pb++ = 0;
	*pb++ = PROP_TAG_BLANKSCR;
	*pb++ = 4;	/* data size */
	*pb++ = PROP_CODE_REQ;
	*pb++ = onoff ? 1 : 0;
	*pb++ = PROP_TAG_END;
	*pb++ = 0;	/* padding */
	propbuf[0] = (char*)pb - (char*)propbuf;

	mb_write(MB_CHAN_PROP, (uint32_t)propbuf >> 4);
	mb_read(MB_CHAN_PROP);

	return propbuf[1] == PROP_RESP_OK ? 0 : -1;
}

uint32_t mb_read(int chan)
{
	uint32_t val;
	do {
		while(REG_MB_STAT & MB_STAT_EMPTY);
		val = REG_MB_READ;
	} while((val & 0xf) != chan);
	return val >> 4;
}

void mb_write(int chan, uint32_t val)
{
	while(REG_MB_STAT & MB_STAT_FULL);
	REG_MB_WRITE = (val << 4) | chan;
}

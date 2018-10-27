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


#define PROP_TAG_END			0

#define PROP_TAG_SET			0x08000
#define PROP_TAG_TEST			0x04000
#define PROP_TAG_GET			0

#define PROP_TAG_ALLOCBUF		0x40001
#define PROP_TAG_BLANKSCR		0x40002
#define PROP_TAG_PHYSRES		0x40003
#define PROP_TAG_VIRTRES		0x40004
#define PROP_TAG_DEPTH			0x40005
#define PROP_TAG_PIXEL_ORDER	0x40006
#define PROP_TAG_ALPHA_MODE		0x40007
#define PROP_TAG_PITCH			0x40008
#define PROP_TAG_VOFFS			0x40009
#define PROP_TAG_OVERSCAN		0x4000a
#define PROP_TAG_PALETTE		0x4000b
#define PROP_TAG_CUR_INFO		0x00010
#define PROP_TAG_CUR_STATE		0x00011


void panic(void);
int prop_blankscr(int onoff);
int prop_setres(int xsz, int ysz);
int prop_getres(int *xsz, int *ysz);
int prop_setvres(int xsz, int ysz);
int prop_getvres(int *xsz, int *ysz);
int prop_setdepth(int bpp);
int prop_getdepth(void);
int prop_getpitch(void);
void *prop_allocbuf(int *size);

uint32_t mb_read(int chan);
void mb_write(int chan, uint32_t val);

void dbgled(int x);
void exit(int x);

int main(void)
{
	int i, j, bpp, fbsize, pitch;
	uint16_t *fb;

	if(prop_setres(640, 480) == -1) panic();
	if(prop_setvres(640, 480) == -1) panic();
	if(prop_setdepth(16) == -1) panic();
	if(!(fb = prop_allocbuf(&fbsize))) panic();
	bpp = prop_getdepth();
	pitch = prop_getpitch();

	if(bpp != 16) panic();

	for(i=0; i<480; i++) {
		for(j=0; j<640; j++) {
			int xor = i ^ j;
			int r = (xor >> 1) & 0xff;
			int g = xor & 0xff;
			int b = (xor << 1) & 0xff;

			*fb++ = ((r << 8) & 0xf800) | ((g << 3) & 0x7e0) | ((b >> 3) & 0x1f);
		}

		//fb += pitch / 2 - 640;
	}

	return 0;
}

void panic(void)
{
	dbgled(2);
	exit(0);
}

static uint32_t propbuf[64] __attribute__((aligned(16)));

static int send_prop(uint32_t *buf)
{
	mb_write(MB_CHAN_PROP, (uint32_t)buf >> 4);
	mb_read(MB_CHAN_PROP);
	return propbuf[1] == PROP_RESP_OK ? 0 : -1;
}

int prop_blankscr(int onoff)
{
	uint32_t *pb = propbuf;

	*pb++ = 32;
	*pb++ = 0;
	*pb++ = PROP_TAG_BLANKSCR;
	*pb++ = 4;	/* data size */
	*pb++ = PROP_CODE_REQ;
	*pb++ = onoff ? 1 : 0;
	*pb++ = PROP_TAG_END;
	*pb++ = 0;	/* padding */

	return send_prop(propbuf);
}

int prop_setres(int xsz, int ysz)
{
	uint32_t *pb = propbuf;

	*pb++ = 32;
	*pb++ = 0;
	*pb++ = PROP_TAG_PHYSRES | PROP_TAG_SET;
	*pb++ = 8;	/* data size */
	*pb++ = PROP_CODE_REQ;
	*pb++ = xsz;
	*pb++ = ysz;
	*pb++ = PROP_TAG_END;

	return send_prop(propbuf);
}

int prop_getres(int *xsz, int *ysz)
{
	uint32_t *pb = propbuf;
	uint32_t *data;

	*pb++ = 32;
	*pb++ = 0;
	*pb++ = PROP_TAG_PHYSRES;
	*pb++ = 8;
	*pb++ = PROP_CODE_REQ;
	data = pb;
	*pb++ = 0;
	*pb++ = 0;
	*pb++ = PROP_TAG_END;

	if(send_prop(propbuf) == -1) {
		return -1;
	}
	*xsz = data[0];
	*ysz = data[1];
	return 0;
}

int prop_setvres(int xsz, int ysz)
{
	uint32_t *pb = propbuf;

	*pb++ = 32;
	*pb++ = 0;
	*pb++ = PROP_TAG_VIRTRES | PROP_TAG_SET;
	*pb++ = 8;	/* data size */
	*pb++ = PROP_CODE_REQ;
	*pb++ = xsz;
	*pb++ = ysz;
	*pb++ = PROP_TAG_END;

	return send_prop(propbuf);
}

int prop_getvres(int *xsz, int *ysz)
{
	uint32_t *pb = propbuf;
	uint32_t *data;

	*pb++ = 32;
	*pb++ = 0;
	*pb++ = PROP_TAG_VIRTRES;
	*pb++ = 8;
	*pb++ = PROP_CODE_REQ;
	data = pb;
	*pb++ = 0;
	*pb++ = 0;
	*pb++ = PROP_TAG_END;

	if(send_prop(propbuf) == -1) {
		return -1;
	}
	*xsz = data[0];
	*ysz = data[1];
	return 0;
}

int prop_setdepth(int bpp)
{
	uint32_t *pb = propbuf;

	*pb++ = 32;
	*pb++ = 0;
	*pb++ = PROP_TAG_DEPTH | PROP_TAG_SET;
	*pb++ = 4;
	*pb++ = PROP_CODE_REQ;
	*pb++ = bpp;
	*pb++ = PROP_TAG_END;
	*pb++ = 0;	/* padding */

	return send_prop(propbuf);
}

int prop_getdepth(void)
{
	uint32_t *pb = propbuf;
	uint32_t *data;

	*pb++ = 32;
	*pb++ = 0;
	*pb++ = PROP_TAG_DEPTH;
	*pb++ = 4;	/* data size */
	*pb++ = PROP_CODE_REQ;
	data = pb;
	*pb++ = 0;
	*pb++ = PROP_TAG_END;
	*pb++ = 0;	/* padding */

	if(send_prop(propbuf) == -1) {
		return 0;
	}
	return *data;
}

int prop_getpitch(void)
{
	uint32_t *pb = propbuf;
	uint32_t *data;

	*pb++ = 32;
	*pb++ = 0;
	*pb++ = PROP_TAG_PITCH;
	*pb++ = 4;	/* data size */
	*pb++ = PROP_CODE_REQ;
	data = pb;
	*pb++ = 0;
	*pb++ = PROP_TAG_END;
	*pb++ = 0;	/* padding */

	if(send_prop(propbuf) == -1) {
		return 0;
	}
	return *data;
}

void *prop_allocbuf(int *size)
{
	uint32_t *pb = propbuf;
	uint32_t *data;

	*pb++ = 32;
	*pb++ = 0;
	*pb++ = PROP_TAG_ALLOCBUF;
	*pb++ = 8;	/* data size */
	*pb++ = PROP_CODE_REQ;
	data = pb;
	*pb++ = 16;	/* alignment */
	*pb++ = 0;
	*pb++ = PROP_TAG_END;

	if(send_prop(propbuf) == -1) {
		return 0;
	}
	*size = data[1];
	return (void*)bus2phys(data[0]);
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

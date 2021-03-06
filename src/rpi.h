#ifndef RPI_H_
#define RPI_H_

#include <stdint.h>


#define RPI_MEM_BUS_COHERENT(addr)	(((uint32_t)addr) | 0x40000000)
#define RPI_MEM_BUS_UNCACHED(addr)	(((uint32_t)addr) | 0xc0000000)


#define RPI_MBOX_FRAMEBUF	1
#define RPI_MBOX_PROP		8

#define RPI_TAG_GETMODEL	0x010001
#define RPI_TAG_GETREV		0x010002
#define RPI_TAG_GETRAM		0x010005
#define RPI_TAG_GETVRAM		0x010006

#define RPI_TAG_GETPOWER	0x020001
#define RPI_TAG_SETPOWER	0x028001

#define RPI_TAG_GETCLKRATE	0x030002
#define RPI_TAG_SETCLKRATE	0x038002
#define RPI_TAG_GETMAXRATE	0x030004
#define RPI_TAG_GETEDID		0x030020

#define RPI_TAG_ALLOCFB		0x040001
#define RPI_TAG_RELEASEFB	0x048001
#define RPI_TAG_BLANKSCR	0x040002
#define RPI_TAG_SETFBPHYS	0x048003
#define RPI_TAG_SETFBVIRT	0x048004
#define RPI_TAG_SETFBDEPTH	0x048005
#define RPI_TAG_GETFBPITCH	0x040008
#define RPI_TAG_SETFBOFFS	0x048009
#define RPI_TAG_GETFBOFFS	0x040009
/* NOTE: for every new tag added, a new entry needs to be added to the tag
 * buffer size struct in rpi.c
 */

#define RPI_CLK_UART	0x02
#define RPI_CLK_ARM		0x03

enum {
	RPI_POWER_SD,
	RPI_POWER_UART0, RPI_POWER_UART1,
	RPI_POWER_USBHCD,
	RPI_POWER_I2C0, RPI_POWER_I2C1, RPI_POWER_I2C2,
	RPI_POWER_SPI,
	RPI_POWER_CCP2TX
};

struct rpi_prop_header {
	uint32_t size;
	uint32_t res;
};

struct rpi_prop {
	uint32_t id, size, res;
	uint32_t data[1];
};

#define RPI_PROP_NEXT(p) \
	((struct rpi_prop*)((char*)((p) + 1) + ((p)->size - sizeof (p)->data)))

extern int rpi_model;
extern uint32_t rpi_iobase;
extern uint32_t rpi_mem_base, rpi_mem_size, rpi_vmem_base, rpi_vmem_size;
extern uint32_t rpi_clk_cur, rpi_clk_max, rpi_clk_uart;

void rpi_init(void);
void rpi_reboot(void) __attribute__((noreturn));

void rpi_mbox_send(int chan, uint32_t msg);
uint32_t rpi_mbox_recv(int chan);
int rpi_mbox_pending(int chan);

/* usage:
 * rpi_prop(RPI_TAG_WHATEVER, foo, bar);
 * rpi_prop(RPI_TAG_XYZZY, 42);
 * if(rpi_prop_send() != -1) {
 *     struct rpi_prop *prop;
 *     while((prop = rpi_prop_next())) {
 *         ... process response tags ...
 *     }
 * }
 */
void rpi_prop(int id, ...);
int rpi_prop_send(void);
struct rpi_prop *rpi_prop_next(void);
/* find specific tag in response */
struct rpi_prop *rpi_prop_find(int id);

#endif	/* RPI_H_ */

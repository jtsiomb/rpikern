#ifndef RPI_H_
#define RPI_H_

#include <stdint.h>


#define RPI_MEM_BUS_COHERENT(addr)	(((uint32_t)addr) | 0x40000000)
#define RPI_MEM_BUS_UNCACHED(addr)	(((uint32_t)addr) | 0xc0000000)


#define RPI_MBOX_FRAMEBUF	1
#define RPI_MBOX_PROP		8

#define RPI_TAG_GETMODEL	0x010001
#define RPI_TAG_GETRAM		0x010005
#define RPI_TAG_GETVRAM		0x010006
#define RPI_TAG_SETCLOCK	0x038002

#define RPI_TAG_ALLOCFB		0x040001
#define RPI_TAG_RELEASEFB	0x048001
#define RPI_TAG_BLANKSCR	0x040002
#define RPI_TAG_SETFBPHYS	0x048003
#define RPI_TAG_SETFBVIRT	0x048004
#define RPI_TAG_SETFBDEPTH	0x048005

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
extern uint32_t rpi_memsize, rpi_vc_memsize;

void rpi_init(void);
void rpi_reboot(void) __attribute__((noreturn));

void rpi_mbox_send(int chan, uint32_t msg);
uint32_t rpi_mbox_recv(int chan);
int rpi_mbox_pending(int chan);

#endif	/* RPI_H_ */

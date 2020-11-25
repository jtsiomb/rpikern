#ifndef GPIO_H_
#define GPIO_H_

#include "rpi.h"
#include "asm.h"

#define GPIO_BASE	(rpi_iobase | 0x200000)
#define GPIO_REG(x)	(*(volatile uint32_t*)(GPIO_BASE | (x)))

/* function select */
#define REG_GPFSEL(x)	GPIO_REG((x) << 2)
#define REG_GPFSEL0		GPIO_REG(0x00)
#define REG_GPFSEL1		GPIO_REG(0x04)
#define REG_GPFSEL2		GPIO_REG(0x08)
#define REG_GPFSEL3		GPIO_REG(0x0c)
#define REG_GPFSEL4		GPIO_REG(0x10)
#define REG_GPFSEL5		GPIO_REG(0x14)
/* pin output set */
#define REG_GPSET0		GPIO_REG(0x1c)
#define REG_GPSET1		GPIO_REG(0x20)
/* pin output clear */
#define REG_GPCLR0		GPIO_REG(0x28)
#define REG_GPCLR1		GPIO_REG(0x2c)
/* pin level */
#define REG_GPLEV0		GPIO_REG(0x34)
#define REG_GPLEV1		GPIO_REG(0x38)
/* pin event detect status */
#define REG_GPEDS0		GPIO_REG(0x40)
#define REG_GPEDS1		GPIO_REG(0x44)
/* pin rising edge detect enable */
#define REG_GPREN0		GPIO_REG(0x4c)
#define REG_GPREN1		GPIO_REG(0x50)
/* pin falling edge detect enable */
#define REG_GPFEN0		GPIO_REG(0x58)
#define REG_GPFEN1		GPIO_REG(0x5c)
/* pin high detect enable */
#define REG_GPHEN0		GPIO_REG(0x64)
#define REG_GPHEN1		GPIO_REG(0x68)
/* pin low detect enable */
#define REG_GPLEN0		GPIO_REG(0x70)
#define REG_GPLEN1		GPIO_REG(0x74)
/* pin async rising edge detect */
#define REG_GPAREN0		GPIO_REG(0x7c)
#define REG_GPAREN1		GPIO_REG(0x80)
/* pin async falling edge detect */
#define REG_GPAFEN0		GPIO_REG(0x88)
#define REG_GPAFEN1		GPIO_REG(0x8c)
/* pin pull-up/down enable */
#define REG_GPPUD		GPIO_REG(0x94)
/* pin pull-up/down enable clock */
#define REG_GPPUDCLK0	GPIO_REG(0x98)
#define REG_GPPUDCLK1	GPIO_REG(0x9c)

/* function select bits */
#define FSEL_IN		0
#define FSEL_OUT	1
#define FSEL_ALT0	4
#define FSEL_ALT1	5
#define FSEL_ALT2	6
#define FSEL_ALT3	7
#define FSEL_ALT4	3
#define FSEL_ALT5	2

/* pull-up/down bits */
#define PUD_DISABLE	0
#define PUD_DOWN	1
#define PUD_UP		2

static inline void gpio_fsel(int x, int f)
{
	static const unsigned int fseltab[54][2] = {
		{0, 0}, {0, 3}, {0, 6}, {0, 9}, {0, 12}, {0, 15}, {0, 18}, {0, 21}, {0, 24}, {0, 27},
		{4, 0}, {4, 3}, {4, 6}, {4, 9}, {4, 12}, {4, 15}, {4, 18}, {4, 21}, {4, 24}, {4, 27},
		{8, 0}, {8, 3}, {8, 6}, {8, 9}, {8, 12}, {8, 15}, {8, 18}, {8, 21}, {8, 24}, {8, 27},
		{12, 0}, {12, 3}, {12, 6}, {12, 9}, {12, 12}, {12, 15}, {12, 18}, {12, 21}, {12, 24}, {12, 27},
		{16, 0}, {16, 3}, {16, 6}, {16, 9}, {16, 12}, {16, 15}, {16, 18}, {16, 21}, {16, 24}, {16, 27},
		{20, 0}, {20, 3}, {20, 6}, {20, 9}
	};
	uint32_t val;

	if(x <= 53) {
		val = f << fseltab[x][1];
		*(volatile uint32_t*)(GPIO_BASE | fseltab[x][0]) = val;
	}
}

static inline void gpio_pullups(uint32_t mask0, uint32_t mask1, int state)
{
	REG_GPPUD = state;
	delay(150);

	if(mask0) REG_GPPUDCLK0 = mask0;
	if(mask1) REG_GPPUDCLK1 = mask1;
	delay(150);

	if(mask0) REG_GPPUDCLK0 = 0;
	if(mask1) REG_GPPUDCLK1 = 0;
}

#endif	/* GPIO_H_ */

#ifndef RPI_IOREG_H_
#define RPI_IOREG_H_

#include "rpi.h"

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

/* IRQ */
#define IRQ_PENDING0_REG	IOREG(0xb200)
#define IRQ_PENDING1_REG	IOREG(0xb204)
#define IRQ_PENDING2_REG	IOREG(0xb208)
#define IRQ_FIQCTL_REG		IOREG(0xb20c)
#define IRQ_ENABLE1_REG		IOREG(0xb210)
#define IRQ_ENABLE2_REG		IOREG(0xb214)
#define IRQ_ENABLE0_REG		IOREG(0xb218)
#define IRQ_DISABLE1_REG	IOREG(0xb21c)
#define IRQ_DISABLE2_REG	IOREG(0xb220)
#define IRQ_DISABLE0_REG	IOREG(0xb224)

#define IRQ_TIMER	0
#define IRQ_MBOX	1
#define IRQ_DOORB0	2
#define IRQ_DOORB1	3
#define IRQ_GPU0HLT	4
#define IRQ_GPU1HLT	5
#define IRQ_ILL1	6
#define IRQ_ILL0	7

#define IRQ_GPU_TIMER0		0
#define IRQ_GPU_TIMER1		1
#define IRQ_GPU_TIMER2		2
#define IRQ_GPU_TIMER3		3
#define IRQ_GPU_AUX			29
#define IRQ_GPU_I2C_SPI_SLV	43
#define IRQ_GPU_PWA0		45
#define IRQ_GPU_PWA1		46
#define IRQ_GPU_SMI			48
#define IRQ_GPU_GPIO0		49
#define IRQ_GPU_GPIO1		50
#define IRQ_GPU_GPIO2		51
#define IRQ_GPU_GPIO3		52
#define IRQ_GPU_I2C			53
#define IRQ_GPU_SPI			54
#define IRQ_GPU_PCM			55
#define IRQ_GPU_UART		57

/* IRQ_PENDING0_REG flags (pending basic interrupts) */
#define IRQ_PEND0_TIMER		0x0000001
#define IRQ_PEND0_MBOX		0x0000002
#define IRQ_PEND0_DOORB0	0x0000004
#define IRQ_PEND0_DOORB1	0x0000008
#define IRQ_PEND0_GPU0HLT	0x0000010
#define IRQ_PEND0_GPU1HLT	0x0000020
#define IRQ_PEND0_ILL1		0x0000040
#define IRQ_PEND0_ILL0		0x0000080
#define IRQ_PEND0_PEND2		0x0000100
#define IRQ_PEND0_PEND1		0x0000200
#define IRQ_PEND0_GPU_IRQ7	0x0000400
#define IRQ_PEND0_GPU_IRQ9	0x0000800
#define IRQ_PEND0_GPU_IRQ10	0x0001000
#define IRQ_PEND0_GPU_IRQ18	0x0002000
#define IRQ_PEND0_GPU_IRQ19	0x0004000
#define IRQ_PEND0_GPU_I2C	0x0008000
#define IRQ_PEND0_GPU_SPI	0x0010000
#define IRQ_PEND0_GPU_PCM	0x0020000
#define IRQ_PEND0_GPU_IRQ56	0x0040000
#define IRQ_PEND0_GPU_UART	0x0080000
#define IRQ_PEND0_GPU_IRQ62	0x0100000

/* IRQ_FIQCTL_REG */
#define IRQ_FIQCTL_SELGPU(x)	(x)
#define IRQ_FIQCTL_SELARM(x)	((x) + 64)
#define IRQ_FIQCTL_ENABLE		0x80

#endif	/* RPI_IOREG_H_ */

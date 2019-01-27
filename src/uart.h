#ifndef UART_PL011_H_
#define UART_PL011_H_

#include "config.h"

#define UART0_BASE		(IO_BASE | 0x201000)
#define UART0_REG(x)	*(volatile uint32_t*)(UART0_BASE | (x))

#define REG_DR		UART0_REG(0x00)	/* data register */
#define REG_RSRECR	UART0_REG(0x04)	/* receive status & error clear */
#define REG_FR		UART0_REG(0x18)	/* flag register */
#define REG_IBRD	UART0_REG(0x24)	/* integer baud rate divisor (low 16 bits) */
#define REG_FBRD	UART0_REG(0x28)	/* fractional baud rate divisor (low 6 (six) bits) */
#define REG_LCRH	UART0_REG(0x2c)	/* line control */
#define REG_CR		UART0_REG(0x30)	/* control register */
#define REG_IFLS	UART0_REG(0x34)	/* interrupt FIFO level select */
#define REG_IMSC	UART0_REG(0x38)	/* interrupt mask set clear */
#define REG_RIS		UART0_REG(0x3c)	/* raw interrupt status */
#define REG_MIS		UART0_REG(0x40)	/* masked interrupt status */
#define REG_ICR		UART0_REG(0x44)	/* interrupt clear */

/* error bits in REG_DR */
#define DR_FRM	0x0100
#define DR_PAR	0x0200
#define DR_BRK	0x0400
#define DR_OVR	0x0800

/* receive status error bits */
#define RCR_FRM	0x01
#define RCR_PAR	0x02
#define RCR_BRK	0x04
#define RCR_OVR	0x08

/* flag register bits */
#define FR_CTS	0x01
#define FR_BUSY	0x08
#define FR_RXFE	0x10	/* receive FIFO empty */
#define FR_TXFF	0x20	/* transmit FIFO full */
#define FR_RXFF	0x40	/* receive FIFO full */
#define FR_TXFE	0x80	/* transmit FIFO empty */

/* line control register bits */
#define LCRH_BRK		0x01	/* send break */
#define LCRH_PAREN		0x02
#define LCRH_PAREVEN	0x04
#define LCRH_STOP2		0x08
#define LCRH_FIFOEN		0x10
#define LCRH_8BITS		0x60
#define LCRH_7BITS		0x40
#define LCRH_STICKPAR	0x80	/* ? */

/* control register bits */
#define CR_UARTEN		0x0001
#define CR_LOOPEN		0x0080
#define CR_TXEN			0x0100
#define CR_RXEN			0x0200
#define CR_RTS			0x0800
#define CR_RTSEN		0x4000
#define CR_CTSEN		0x8000

/* interrupt bits for IMSC, RIS, ICR */
#define I_CTS		0x0002
#define I_RX		0x0010
#define I_TX		0x0020
#define I_RTIME		0x0040
#define I_FRM		0x0080
#define I_PAR		0x0100
#define I_BRK		0x0200
#define I_OVR		0x0400

#endif	/* UART_PL011_H_ */

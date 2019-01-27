#ifndef TIMER_H_
#define TIMER_H_

#include "config.h"

#define TM_BASE	(IO_BASE | 0xb000)

#define TM_REG(x)	(*(volatile uint32_t*)(TIMER_BASE | (x)))
#define TM_LOAD		TM_REG(0x400)
#define TM_VALUE	TM_REG(0x404)
#define TM_CTL		TM_REG(0x408)
#define TM_ICLR		TM_REG(0x40c)
#define TM_IRAW		TM_REG(0x410)
#define TM_IMSK		TM_REG(0x414)
#define TM_RELOAD	TM_REG(0x418)
#define TM_PREDIV	TM_REG(0x41c)
#define TM_COUNT	TM_REG(0x420)

#define TMCTL_23BIT		0x000002
#define TMCTL_DIV16		0x000004
#define TMCTL_DIV256	0x000008
#define TMCTL_DIV1		0x00000c
#define TMCTL_IEN		0x000020
#define TMCTL_EN		0x000080
#define TMCTL_DBGHALT	0x000100
#define TMCTL_CNTEN		0x000200

#define TMCTL_PRESCALER(x)	(((uint32_t)(x) & 0xff) << 16)

#endif	/* TIMER_H_ */

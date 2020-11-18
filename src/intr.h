#ifndef INTR_H_
#define INTR_H_

#include <stdint.h>

enum {
	INTR_RESET		= 0,
	INTR_UNDEF		= 1,
	INTR_SWI		= 2,
	INTR_IABORT		= 3,
	INTR_DABORT		= 4,
	INTR_IRQ		= 6,
	INTR_FIQ		= 7
};

void intr_init(void);
void setvect(int idx, uint32_t addr);

void enable_arm_irq(int irq);
void disable_arm_irq(int irq);

void enable_gpu_irq(int irq);
void disable_gpu_irq(int irq);

#endif	/* INTR_H_ */

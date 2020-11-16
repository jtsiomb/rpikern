#ifndef INTR_H_
#define INTR_H_

#include <stdint.h>

enum {
	INTR_RESET,
	INTR_UNDEF,
	INTR_SWI,
	INTR_IABORT,
	INTR_DABORT,
	INTR_IRQ,
	INTR_FIQ
};

void intr_init(void);
void setvect(int idx, uint32_t addr);

void enable_arm_irq(int irq);
void disable_arm_irq(int irq);

void enable_gpu_irq(int irq);
void disable_gpu_irq(int irq);

#endif	/* INTR_H_ */

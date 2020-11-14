#ifndef INTR_H_
#define INTR_H_

void enable_arm_irq(int irq);
void disable_arm_irq(int irq);

void enable_gpu_irq(int irq);
void disable_gpu_irq(int irq);

#endif	/* INTR_H_ */

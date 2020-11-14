#include "rpi.h"
#include "rpi_ioreg.h"
#include "asm.h"

void enable_arm_irq(int irq)
{
	mem_barrier();
	if(irq == IRQ_TIMER) {
		IRQ_FIQCTL_REG = IRQ_FIQCTL_SELARM(IRQ_TIMER) | IRQ_FIQCTL_ENABLE;
	} else {
		IRQ_ENABLE0_REG |= 1 << irq;
	}
	mem_barrier();
}

void disable_arm_irq(int irq)
{
	mem_barrier();
	if(irq == IRQ_TIMER) {
		IRQ_FIQCTL_REG = 0;
	} else {
		IRQ_ENABLE0_REG &= ~(1 << irq);
	}
	mem_barrier();
}

void enable_gpu_irq(int irq)
{
	mem_barrier();
	if(irq == IRQ_GPU_TIMER1) {
		IRQ_FIQCTL_REG = IRQ_FIQCTL_SELGPU(IRQ_GPU_TIMER1) | IRQ_FIQCTL_ENABLE;
	} else if(irq < 32) {
		IRQ_ENABLE1_REG |= 1 << irq;
	} else {
		IRQ_ENABLE2_REG |= 1 << (irq - 32);
	}
	mem_barrier();
}

void disable_gpu_irq(int irq)
{
	mem_barrier();
	if(irq == IRQ_GPU_TIMER1) {
		IRQ_FIQCTL_REG = 0;
	} else if(irq < 32) {
		IRQ_ENABLE1_REG &= ~(1 << irq);
	} else {
		IRQ_ENABLE2_REG &= ~(1 << (irq - 32));
	}
	mem_barrier();
}

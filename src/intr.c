#include <stdio.h>
#include "intr.h"
#include "rpi.h"
#include "rpi_ioreg.h"
#include "asm.h"
#include "sysctl.h"
#include "debug.h"

void startup();
void intr_entry_nop();
void intr_entry_irq();
void intr_entry_fiq();

void intr_init(void)
{
	/* setup interrupt vectors */
	setvect(INTR_RESET, (uint32_t)startup);
	setvect(INTR_UNDEF, (uint32_t)intr_entry_nop);
	setvect(INTR_SWI, (uint32_t)intr_entry_nop);
	setvect(INTR_IABORT, (uint32_t)intr_entry_nop);
	setvect(INTR_DABORT, (uint32_t)intr_entry_nop);
	setvect(INTR_IRQ, (uint32_t)intr_entry_irq);
	setvect(INTR_FIQ, (uint32_t)intr_entry_fiq);
}

void setvect(int idx, uint32_t addr)
{
	uint32_t *ivec = 0;
	uint32_t pc = (uint32_t)(ivec + idx + 2);

	/* construct branch instruction */
	ivec[idx] = 0xea000000 | (((addr - pc) >> 2) & 0xffffff);

	/* we also probably need to invalidate the instr. cache */
	sysctl_icache_inval(0, 32);
}

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

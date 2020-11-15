#include "timer.h"
#include "rpi_ioreg.h"
#include "intr.h"

volatile unsigned long num_ticks;

void timer_init(void)
{
	/* clear all 4 timer interrupt bits */
	STM_CTL_REG = 0xf;
	/* setup first timer interrupt */
	STM_CMP1_REG = STM_LCNT_REG + 4000;

	enable_gpu_irq(IRQ_GPU_TIMER1);
}

#include "timer.h"
#include "rpi_ioreg.h"
#include "intr.h"

volatile unsigned long num_ticks;

void timer_init(void)
{
	enable_gpu_irq(IRQ_GPU_TIMER1);
}

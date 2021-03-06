#include <stdint.h>
#include "serial.h"
#include "uart.h"
#include "gpio.h"
#include "rpi.h"

/* baud rate: BAUDDIV = (UART_CLK / (16 * baud)) */
void init_serial(int baud)
{
	uint32_t bdiv_fp6;

	mem_barrier();
	REG_CR = 0;		/* disable UART */

	/* disable pullups for GPIO 14 & 15 */
	gpio_pullups(0xc000, 0, PUD_DISABLE);
	/* select alt0 function for GPIO 14 & 15 */
	/*gpio_fsel(14, FSEL_ALT0);
	gpio_fsel(15, FSEL_ALT0);*/

	REG_ICR = 0x7ff;	/* clear pending interrupts */

	/* calculate baud rate divisor */
	bdiv_fp6 = (rpi_clk_uart << 6) / (16 * baud);
	REG_IBRD = (bdiv_fp6 >> 6) & 0xffff;	/* 16 bits integer part */
	REG_FBRD = bdiv_fp6 & 0x3f;		/* 6 bits fractional precision */

	/* line control: fifo enable, 8n1 */
	REG_LCRH = LCRH_FIFOEN | LCRH_8BITS;
	/* mask all interrupts */
	REG_IMSC = I_CTS | I_RX | I_TX | I_RTIME | I_FRM | I_PAR | I_BRK | I_OVR;

	/* enable UART RX&TX */
	REG_CR = CR_UARTEN | CR_TXEN | CR_RXEN;
	mem_barrier();
}

void ser_putchar(int c)
{
	if(c == '\n') ser_putchar('\r');

	mem_barrier();
	while(REG_FR & FR_TXFF);
	REG_DR = c & 0xff;
	mem_barrier();
}

int ser_getchar(void)
{
	mem_barrier();
	while(REG_FR & FR_RXFE);
	return REG_DR & 0xff;
}

int ser_pending(void)
{
	mem_barrier();
	return (REG_FR & FR_RXFE) == 0;
}

void ser_printstr(const char *s)
{
	while(*s) {
		ser_putchar(*s++);
	}
}

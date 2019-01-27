#include <stdint.h>
#include "serial.h"
#include "uart.h"
#include "gpio.h"

/* baud rate: BAUDDIV = (UART_CLK / (16 * baud)) */
#define UART_CLK	3000000

void init_serial(int baud)
{
	uint32_t bdiv_fp6;

	REG_CR = 0;		/* disable UART */

	/* disable pullups for GPIO 14 & 15 */
	gpio_pullups(0xc000, 0, PUD_DISABLE);
	/* select alt0 function for GPIO 14 & 15 */
	/*gpio_fsel(14, FSEL_ALT0);
	gpio_fsel(15, FSEL_ALT0);*/

	REG_ICR = 0x7ff;	/* clear pending interrupts */

	/* calculate baud rate divisor */
	bdiv_fp6 = (UART_CLK << 6) / (16 * baud);
	REG_IBRD = (bdiv_fp6 >> 6) & 0xffff;	/* 16 bits integer part */
	REG_FBRD = bdiv_fp6 & 0x3f;		/* 6 bits fractional precision */

	/* line control: fifo enable, 8n1 */
	REG_LCRH = LCRH_FIFOEN | LCRH_8BITS;
	/* mask all interrupts */
	REG_IMSC = I_CTS | I_RX | I_TX | I_RTIME | I_FRM | I_PAR | I_BRK | I_OVR;

	/* enable UART RX&TX */
	REG_CR = CR_UARTEN | CR_TXEN | CR_RXEN;
}

void ser_putchar(int c)
{
	while(REG_FR & FR_TXFF);
	REG_DR = c & 0xff;
}

int ser_getchar(void)
{
	while(REG_FR & FR_RXFE);
	return REG_DR & 0xff;
}

void ser_printstr(const char *s)
{
	while(*s) {
		if(*s == '\n') {
			ser_putchar('\r');
		}
		ser_putchar(*s++);
	}
}

	.text
	.code 32

	.equ UART_CLOCK, 48000000
	.equ BAUD, 115200

	.equ UART_BASE, 0x201000
	.equ REG_DR,	0x00
	.equ REG_FR,	0x18
	.equ REG_IBRD,	0x24
	.equ REG_FBRD,	0x28
	.equ REG_LCRH,	0x2c
	.equ REG_CR,	0x30
	.equ REG_IMSC,	0x38
	.equ REG_ICR,	0x44

	.equ LCRH_FIFOEN, 0x10
	.equ LCRH_8BITS, 0x60

	.equ CR_UARTEN,	0x0001
	.equ CR_TXEN,	0x0100
	.equ CR_RXEN,	0x0200

	.equ FR_RXFE,	0x10
	.equ FR_TXFF,	0x20

	.equ GPIO_BASE, 0x200000
	.equ REG_GPPUD, 0x94
	.equ REG_GPPUDCLK0, 0x98
	.equ PUD_DISABLE, 0


	.equ I_CTS,	0x0002
	.equ I_RX,	0x0010
	.equ I_TX,	0x0020
	.equ I_RTIME,	0x0040
	.equ I_FRM,	0x0080
	.equ I_PAR,	0x0100
	.equ I_BRK,	0x0200
	.equ I_OVR,	0x0400


	.macro delay, iter
	ldr r10, =\iter
0:	subs r10, #1
	bne 0b
	.endm

	.global init_serial
init_serial:
	stmfd sp!, {r8,r9}
	dmb
	@ disable pullups for GPIO 14 & 15?
	ldr r9, =iobase
	ldr r8, [r9]
	orr r8, #GPIO_BASE

	mov r0, #PUD_DISABLE
	str r0, [r8, #REG_GPPUD]
	delay 150
	mov r0, #0xc000
	str r0, [r8, #REG_GPPUDCLK0]
	delay 150
	mov r0, #0
	str r0, [r8, #REG_GPPUDCLK0]

	ldr r9, =iobase
	ldr r8, [r9]
	ldr r9, =UART_BASE
	orr r8, r9

	dmb
	@ disable UART
	mov r0, #0
	str r0, [r8, #REG_CR]

	@ clear pending intr
	ldr r0, =0x7ff
	str r0, [r8, #REG_ICR]
	@ set baud rate divisor
	ldr r0, =#(UART_CLOCK << 6) / (16 * BAUD)
	ldr r1, =0xffff
	and r1, r0, LSR #6
	str r1, [r8, #REG_IBRD]
	and r0, #0x3f
	str r0, [r8, #REG_FBRD]
	@ line control: fifo enable, 8n1
	ldr r0, =#LCRH_FIFOEN | LCRH_8BITS
	str r0, [r8, #REG_LCRH]
	@ mask all interrupts
	ldr r0, =#I_CTS | I_RX | I_TX | I_RTIME | I_FRM | I_PAR | I_BRK | I_OVR
	str r0, [r8, #REG_IMSC]

	@ enable UART RX/TX
	ldr r0, =#CR_UARTEN | CR_TXEN | CR_RXEN
	str r0, [r8, #REG_CR]

	dmb
	ldmfd sp!, {r8,r9}
	bx lr

	.global ser_putchar
ser_putchar:
	dmb

	ldr r3, =iobase
	ldr r2, [r3]
	ldr r3, =UART_BASE
	orr r2, r3

	cmp r0, #10
	moveq r0, #13	@ change it to a 13, which will be followed by a 10

	@ wait until there's space in the transmit fifo
0:	ldr r3, [r2, #REG_FR]
	tst r3, #FR_TXFF
	bne 0b
	str r0, [r2, #REG_DR]

	@ if it was a 13, loop back and write a 10
	cmp r0, #13
	moveq r0, #10
	beq 0b

	dmb
	bx lr

	.global ser_getchar
ser_getchar:
	dmb

	ldr r3, =iobase
	ldr r2, [r3]
	ldr r3, =UART_BASE
	orr r2, r3

	@wait until there's something in the recv queue
0:	ldr r3, [r2, #REG_FR]
	tst r3, #FR_RXFE
	bne 0b
	ldr r0, [r2, #REG_DR]
	bx lr

	.global ser_pending
ser_pending:
	dmb

	ldr r3, =iobase
	ldr r2, [r3]
	ldr r3, =UART_BASE
	orr r2, r3

	ldr r0, [r2, #REG_FR]
	ands r0, #FR_RXFE
	movne r0, #0
	moveq r0, #1
	bx lr

	.global ser_printstr
ser_printstr:
	stmfd sp!, {r4,lr}

	mov r4, r0
0:	ldrb r0, [r4], #1
	cmp r0, #0
	beq 1f
	bl ser_putchar
	b 0b

1:	ldmfd sp!, {r4,lr}
	bx lr

@ vi:set filetype=armasm:

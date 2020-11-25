	@ ---- Change IOBASE according to the device you wish to build for -----
	@.equ IOBASE, 0x20000000	@ RPI 1/zero
	.equ IOBASE, 0x3f000000		@ RPI 2/3
	@.equ IOBASE, 0xfe000000	@ RPI 4
	@ ----------------------------------------------------------------------

	.equ GPIO_BASE, 0x200000

	.equ DBGLED_RED, 1
	.equ DBGLED_GREEN, 2
	.equ DBGLED_YELLOW, 3

	.extern init_uart
	.extern ser_putchar
	.extern ser_getchar
	.extern ser_printstr

	.section .startup
	.code 32

	.global startup
startup:
	@ park all but one of the cores (TODO unpark later)
	mrc p15, 0, r0, c0, c0, 5
	ands r0, r0, #0xff
	bne park

	@ put the stacktop at 4000h
	mov sp, #0x4000

	@ move the boot loader out of the way to load the kernel into 8000h later
	mov r8, sp
	ldr r9, =_kern_start
	ldr r10, =_kern_end	@ r10 will act as sentinel
0:	ldmia r9!, {r0-r7}
	stmia r8!, {r0-r7}
	cmp r9, r10
	blo 0b

	@ continue executing in the copy 4000h lower
	sub pc, #0x4000

	mov r0, #DBGLED_RED
	bl dbgled	@ lite up red if we reached this point

	bl init_uart

	ldr r0, ='@'
	bl ser_putchar

	ldr r0, =hello
	bl ser_printstr

	mov r0, #DBGLED_YELLOW
	bl dbgled

park:	wfe
	b park

hello:	.asciz "hello world!\n"
	.align 2

	@ r0 bits 0 and 1, control LEDs connected to GPIO 18 and 23 
	.global dbgled
dbgled:
	ldr r3, iobase
	orr r3, #GPIO_BASE	@ gpio base
	ldr r2, =0x1000000	@ gpio 18 output
	str r2, [r3, #4]
	ldr r2, =0x200		@ gpio 23 output
	str r2, [r3, #8]	@ store to GPFSEL2
	ldr r2, =0x00040000	@ bit 18
	tst r0, #1
	strne r2, [r3, #0x1c]	@ GPSET0
	streq r2, [r3, #0x28]	@ GPCLR0
	ldr r2, =0x00800000
	tst r0, #2
	strne r2, [r3, #0x1c]	@ GPSET0
	streq r2, [r3, #0x28]	@ GPCLR0
	bx lr

	.global iobase
iobase: .long IOBASE

@ vi:set filetype=armasm:

	@ ---- Change IOBASE according to the device you wish to build for -----
	@.equ IOBASE, 0x20000000	@ RPI 1/zero
	.equ IOBASE, 0x3f000000		@ RPI 2/3
	@.equ IOBASE, 0xfe000000	@ RPI 4
	@ ----------------------------------------------------------------------

	.equ SCTLR_DCACHE, 0x0004
	.equ SCTLR_ICACHE, 0x1000

	.section .startup
	.code 32

	.global startup
startup:
	@ park all but one of the cores (TODO unpark later)
	mrc p15, 0, r0, c0, c0, 5
	ands r0, r0, #0xff
	bne park

	@ disable caches
	mrc p15, 0, r0, c1, c0, 0
	bic r0, #SCTLR_DCACHE
	bic r0, #SCTLR_ICACHE
	mcr p15, 0, r0, c1, c0, 0

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

	bl init_serial

	ldr r0, =hello
	bl ser_printstr

mainloop:
	bl ser_getchar
	mov r0, #'.'
	bl ser_putchar
	b mainloop

park:	wfe
	b park

hello:	.ascii "Simple raspberry pi serial port boot loader\n"
	.asciz "by John Tsiombikas <nuclear@member.fsf.org>\n"
	.align 2

	.global iobase
iobase: .long IOBASE

@ vi:set filetype=armasm:

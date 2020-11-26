	.extern main

	.section .startup
	.code 32

	.equ MODE_FIQ, 0x11
	.equ MODE_IRQ, 0x12
	.equ MODE_SVC, 0x13
	.equ MODE_HYP, 0x1a
	.equ MODE_MASK, 0x1f
	.equ FLAG_I, 0x0080
	.equ FLAG_F, 0x0040
	.equ FLAG_A, 0x0100

	.equ SCTLR_DCACHE, 0x0004
	.equ SCTLR_BPRED, 0x0800
	.equ SCTLR_ICACHE, 0x1000

	.global startup
startup:
	@ stop all but one of the cores
	mrc p15, 0, r0, c0, c0, 5
	ands r0, r0, #0xff
	bne exit

	@ set the vector base to 0
	mov r0, #0
	mcr p15, 0, r0, c12, c0, 0

	@ detect if we're running in hyp mode, and drop to svc
	mrs r0, cpsr
	and r1, r0, #MODE_MASK	@ grab the current mode
	cmp r1, #MODE_HYP	@ see if it's hyp (1ah)
	bne hypend		@ skip mode change if it's not

	@ set HVBAR also to 0
	mov r1, #0
	mcr p15, 4, r1, c12, c0, 0

	bic r0, #MODE_MASK	@ mask out the mode bits
	orr r0, #MODE_SVC	@ set the SVC mode bits
	orr r0, #FLAG_I | FLAG_F | FLAG_A  @ make sure i/f/a disabled
	msr spsr_cxsf, r0	@ change the saved PSR
	add lr, pc, #4		@ calculate return address (hypend)
	msr elr_hyp, lr		@ put it in elr_hyp
	eret			@ return from exception to SVC
hypend:
	@ setup initial stacks, allow 4k for each exception stack
	ldr r1, =_stacktop
	mov r0, #MODE_IRQ	@ switch to IRQ mode
	msr cpsr_c, r0
	mov sp, r1
	sub r1, #4096
	mov r0, #MODE_FIQ	@ switch to FIQ mode
	msr cpsr_c, r0
	mov sp, r1
	sub r1, #4096
	mov r0, #MODE_SVC	@ switch back to supervisor mode
	msr cpsr_c, r0
	mov sp, r1

	@ clear bss
	ldr r0, =_bss_start
	ldr r1, =_bss_size
	cmp r1, #0
	beq 1f		@ 0-sized bss, skip clear
	mov r2, #0
0:	str r2, [r0], #4
	subs r1, #4
	bne 0b
1:

	@ enable cache
	mrc p15, 0, r0, c1, c0, 0
	orr r0, #SCTLR_DCACHE
	orr r0, #SCTLR_ICACHE | SCTLR_BPRED
	mcr p15, 0, r0, c1, c0, 0

	blx main

	.global exit
exit:	wfe
	b exit

	.extern rpi_reboot
dbgstop_green:		@ blink green and reboot
	mov r9, #1
	b skipred
dbgstop_red:		@ blink red and reboot
	mov r9, #0
skipred:
	mov r8, #0xf
0:	and r0, r8, #1
	mov r0, r0, LSL r9
	bl dbgled
	mov r0, #0x8000000
1:	subs r0, #1
	bne 1b
	subs r8, #1
	bne 0b
	b rpi_reboot

dbgstop_yellow:		@ blink yellow and reboot
	mov r8, #0xf
0:	and r0, r8, #1
	orr r0, r0, LSL #1
	bl dbgled
	mov r0, #0x8000000
1:	subs r0, #1
	bne 1b
	subs r8, #1
	bne 0b
	b rpi_reboot

	@ r0 bits 0 and 1, control LEDs connected to GPIO 18 and 23 
	.global dbgled
dbgled:
	ldr r3, =0x3f200000	@ gpio base
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

@ vi:set filetype=armasm:

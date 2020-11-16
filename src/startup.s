	.extern main

	.section .startup
	.code 32

	.global startup
startup:
	cpsid if

	@ stop all but one of the cores
	mrc p15, 0, r0, c0, c0, 5
	ands r0, r0, #0xff
	bne exit

	@ detect if we're running in hyp mode, and drop to svc
	mrs r0, cpsr
	and r1, r0, #0x1f
	cmp r1, #0x1a
	bne hypend

	bic r0, #0x1f
	orr r0, #0x13
	@msr spsr_cxsf, r0
	add r0, pc, #4
	msr elr_hyp, r0
	@eret
	mov pc, r0
hypend:

	@ setup initial stacks, allow 4k stack for IRQs
	@mov r0, #0x12	@ switch to IRQ mode
	@msr cpsr, r0
	@ldr sp, =_stacktop
	@mov r0, #0x13	@ switch to supervisor mode
	@msr cpsr, r0
	ldr sp, =_stacktop - 4096

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
	blx main

	.global exit
exit:	wfe
	b exit

@ vi:set filetype=armasm:

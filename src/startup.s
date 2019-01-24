	.extern main

	.section .startup
	.code 32

startup:
	ldr sp, =_stacktop

	mov r0, #2
	bl dbgled

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
	bl main

	mov r0, #0
	bl dbgled

	.global exit
exit:	wfe
	b exit

	.global dbgled
dbgled:
	ldr r3, =0x3f200000	@ gpio base
	ldr r2, =0x9000		@ gpio 24 and 25 -> output
	str r2, [r3, #8]	@ store to GPFSEL2
	ldr r2, =0x01000000	@ bit 24
	tst r0, #1
	strne r2, [r3, #0x1c]	@ GPSET0
	streq r2, [r3, #0x28]	@ GPCLR0
	lsl r2, #1
	tst r0, #2
	strne r2, [r3, #0x1c]	@ GPSET0
	streq r2, [r3, #0x28]	@ GPCLR0
	bx lr


@ vi:set filetype=armasm:

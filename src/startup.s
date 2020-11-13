	.extern main

	.section .startup
	.code 32

startup:
	@ stop all but one of the cores
	mrc p15, 0, r0, c0, c0, 5
	ands r0, r0, #0xff
	bne exit

	@ setup stack
	ldr sp, =_stacktop

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

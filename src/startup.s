	.section .startup
	@ clear bss
	ldr r0, =_bss_start
	ldr r1, =_bss_size
	mov r2, #0
0:	str r2, [r0], #4
	subs r1, #4
	bne 0b

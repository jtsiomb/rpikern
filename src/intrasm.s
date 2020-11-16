	.code 32

@	.section .vect,"a"
@	.extern startup
@intr_vector:
@	b startup
@	b intr_entry_undef
@	b intr_entry_swi
@	b intr_entry_iabort
@	b intr_entry_dabort
@	b intr_entry_irq
@
@	@ FIQ entry point used for timer interrupts
@intr_entry_fiq:
@	ldr r9, =num_ticks
@	ldr r8, [r9]
@	add r8, #1
@	str r8, [r9]
@	subs pc, lr, #4
@	.align 8
@	.ascii "xyzzy"
@
	.text

	.global intr_entry_nop
	.global intr_entry_undef
	.global intr_entry_swi
	.global intr_entry_iabort
	.global intr_entry_dabort
	.global intr_entry_irq
	.global intr_entry_fiq

intr_entry_undef:
intr_entry_swi:
intr_entry_iabort:
intr_entry_dabort:
intr_entry_nop:
	subs pc, lr, #4

intr_entry_irq:
	ldr sp, =_stacktop	@ HACK

	stmfd sp!, {r0, r1}

	mov r0, #'.'
	.extern ser_putchar
	bl ser_putchar

	ldr r1, =num_ticks
	ldr r0, [r1]
	add r0, #1
	str r0, [r1]

	@ setup next interrupt
	ldr r1, =#0x3f003004	@ low counter in rpi2 TODO use rpi_iobase
	ldr r0, [r1]
	add r0, #4000		@ 1Mhz / 250hz = 4k counts
	ldr r1, =#0x3f003010	@ compare 1 reg
	str r0, [r1]

	@ clear interrupt
	ldr r1, =#0x3f003000
	mov r0, #2	@ set bit 1 to clear interrupt from C1
	str r0, [r1]

	ldmfd sp!, {r0, r1}
	subs pc, lr, #4

intr_entry_fiq:
	ldr r9, =num_ticks
	ldr r8, [r9]
	add r8, #1
	str r8, [r9]
	subs pc, lr, #4

@ vi:set ft=armasm:

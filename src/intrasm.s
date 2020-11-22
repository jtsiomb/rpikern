	.code 32
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
	stmfd sp!, {r0-r12,lr}

	ldmfd sp!, {r0-r12,lr}
	subs pc, lr, #4

intr_entry_fiq:
	@ increment ticks counter
	ldr r9, =num_ticks
	ldr r8, [r9]
	add r8, #1
	str r8, [r9]
	@ setup next interrupt
	ldr r9, =#0x3f003004	@ low counter in rpi2 TODO use rpi_iobase
	ldr r8, [r9]
	add r8, #4000		@ 1MHz / 250hz = 4k counts
	ldr r9, =#0x3f003010	@ comparison reg 1
	str r8, [r9]
	@ clear interrupt
	ldr r9, =#0x3f003000
	mov r8, #2	@ set bit 1 to clear interrupt from C1
	str r8, [r9]
	subs pc, lr, #4

@ vi:set ft=armasm:

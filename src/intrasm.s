	.code 32

	.section .vect
	.extern startup
intr_vector:
	b startup
	b intr_entry_undef
	b intr_entry_swi
	b intr_entry_iabort
	b intr_entry_dabort
	b intr_entry_irq

	@ FIQ entry point used for timer interrupts
	.extern num_ticks
intr_entry_fiq:
	ldr r9, =num_ticks
	ldr r8, [r9]
	add r8, #1
	str r8, [r9]
	subs pc, lr, #4

	.text
intr_entry_undef:
intr_entry_swi:
intr_entry_iabort:
intr_entry_dabort:
intr_entry_irq:
	subs pc, lr, #4

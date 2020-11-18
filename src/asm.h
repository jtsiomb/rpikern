#ifndef ASM_H_
#define ASM_H_

#define enable_intr()	asm volatile ("cpsie if")
#define disable_intr()	asm volatile ("cpsid if")
#define mem_barrier()	asm volatile ("dmb" ::: "memory")

#define delay(x)  asm volatile ( \
		"0: subs %0, %0, #1\n\t" \
		"bne 0b\n\t" \
		:: "r"(x) : "cc")

/* TODO ifdef magic for older processors without a wfi instruction:
 * asm volatile("mcr p15, 0, %0, c7, c0, 4":: "r"(0))
 */
#define halt_cpu() asm volatile("wfi");

#endif	/* ASM_H_ */

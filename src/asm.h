#ifndef ASM_H_
#define ASM_H_

#define enable_intr()	asm volatile ("cpsie if")
#define disable_intr()	asm volatile ("cpsid if")
#define mem_barrier()	asm volatile ("dmb" ::: "memory")

#define delay(x)  asm volatile ( \
		"0: subs %0, %0, #1\n\t" \
		"bne 0b\n\t" \
		:: "r"(x) : "cc")

#define halt_cpu() asm volatile("wfe");

#endif	/* ASM_H_ */

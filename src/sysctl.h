#ifndef SYSCTL_H_
#define SYSCTL_H_

/* c7 reg, op1=0, mode c14 is clean&inval, op2=1 is cacheline MVA */
#define sysctl_dcache_clean_inval(addr, len) \
	do { \
		register uint32_t a asm("r0") = addr; \
		asm volatile( \
			"\n0:\tmcr p15, 0, %0, c7, c14, 1" \
			"\n\tadd %0, #64" \
			"\n\tcmp %0, %1" \
			"\n\tblo 0b" \
			:: "r"(a), "r"(addr + len) \
			: "memory"); \
	} while(0)


#endif	/* SYSCTL_H_ */

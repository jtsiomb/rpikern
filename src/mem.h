#ifndef MEM_H_
#define MEM_H_

#define MEM_BUS_COHERENT(addr)	(((uint32_t)addr) | 0x40000000)
#define MEM_BUS_UNCACHED(addr)	(((uint32_t)addr) | 0xc0000000)

#endif	/* MEM_H_ */

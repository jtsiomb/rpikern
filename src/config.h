#ifndef CONFIG_H_
#define CONFIG_H_

#ifndef RPI_MODEL
#define RPI_MODEL 2
#endif

#if RPI_MODEL == 1
#define IO_BASE		0x20000000
#else
#define IO_BASE		0x3f000000
#endif

#endif	/* CONFIG_H_ */

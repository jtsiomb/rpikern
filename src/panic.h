#ifndef PANIC_H_
#define PANIC_H_

void panic(const char *fmt, ...) __attribute__((noreturn));

#endif	/* PANIC_H_ */

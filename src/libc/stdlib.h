#ifndef STDLIB_H_
#define STDLIB_H_

#include <stddef.h>

#define RAND_MAX	2147483647

#define abs(x)	__builtin_abs(x)

int atoi(const char *str);
long atol(const char *str);
long strtol(const char *str, char **endp, int base);

void itoa(int val, char *buf, int base);
void utoa(unsigned int val, char *buf, int base);

double atof(const char *str);
double strtod(const char *str, char **endp);

void exit(int x) __attribute__((noreturn));	/* defined in startup.s */
void abort(void);

void qsort(void *arr, size_t count, size_t size, int (*cmp)(const void*, const void*));

int rand(void);
int rand_r(unsigned int *seedp);
void srand(unsigned int seed);

#endif	/* STDLIB_H_ */

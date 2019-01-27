#ifndef LIBC_STRING_H_
#define LIBC_STRING_H_

void *memset(void *ptr, int val, int size);
void *memcpy(void *dest, void *src, int size);

int strcmp(const char *a, const char *b);

#endif	/* LIBC_STRING_H_ */

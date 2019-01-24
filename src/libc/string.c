#include "string.h"

void *memset(void *ptr, int val, int size)
{
	unsigned char *p = ptr;
	while(size--) {
		*p++ = val;
	}
	return ptr;
}

void *memcpy(void *dest, void *src, int size)
{
	unsigned char *d = dest;
	unsigned char *s = src;
	while(size--) {
		*d++ = *s++;
	}
	return dest;
}

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

int strcmp(const char *a, const char *b)
{
	while(*a && *a == *b) {
		a++;
		b++;
	}

	if(!*a && !*b) return 0;

	if(!*a) return -1;
	if(!*b) return 1;
	return *a - *b > 0 ? 1 : -1;
}

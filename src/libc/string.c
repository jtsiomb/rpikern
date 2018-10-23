#include "string.h"

void *memset(void *ptr, int val, int size)
{
	unsigned char *p = ptr;
	while(size--) {
		*p++ = val;
	}
	return ptr;
}

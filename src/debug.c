#include <stdio.h>
#include <ctype.h>
#include "debug.h"


void hexdump(void *start, int nbytes)
{
	int i, j, nlines;
	unsigned char *ptr = start;
	unsigned char *end = ptr + nbytes;

	nlines = (nbytes + 0xf) >> 4;

	for(i=0; i<nlines; i++) {
		printf("%08x  ", (uint32_t)ptr);

		for(j=0; j<16; j++) {
			if(ptr + j < end) {
				printf("%02x ", ptr[j]);
			} else {
				printf("   ");
			}
			if((j & 7) == 7) putchar(' ');
		}

		putchar('|');
		for(j=0; j<16; j++) {
			if(ptr + j < end) {
				putchar(isprint(ptr[j]) ? ptr[j] : '.');
			} else {
				putchar(' ');
			}
		}
		printf("|\n");

		ptr += 16;
	}
}

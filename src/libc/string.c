#include <string.h>
#include <ctype.h>
#include <stddef.h>

void *memset(void *ptr, int val, size_t size)
{
	unsigned char *p = ptr;
	while(size--) {
		*p++ = val;
	}
	return ptr;
}

void *memcpy(void *dest, const void *src, size_t n)
{
	unsigned char *d = dest;
	const unsigned char *s = src;
	while(n--) {
		*d++ = *s++;
	}
	return dest;
}

void *memmove(void *dest, const void *src, size_t n)
{
	int i;
	char *dptr;
	const char *sptr;

	if(dest <= src) {
		/* forward copy */
		dptr = dest;
		sptr = src;
		for(i=0; i<n; i++) {
			*dptr++ = *sptr++;
		}
	} else {
		/* backwards copy */
		dptr = (char*)dest + n - 1;
		sptr = (const char*)src + n - 1;
		for(i=0; i<n; i++) {
			*dptr-- = *sptr--;
		}
	}

	return dest;
}

int memcmp(void *aptr, void *bptr, size_t n)
{
	int i, startoffs, diff;
	uint32_t *a32, *b32;
	unsigned char *a = aptr;
	unsigned char *b = bptr;

	a32 = (uint32_t*)((intptr_t)(a + 3) & 0xfffffffc);
	b32 = (uint32_t*)((intptr_t)(b + 3) & 0xfffffffc);

	/* if both are aligned the same way... */
	if((startoffs = (unsigned char*)a32 - a) == (unsigned char*)b32 - b) {
		/* catch-up to the 32bit alignment */
		for(i=0; i<startoffs; i++) {
			if((diff = *a++ - *b++) != 0 || --n <= 0) {
				return diff;
			}
		}

		/* compare 32bit at once */
		while(n >= 4) {
			if(*a32 != *b32) break;
			a32++;
			b32++;
			n -= 4;
		}

		/* update byte pointers to contine with the tail */
		a = (unsigned char*)a32;
		b = (unsigned char*)b32;
	}

	/* we're here both for the tail-end of same-alignment buffers, or for the
	 * whole length of mis-aligned buffers.
	 */
	while(n-- > 0) {
		if((diff = *a++ - *b++) != 0) {
			return diff;
		}
	}
	return 0;
}

size_t strlen(const char *s)
{
	size_t len = 0;
	while(*s++) len++;
	return len;
}

char *strchr(const char *s, int c)
{
	while(*s) {
		if(*s == c) {
			return (char*)s;
		}
		s++;
	}
	return 0;
}

char *strrchr(const char *s, int c)
{
	const char *ptr = s;

	/* find the end */
	while(*ptr) ptr++;

	/* go back checking for c */
	while(--ptr >= s) {
		if(*ptr == c) {
			return (char*)ptr;
		}
	}
	return 0;
}

char *strstr(const char *str, const char *substr)
{
	while(*str) {
		const char *s1 = str;
		const char *s2 = substr;

		while(*s1 && *s1 == *s2) {
			s1++;
			s2++;
		}
		if(!*s2) {
			return (char*)str;
		}
		str++;
	}
	return 0;
}

char *strcasestr(const char *str, const char *substr)
{
	while(*str) {
		const char *s1 = str;
		const char *s2 = substr;

		while(*s1 && tolower(*s1) == tolower(*s2)) {
			s1++;
			s2++;
		}
		if(!*s2) {
			return (char*)str;
		}
		str++;
	}
	return 0;
}

int strcmp(const char *s1, const char *s2)
{
	while(*s1 && *s1 == *s2) {
		s1++;
		s2++;
	}
	return *s1 - *s2;
}

int strcasecmp(const char *s1, const char *s2)
{
	while(*s1 && tolower(*s1) == tolower(*s2)) {
		s1++;
		s2++;
	}
	return tolower(*s1) - tolower(*s2);
}

int strncmp(const char *s1, const char *s2, int n)
{
	if(n <= 0) return 0;

	while(n-- > 0 && *s1 && *s2 && *s1 == *s2) {
		s1++;
		s2++;
	}

	if(n <= 0) return 0;
	return *s1 - *s2;
}

int strncasecmp(const char *s1, const char *s2, int n)
{
	if(n <= 0) return 0;

	while(n-- > 0 && *s1 && *s2 && tolower(*s1) == tolower(*s2)) {
		s1++;
		s2++;
	}

	if(n <= 0) return 0;
	return tolower(*s1) - tolower(*s2);
}

char *strcpy(char *dest, const char *src)
{
	char *dptr = dest;
	while((*dptr++ = *src++));
	return dest;
}

char *strcat(char *dest, const char *src)
{
	strcpy(dest + strlen(dest), src);
	return dest;
}

char *strncpy(char *dest, const char *src, int n)
{
	char *dptr = dest;
	while(n-- > 0 && (*dptr++ = *src++));
	return dest;
}


static const char *errstr[] = {
	"Success",
	"Foo",
	"Interrupted",
	"Invalid",
	"Child",
	"Timeout",
	"Out of memory",
	"I/O error",
	"Not found",
	"Name too long",
	"No space left on device",
	"Permission denied",
	"Not a directory",
	"Is a directory",
	"Does not exist",
	0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,
	"Bug"
};

char *strerror(int err)
{
	if(err < 0 || err > sizeof errstr / sizeof *errstr || !errstr[err]) {
		return "Unknown";
	}
	return (char*)errstr[err];
}

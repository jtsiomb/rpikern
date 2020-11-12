#include <setjmp.h>

int setjmp(jmp_buf buf)
{
	return __builtin_setjmp(buf);
}

void longjmp(jmp_buf buf, int val)
{
	__builtin_longjmp(buf, 1);
}

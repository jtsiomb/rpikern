#ifndef SETJMP_H_
#define SETJMP_H_

typedef unsigned long jmp_buf[5];

int setjmp(jmp_buf buf);
void longjmp(jmp_buf buf, int val);

#endif	/* SETJMP_H_ */

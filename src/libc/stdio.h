#ifndef STDIO_H_
#define STDIO_H_

#include <stdlib.h>
#include <stdarg.h>

typedef struct FILE FILE;

#define SEEK_SET	0
#define SEEK_CUR	1
#define SEEK_END	2

#define EOF	(-1)

#define stdin	((FILE*)0)
#define stdout	((FILE*)1)
#define stderr	((FILE*)2)

int putchar(int c);
int puts(const char *s);

int printf(const char *fmt, ...);
int vprintf(const char *fmt, va_list ap);

int sprintf(char *buf, const char *fmt, ...);
int vsprintf(char *buf, const char *fmt, va_list ap);

int snprintf(char *buf, size_t sz, const char *fmt, ...);
int vsnprintf(char *buf, size_t sz, const char *fmt, va_list ap);

/* TODO */
int fprintf(FILE *fp, const char *fmt, ...);
int vfprintf(FILE *fp, const char *fmt, va_list ap);

/* TODO
int fscanf(FILE *fp, const char *fmt, ...);
int vfscanf(FILE *fp, const char *fmt, va_list ap);

int sscanf(const char *str, const char *fmt, ...);
int vsscanf(const char *ptr, const char *fmt, va_list ap);
*/

/* printf to the serial port */
int ser_printf(const char *fmt, ...);
int ser_vprintf(const char *fmt, va_list ap);

void perror(const char *s);

int getchar(void);

#endif	/* STDIO_H_ */

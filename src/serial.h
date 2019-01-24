#ifndef SERIAL_H_
#define SERIAL_H_

void init_serial(int baud);

void ser_putchar(int c);
int ser_getchar(void);

void ser_printstr(const char *s);

#endif	/* SERIAL_H_ */

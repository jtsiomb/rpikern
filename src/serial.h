#ifndef SERIAL_H_
#define SERIAL_H_

void init_serial(int baud);

void ser_putchar(int c);
int ser_getchar(void);

int ser_pending(void);

void ser_printstr(const char *s);

#endif	/* SERIAL_H_ */

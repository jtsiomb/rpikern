#ifndef CTYPES_H_
#define CTYPES_H_

int isalnum(int c);
int isalpha(int c);
#define isascii(c)	((c) < 128)
int isblank(int c);
int isdigit(int c);
int isupper(int c);
int islower(int c);
int isprint(int c);
int isspace(int c);

int toupper(int c);
int tolower(int c);


#endif	/* CTYPES_H_ */

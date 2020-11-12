#ifndef MATH_H_
#define MATH_H_

#define INFINITY	__builtin_inff()
#define NAN			__builtin_nanf

#define M_PI		3.141592653589793

#define sin(x)		__builtin_sin(x)
#define cos(x)		__builtin_cos(x)
#define tan(x)		__builtin_tan(x)
#define fabs(x)		__builtin_fabs(x)
#define fmod(x, y)	__builtin_fmod(x, y)
#define sqrt(x)		__builtin_sqrt(x)
#define atan2(y, x)	__builtin_atan2(y, x)

double pow(double x, double y);

#endif	/* MATH_H_ */

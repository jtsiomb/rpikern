#include "math.h"

static double calc_pow(double x, double y, double precision);

double pow(double x, double y)
{
	if(y == 0.0 || y == -0.0) {
		return 1.0;
	}
	if(y == 1.0) {
		return x;
	}
	if(y == -INFINITY) {
		return fabs(x) < 1.0 ? INFINITY : 0.0;
	}
	if(y == INFINITY) {
		return fabs(x) < 1.0 ? 0.0 : INFINITY;
	}
	return calc_pow(x, y, 1e-6);
}

static double calc_pow(double x, double y, double precision)
{
	if(y < 0.0) {
		return 1.0 / calc_pow(x, -y, precision);
	}
	if(y >= 10.0) {
		double p = calc_pow(x, y / 2.0, precision / 2.0);
		return p * p;
	}
	if(y >= 1.0) {
		return x * calc_pow(x, y - 1.0, precision);
	}
	if(precision >= 1) {
		return __builtin_sqrt(x);
	}
	return __builtin_sqrt(calc_pow(x, y * 2.0, precision * 2.0));
}

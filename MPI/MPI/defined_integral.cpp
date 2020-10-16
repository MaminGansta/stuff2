
#include "defined_integral.h"

double DefiniteIntegral(IntegralFunction func,
	double from,
	double to,
	double step,
	IntegralType type)
{
	double Integral = 0.0;
	double n = (to - from) / step; // число разбиений

	switch (type)
	{
		// вычисляем интеграл по формуле центральных прямугольников
		case IntegralType::CentralRectangles:
		{
			for (double i = 1.0; i <= n; i++) {
				Integral = Integral + step * func(from + step * (i - 0.5));
			}
		}break;

		// вычисляем интеграл по формуле трапеций
		case IntegralType::Trapezium:
		{
			Integral = step * (func(from) + func(to)) / 2.0;
			for (double i = 1.0; i <= n - 1.0; i++) {
				Integral = Integral + step * func(from + step * i);
			}
		}break;

		// вычисляем интеграл по формуле Симпсона
		case IntegralType::Simpson:
		{
			Integral = step * (func(from) + func(to)) / 6.0;
			for (double i = 1.0; i <= n; i++) {
				Integral = Integral + 4.0 / 6.0 * step * func(from + step * (i - 0.5));
			}
			for (double i = 1.0; i <= n - 1.0; i++) {
				Integral = Integral + 2.0 / 6.0 * step * func(from + step * i);
			}
		}break;
	}
	return Integral;
}

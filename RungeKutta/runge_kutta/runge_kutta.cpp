
#include "runge_kutta.h"


std::tuple<std::vector<double>, 
		   std::vector<double>> runge_kutta(RungeKuttaFunction func,
											double from,
											double to,
											double clause,
											double step)
{
	std::vector<double> X;
	std::vector<double> Y;

	double n = (to - from) / step;
	X.resize((int)n);
	Y.resize((int)n);

	// Start clause
	X[0] = from; Y[0] = clause;

	// Calculate
	for (int i = 1; i < n; i++)
	{
		X[i] = from + i * step;
		double K1 = step * func(X[i - 1], Y[i - 1]);
		double K2 = step * func(X[i - 1] + step / 2.0, Y[i - 1] + K1 / 2.0);
		double K3 = step * func(X[i - 1] + step / 2.0, Y[i - 1] + K2 / 2.0);
		double K4 = step * func(X[i - 1] + step, Y[i - 1] + K3);
		Y[i] = Y[i - 1] + (K1 + 2.0 * K2 + 2.0 * K3 + K4) / 6.0;
	}

	return { std::move(X), std::move(Y) };
}


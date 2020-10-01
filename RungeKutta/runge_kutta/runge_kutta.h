#pragma once

#include <vector>
#include <tuple>

typedef double (*RungeKuttaFunction)(double, double);


/*
	start clause: Y[0] = clause
*/
std::tuple<std::vector<double>,
		   std::vector<double>> runge_kutta(RungeKuttaFunction func,
											double from,
											double to,
											double clause,
											double step = 0.1);

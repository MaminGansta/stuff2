
#include <iostream>
#include <cmath>

#include "runge_kutta/runge_kutta.h"


int main()
{
	auto F = [](double x, double y)->double {
		return 3 * sin(2 * y) + x;
	};
	
	auto [X, Y] = runge_kutta(F, 0.0, 1.0, 2.0);
	
	
	// Output
	for (int i = 0; i < X.size(); i++) {
		std::cout << "X[" << i << "]=" << X[i] << "\n";
	}
	std::cout << std::endl;
	
	for (int i = 0; i < Y.size(); i++) {
		std::cout << "Y[" << i << "]=" << Y[i] << "\n";
	}

	return 0;
}
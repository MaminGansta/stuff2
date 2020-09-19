#include <iostream>
#include <math.h>
#include <omp.h>

#include "integral/defined_integral.h"
#include "timer/timer.h"

#include <time.h>

#include <vector>

int main()
{
	double from, to;
	auto function = [](double x) { return sin(x); };
	
   std::cout << "Enter the interval for integral calculation\n";
   std::cin >> from >> to;
	
   if (to < from) {
       std::swap(to, from);
   }
	
   // Single core
   {
       my::Timer timer;
		double step = 0.01;
       double result = DefiniteIntegral(function, from, to, step);
       std::cout << "\n\nSingle core\nResult: " << result << "\n";
   }
	
   // Multi core
	{
		my::Timer timer;
	    double result = 0.0;
	
		int max_threads = omp_get_max_threads();
	    double range = to - from;
	    double range_step = range / max_threads;
	
	    #pragma omp parallel firstprivate(from, range_step) shared(result) num_threads(max_threads)
	    {
	        int thread_id = omp_get_thread_num();
	
			double local_from = from + range_step * thread_id;
			double local_to = from + range_step * (thread_id + 1);
			double step = 0.01;
		
			double local_reslut = DefiniteIntegral(function, local_from, local_to, step);
		
			#pragma omp atomic
			result += local_reslut;
	    }
	
		std::cout << "\n\nMulti core\nResult: " << result << "\n";
	}

}

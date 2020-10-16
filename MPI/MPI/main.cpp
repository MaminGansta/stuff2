
#include "mpi.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "defined_integral.h"


int main(int argc, char** argv)
{
	int rank;
	double a = 0.0;
	double b = 0.0;

	MPI_Status status;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	int group_size;
	MPI_Comm_size(MPI_COMM_WORLD, &group_size);

	auto F = [](double x) { return sin(x); };
	double from = 0.0; 
	double to = 2.0;
	double step = 0.1;

	// Root
	if (rank == 0)
	{
		double range_step = (to - from) / group_size;

		// Send range info to childs
		for (int i = 1; i < group_size; i++)
		{
			double child_from = from + range_step * i;
			double child_to = to + range_step * (i + 1);

			MPI_Send(&child_from, 1, MPI_DOUBLE, i, 5, MPI_COMM_WORLD);
			MPI_Send(&child_to, 1, MPI_DOUBLE, i, 5, MPI_COMM_WORLD);
		}

		double res = DefiniteIntegral(F, from + range_step, to + range_step, step);

		// Get results
		for (int i = 1; i < group_size; i++)
		{
			double child_res;
			MPI_Recv(&child_res, 1, MPI_DOUBLE, i, 5, MPI_COMM_WORLD, &status);
			res += child_res;
		}

		printf("Result: %lf", res);

	}
	// Child
	else
	{
		double child_from = 0.0;
		double child_to = 0.0;
		MPI_Recv(&from, 1, MPI_DOUBLE, 0, 5, MPI_COMM_WORLD, &status);
		MPI_Recv(&to, 1, MPI_DOUBLE, 0, 5, MPI_COMM_WORLD, &status);

		double res = DefiniteIntegral(F, child_from, child_to, step);

		MPI_Send(&res, 1, MPI_DOUBLE, 0, 5, MPI_COMM_WORLD);
	}

	MPI_Finalize();
}
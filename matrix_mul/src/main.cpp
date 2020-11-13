
#include <mpi.h>
#include <cstdio>

#include "mpi_send.h"
#include "geometry/geometry.h"


// Temp: Geometry Debug
void PrintMat(const gm::mat& mat)
{
	for (int i = 0; i < mat.GetRowsNum(); i++)
	{
		for (int j = 0; j < mat.GetColsNum(); j++)
		{
			printf("%0.1f ", mat[i][j]);
		}
		printf("\n");
	}
	printf("\n");
}

void PrintVec(const gm::vec& vec)
{
	for (int i = 0; i < vec.GetSize(); i++)
	{
		printf("%0.1f ", vec[i]);
	}
	printf("\n\n");
}

void FillMat(gm::mat& mat)
{
	for (int i = 0; i < mat.GetRowsNum(); i++)
	{
		for (int j = 0; j < mat.GetColsNum(); j++)
		{
			mat[i][j] = i * mat.GetColsNum() + j;
		}
	}
}



int main(int argc, char* argv[])
{
	// Temp data
	gm::mat mat1(10, 10);
	gm::mat mat2;

	FillMat(mat1);
	mat2 = gm::Mat<float>::GetIdentity(mat1.GetRowsNum(), mat1.GetColsNum());
	
	for (int i = 0; i < mat2.GetColsNum(); i++) {
		mat2[i][i] = 2.0f;
	}



	int err_code;
	if ((err_code = MPI_Init(&argc, &argv)) != 0)
	{
		return err_code;
	}

	int rank;
	MPI_Status status;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	// Master thread
	if (rank == 0)
	{
		int group_size;
		MPI_Comm_size(MPI_COMM_WORLD, &group_size);

		// Calculate ranges
		std::vector<std::pair<int, int>> from_to;
		for (int thread = 0; thread < group_size; thread++)
		{
			int from = thread * mat1.GetRowsNum() / group_size;
			int to = (thread + 1) * mat1.GetRowsNum() / group_size;
			from_to.push_back({ from, to });
		}

		// Send data to children
		for (int thread = 1; thread < group_size; thread++)
		{
			auto [from, to] = from_to[thread];
			int rows_amount = to - from;

			// Send amount of rows
			SendInt(rows_amount, thread);
			for (int j = from; j < to; j++)
			{
				SendVec(mat1.GetRow(j), thread);
			}
			SendMat(mat2, thread);
		}
		
		gm::mat res(mat1.GetColsNum(), mat2.GetRowsNum());

		// Calculate master part
		auto [from, to] = from_to[0];
		int rows_num = to - from;

		for (int i = 0; i < rows_num; i++)
		{
			for (int j = 0; j < mat2.GetColsNum(); j++)
			{
				res[i][j] = gm::RowColMul(mat1.GetRow(i), mat2.GetCol(j));
			}
		}

		// Take local results from children
		for (int thread = 1; thread < group_size; thread++)
		{
			auto [from, to] = from_to[thread];
			int rows_amount = to - from;

			for (int j = from; j < to; j++)
			{
				res.SetRow(j, RecvVec(thread, status));
			}
		}

		PrintMat(res);
	}

	// Child threads
	if (rank != 0)
	{
		int rows_num = RecvInt(0, status);
		std::vector<gm::vec> rows;
		rows.reserve(rows_num);
		
		// Receive data
		for (int i = 0; i < rows_num; i++) {
			rows.push_back(RecvVec(0, status));
		}
		gm::mat mat = RecvMat(0, status);


		// Calculate
		std::vector<gm::vec> res(rows_num, mat.GetRowsNum());

		for (int i = 0; i < rows_num; i++)
		{
			for (int j = 0; j < mat.GetColsNum(); j++)
			{
				res[i][j] = gm::RowColMul(rows[i], mat.GetCol(j));
			}
		}

		// Send results
		for (int i = 0; i < res.size(); i++) 
		{
			SendVec(res[i], 0);
		}
	}

	MPI_Finalize();
	return 0;
}

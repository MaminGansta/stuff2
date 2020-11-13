
#include <mpi.h>
#include "geometry/geometry.h"


inline void SendInt(int data, int destination)
{
	MPI_Send(&data, 1, MPI_INT, destination, 5, MPI_COMM_WORLD);
}

inline int RecvInt(int source, MPI_Status& status)
{
	int data;
	MPI_Recv(&data, 1, MPI_INT, source, 5, MPI_COMM_WORLD, &status);
	return data;
}


inline void SendVec(const gm::vec& vec, int destination)
{
	int size = vec.GetSize();
	MPI_Send(&size, 1, MPI_INT, destination, 5, MPI_COMM_WORLD);
	MPI_Send(vec.GetData(), size, MPI_FLOAT, destination, 5, MPI_COMM_WORLD);
}

inline gm::vec RecvVec(int source, MPI_Status& status)
{
	int len = 0;
	MPI_Recv(&len, 1, MPI_INT, source, 5, MPI_COMM_WORLD, &status);

	gm::vec vec(len);
	MPI_Recv(vec.GetData(), len, MPI_FLOAT, source, 5, MPI_COMM_WORLD, &status);

	return std::move(vec);
}


inline void SendMat(const gm::mat& mat, int destination)
{
	int rows = mat.GetRowsNum();
	int cols = mat.GetColsNum();
	MPI_Send(&rows, 1, MPI_INT, destination, 5, MPI_COMM_WORLD);
	MPI_Send(&cols, 1, MPI_INT, destination, 5, MPI_COMM_WORLD);
	MPI_Send(mat.GetData(), rows * cols, MPI_FLOAT, destination, 5, MPI_COMM_WORLD);
}

inline gm::mat RecvMat(int source, MPI_Status& status)
{
	int rows = 0;
	int cols = 0;

	MPI_Recv(&rows, 1, MPI_INT, source, 5, MPI_COMM_WORLD, &status);
	MPI_Recv(&cols, 1, MPI_INT, source, 5, MPI_COMM_WORLD, &status);

	gm::mat mat(rows, cols);
	MPI_Recv(mat.GetData(), rows * cols, MPI_FLOAT, source, 5, MPI_COMM_WORLD, &status);

	return std::move(mat);
}

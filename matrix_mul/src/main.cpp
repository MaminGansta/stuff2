

#include <iostream>
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
		printf("\n\n");
	}
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



int main()
{
	gm::mat mat(3, 3);
	gm::mat mat2;

	FillMat(mat);
	mat2 = gm::Mat<float>::GetIdentity(3, 3);


	PrintMat(mat);
	PrintMat(mat2);

	PrintVec(mat.GetCol(0));
	PrintVec(mat2.GetCol(1));

	gm::mat res = gm::MatMul(mat, mat2);
	PrintMat(res);
}
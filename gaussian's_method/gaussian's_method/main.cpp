
#include <iostream>
#include <string>
#include <sstream>
#include <cassert>
#include <vector>



struct matf
{
	float* mat;
	size_t row, column;


	matf() : mat(nullptr), row(0), column(0) {}
	matf(size_t row, size_t column) :  row(row), column(column) {
		mat = new float[row * column];
	}

	~matf() { 
		delete[] mat;
	}

	float* operator [] (size_t i)
	{
		assert(i < row);
		return mat + i * column;
	}

	matf(matf& other)
	{
		row = other.row;
		column = other.column;
		mat = new float[row * column];
		memmove(mat, other.mat, sizeof(float) * row * column);
	}

	matf(matf&& other)
	{
		mat = other.mat;
		row = other.row;
		column = other.column;
		other.mat = nullptr;
	}

	matf& operator =(matf&& other)
	{
		delete[] mat;
		mat = other.mat;
		row = other.row;
		column = other.column;
		other.mat = nullptr;
		return *this;
	}
};



matf input()
{
	float plug;
	size_t columns = 0;

	std::string line;
	std::getline(std::cin, line);
	std::stringstream stream(line);
	while (!stream.eof())
	{
		stream >> plug;
		columns++;
	}

	matf mat(columns - 1, columns);
	stream.seekg(0);

	for (int y = 0; y < columns - 1; y++)
	{
		if (y != 0)
		{
			std::getline(std::cin, line);
			stream.str(std::move(line));
			stream.seekg(0);
		}

		for (int x = 0; x < columns; x++)
		{
			if (stream.eof()) return matf();
			stream >> mat[y][x];
		}

		if (!stream.eof()) return matf();
	}

	return mat;
}


std::vector<float> gausian_method(matf mat)
{
	std::vector<float> res(mat.row);

	for (int column = 0, row = 0; column < mat.column && row < mat.row; column++)
	{
		int pivot = row;
		for (int i = row; i < mat.row; i++)
			if (fabs(mat[pivot][column]) < fabs(mat[i][column])) pivot = i;

		if (fabs(mat[pivot][column]) < 0.0001f) continue;

		for (int i = 0; i < mat.column; i++)
			std::swap(mat[pivot][i], mat[row][i]);

		for (int i = 0; i < mat.row; i++)
		{
			if (i != row)
			{
				float c = mat[i][column] / mat[row][column];
				for (int j = column; j <= mat.row; j++)
					mat[i][j] -= mat[row][j] * c;
			}
		}
		row++;
	}

	for (int i = 0; i < mat.row; i++)
		res[i] = mat[i][mat.column-1] / mat[i][i];

	return res;
}


int main(void)
{
	std::cout << "write a coefficient matrix below\n";
	
	matf mat; 
	while (mat = input(), !mat.mat)
		std::cout << "invalid input, try agen\n";

	std::vector<float> res = gausian_method(mat);

	return 0;
}

/*
5 1 1 10
6 3 5 27
1 1 6 21
*/
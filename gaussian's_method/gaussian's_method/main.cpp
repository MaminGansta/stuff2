#include <iostream>
#include <string>
#include <sstream>
#include <cassert>
#include <vector>
#include <tuple>


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
	size_t rows = 0;

	std::cout << "rows >> ";
	std::string line;
	std::getline(std::cin, line);
	std::stringstream stream(line);

	// rows
	stream >> rows;
	std::cout << "write a coefficient matrix below\n";

	// columns
	std::getline(std::cin, line);
	stream.str(line);
	stream.seekg(0);

	while (!stream.eof())
	{
		stream >> plug;
		columns++;
	}

	// data input
	matf mat(rows, columns);
	stream.seekg(0);

	for (int y = 0; y < rows; y++)
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


// flag != 0 if system have at least one solution, otherwise 0

std::tuple<int, std::vector<float>> gausian_method(matf mat)
{
	int columns = mat.column - 1;
	std::vector<float> res(columns);
	std::vector<int> where(columns, -1);

	for (int column = 0, row = 0; column < columns && row < mat.row; column++)
	{
		int pivot = row;
		for (int i = row; i < mat.row; i++)
			if (fabs(mat[pivot][column]) < fabs(mat[i][column])) pivot = i;

		if (fabs(mat[pivot][column]) < 0.0001f) continue;

		for (int i = 0; i < mat.column; i++)
			std::swap(mat[pivot][i], mat[row][i]);
		
		where[column] = row;

		for (int i = 0; i < mat.row; i++)
		{
			if (i != row)
			{
				double c = mat[i][column] / mat[row][column];
				for (int j = column; j <= mat.row; j++)
					mat[i][j] -= mat[row][j] * c;
			}
		}
		row++;
	}

	int flag = 1;

	for (int i = 0; i < columns; ++i)
		if (where[i] != -1)
			res[i] = mat[where[i]][columns] / mat[where[i]][i];

	for (int i = 0; i < mat.row; ++i)
	{
		float sum = 0;
		for (int j = 0; j < columns; ++j)
			sum += res[j] * mat[i][j];
		
		if (abs(sum - mat[i][columns]) > 0.0001f)
		{
			flag = 0;
			break;
		}
	}

	if (flag)
		for (int i = 0; i < columns; ++i)
			if (where[i] == -1)
				flag = -1;

	return {flag, res};
}


int main(void)
{
	
	matf mat; 
	while (mat = input(), !mat.mat)
		std::cout << "invalid input, try agen\n";

	auto [flag, res] = gausian_method(mat);

	const char* types[3] = { "inf", "non", "yes" };
	std::cout << "\nsolution: " << types[flag + 1] << '\n';

	for (int i = 0; i < res.size(); i++)
		std::cout << 'a' << i << '=' << res[i] << ' ';

	std::cout << '\n';
	return 0;
}

/*
have solution

3
5 1 1 10
6 3 5 27
1 1 6 21

*/

/*
have no solution

4
5 1 1 10
6 3 5 27
1 3 6 15
5 1 6 31

*/

/*
have no solution

3
5 1 1 10
10 2 2 27
1 1 6 21

*/

/*
have inf solutions

3
5 1 1 10
10 2 2 20
1 1 6 21

*/
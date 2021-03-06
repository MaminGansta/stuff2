#include <tuple>

Fraction abs(Fraction fraction)
{
	return Fraction(abs(fraction.top), abs(fraction.bottom));
}


template <typename T>
std::tuple<int, std::vector<T>> gausian_method(Mat<T>& mat)
{
	int columns = mat.column - 1;
	std::vector<T> res(columns);
	std::vector<int> where(columns, -1);

	for (int column = 0, row = 0; column < columns && row < mat.row; column++)
	{
		int pivot = row;
		for (int i = row; i < mat.row; i++)
			if (abs(mat[pivot][column]) < abs(mat[i][column])) pivot = i;

		if (abs(mat[pivot][column] + 0.0f) < 0.0001f) continue;

		for (int i = 0; i < mat.column; i++)
			std::swap(mat[pivot][i], mat[row][i]);

		where[column] = row;

		for (int i = 0; i < mat.row; i++)
		{
			if (i != row)
			{
				T c = mat[i][column] / mat[row][column];
				for (int j = column; j < mat.column; j++)
					mat[i][j] -= mat[row][j] * c;
			}
		}
		row++;
	}

	// normalize
	for (int i = 0; i < mat.row; i++)
	{
		if (abs(1.0f - mat[i][i]) > 0.01f)
		{
			T c = mat[i][i];
			for (int j = 0; j < mat.column; j++)
				mat[i][j] = mat[i][j] / c;
		}
	}


	int flag = 1;

	for (int i = 0; i < columns; ++i)
		if (where[i] != -1)
			res[i] = mat[where[i]][columns] / mat[where[i]][i];

	for (int i = 0; i < mat.row; ++i)
	{
		T sum = 0;
		for (int j = 0; j < columns; ++j)
			sum = sum + mat[i][j] * res[j];

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

	return { flag, res };
}
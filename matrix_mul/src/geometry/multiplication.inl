#pragma once

#include "multiplication.h"


namespace gm
{
	template <typename T>
	Mat<T> MatMul(const Mat<T>& mat1, const Mat<T>& mat2)
	{
		assert(mat1.GetColsNum() == mat2.GetRowsNum(), "Incompatible matrices sizes");

		Mat<T> res(mat1.GetRowsNum(), mat2.GetColsNum());
		for (int i = 0; i < mat1.GetRowsNum(); i++)
		{
			for (int j = 0; j < mat2.GetColsNum(); j++)
			{
				res[i][j] = RowColMul<Vec<T>>(mat1.GetRow(i), mat2.GetCol(j));
			}
		}
		
		return std::move(res);
	}


	template <typename VecType>
	auto RowColMul(const VecType& row, const VecType& col) -> typename VecType::value_type
	{
		assert(row.GetSize() == col.GetSize(), "Incompatible vectors size");

		VecType::value_type res = VecType::value_type{0};
		for (int i = 0; i < row.GetSize(); i++)
		{
			res += row[i] * col[i];
		}
		
		return res;
	}

}
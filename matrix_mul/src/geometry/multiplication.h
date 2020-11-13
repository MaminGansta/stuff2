#pragma once

#include "mat.h"
#include "vec.h"
#include "vec_view.h"

namespace gm
{
	template <typename T>
	Mat<T> MatMul(const Mat<T>& mat1, const Mat<T>& mat2);

	template <typename VecType>
	auto RowColMul(const VecType& row, const VecType& col) -> typename VecType::value_type;

}
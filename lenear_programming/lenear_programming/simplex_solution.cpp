

template<typename T>
void mat_output(Mat<T>& mat)
{
	for (int i = 0; i < mat.row; i++)
	{
		for (int j = 0; j < mat.column; j++)
		{
			wchar_t buffer[32];
			to_str(buffer, mat[i][j]);
			doutput(L"%s", buffer);
		}

		doutput("\n");
	}
	doutput("\n");
}



template <typename T>
struct Simplex_step
{
	int iteration = 0;
	int pivot_x = 0, povot_y = 0;
	Mat<T> mat; // simplex max

	//Simplex_step() = default;
};


template <typename T>
struct Simplex_window : Window
{
	ListView table;
	std::vector<Simplex_step<T>> steps;

	Simplex_window(std::vector<T> target, std::vector<T>& basis, Mat<T>& limits, int type)
	{
		init(L"simplex methid", 800, 600, [](HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, Args args)->LRESULT
			{
				Simplex_window* window = (Simplex_window*)args[0];

				switch (msg)
				{
				case WM_CLOSE:
				{
					safe_release(window);
				}return 0;
				}

				return DefWindowProc(hwnd, msg, wParam, lParam);
			});

		set_min_max_size(800, 600);
		table.init(getHWND(), 0, 0, 0, 0.7f, 1.0f, RESIZABLE);

		// make min promblem
		if (type)
		{
			for (int i = 0; i < target.size(); i++)
				target[i] = -target[i];
		}


		for (int i = 0; i < basis.size(); i++)
		{
			if (basis[i] != 0) break;
			if (i == basis.size() - 1);
			// TODO: Call the basis problem solver
		}

		steps.push_back(Simplex_zero_step(target, basis, limits));
	}


	// ======== Layout ==========
	

	// Prepare Simplex table from input params
	Simplex_step<T> Simplex_zero_step(std::vector<T>& target, std::vector<T>& basis, Mat<T> limits)
	{
		Mat<T> simplex_mat(limits.row + 2, limits.column - limits.row + 1);

		std::vector<int> indices(limits.column - 1); // limits indices (x1, x2 ...)
		for (int i = 0; i < indices.size(); i++)
			indices[i] = i;

		// TODO: add exception handle => Basis's vectors amount must be equal limits amount

		int basis_num = 0;
		for (int i = 0; i < basis.size(); i++)
		{
			if (basis[i] != 0)
				change_colums(limits, indices, basis_num++, i);
		}

		Mat<T> temp = limits;
		auto [flag, res] = gausian_method(temp);


		//make simplex mat
		for (int i = 0; i < limits.row; i++)
			simplex_mat[i + 1][0] = indices[i];

		mat_output(simplex_mat);

		for (int i = 0; i < indices.size() - limits.row; i++)
			simplex_mat[0][i + 1] = indices[limits.row + i];

		mat_output(simplex_mat);


		for (int i = 0; i < temp.row; i++)
			for (int j = 0; j < temp.column - temp.row; j++)
				simplex_mat[i + 1][j + 1] = temp[i][j + temp.row];

		mat_output(simplex_mat);



		// calculate target coefs in simplex table
		std::vector<int> coefs(limits.row);
		for (int i = 0; i < limits.row; i++)
			coefs[i] = target[simplex_mat[0][1 + i]];

		for (int i = 1; i < simplex_mat.column - 1; i++)
		{
			T target_coef = T();
			for (int j = 1; j < simplex_mat.row -1; j++)
				target_coef += simplex_mat[i][j] * coefs[j-1];
			
			simplex_mat[simplex_mat.row - 1][i] = target_coef;
		}

		// calculate target value in simplex table
		T c = T();
		for (int i = 1; i < simplex_mat.row - 1; i++)
		{
			c += simplex_mat[i][simplex_mat.column - 1];
		}
		simplex_mat[simplex_mat.row - 1][simplex_mat.column - 1] = c + target[target.size() - 1];

		mat_output(simplex_mat);



		return Simplex_step<T>();
	}

	void change_colums(Mat<T>& mat, std::vector<int>& indices, int c1, int c2)
	{
		std::swap(indices[c1], indices[c2]);
		for (int i = 0; i < mat.row; i++)
			std::swap(mat[i][c1], mat[i][c2]);
	}


	// Make a simplex step from last state
	Simplex_step<T> make_step(Simplex_step<T>& privius_step)
	{

	}



};
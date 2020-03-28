#include <wchar.h>

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


struct pivot { int x, y; };

template <typename T>
struct Simplex_step
{
	int iteration = 0;

	std::vector<pivot> pivots;

	Mat<T> mat; // simplex mat

	T* operator[] (int ind)
	{
		assert(ind < mat.row);
		return mat[ind];
	}

	int cols() { return mat.column; }
	int rows() { return mat.row; }
};


template <typename T>
struct Simplex_window : Window
{
	ListView table;
	std::vector<Simplex_step<T>> steps;

	Label lPivot;
	ComboBox cPivot;

	Button bNext;
	Button bPriv;

	Simplex_window(std::vector<T> target, std::vector<T>& basis, Mat<T>& limits, int type)
	{
		init(L"simplex method", 800, 600, [](HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, Args args)->LRESULT
			{
				Simplex_window* window = (Simplex_window*)args[0];

				switch (msg)
				{
					case WM_COMMAND:
					{
						if (LOWORD(wParam) == window->bNext.id)
							window->make_step();
						else if (LOWORD(wParam) == window->bPriv.id)
							window->remove_step();

					}return 0;
					case WM_CLOSE:
					{
						safe_release(window);
					}return 0;
				}

				return DefWindowProc(hwnd, msg, wParam, lParam);
			});

		set_min_max_size(800, 600);

		// gui components
		table.init(getHWND(), 0, 0, 0, 0.7f, 1.0f, RESIZABLE);
		table.add_columns(std::vector<std::wstring>(limits.column + 2));

		lPivot.init(getHWND(), L"Pivot choosed", 0, 0.7f, 0.3f, 0.2f, 0.05f, RESIZABLE);
		set_font_size(lPivot.handle, 25);
		//tPivot_x.init(getHWND(), 0, 0.7f, 0.35f, 0.15f, 0.05f, RESIZABLE, DEF_EDIT | ES_CENTER | ES_NUMBER);
		//tPivot_y.init(getHWND(), 0, 0.85f, 0.35f, 0.15f, 0.05f, RESIZABLE, DEF_EDIT | ES_CENTER | ES_NUMBER);
		cPivot.init(getHWND(), 0, 0.7f, 0.35f, 0.15f, 0.05f, RESIZABLE);

		bPriv.init(getHWND(), L"Privius", 100, 0.7f, 0.4f, 0.15f, 0.05f, RESIZABLE);
		bNext.init(getHWND(), L"Next", 101, 0.85f, 0.4f, 0.15f, 0.05f, RESIZABLE);


		// make min if it's max
		if (type)
		{
			for (int i = 0; i < target.size(); i++)
				target[i] = -target[i];
		}

		for (int i = 0; i < basis.size(); i++)
		{
			if (basis[i] != 0) break;
			if (i == basis.size() - 1);
			// TODO: Call the artificiant basis problem solver
		}


		Simplex_zero_step(target, basis, limits);
	}


	// ======== Layout ==========
	
	// Prepare Simplex table from input params
	void Simplex_zero_step(std::vector<T>& target, std::vector<T>& basis, Mat<T> limits)
	{
		Simplex_step<T> first_step;
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
		//auto [flag, res] = gausian_method(temp);
		gausian_method(temp);

		mat_output(temp);

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
			coefs[i] = target[simplex_mat[1 + i][0]];

		for (int i = 1; i < simplex_mat.column - 1; i++)
		{
			T target_coef = T();
			for (int j = 1; j < simplex_mat.row -1; j++)
				target_coef -= simplex_mat[j][i] * coefs[j-1];
			
			simplex_mat[simplex_mat.row - 1][i] = target_coef + target[simplex_mat[0][i]];
		}

		mat_output(simplex_mat);


		// calculate target value in simplex table
		T c = T();
		for (int i = 1; i < simplex_mat.row - 1; i++)
		{
			c += simplex_mat[i][simplex_mat.column - 1] * target[simplex_mat[0][i]];
		}
		simplex_mat[simplex_mat.row - 1][simplex_mat.column - 1] = -(c + target[target.size() - 1]);

		mat_output(simplex_mat);

		// find pivot
		first_step.mat = std::move(simplex_mat);
		find_pivots(first_step);

		steps.push_back(first_step);

		mat_output(first_step.mat);

		dump_steps();
	}

	void change_colums(Mat<T>& mat, std::vector<int>& indices, int c1, int c2)
	{
		std::swap(indices[c1], indices[c2]);
		for (int i = 0; i < mat.row; i++)
			std::swap(mat[i][c1], mat[i][c2]);
	}


	void find_pivots(Simplex_step<T>& step)
	{
		int target_row = step.rows() - 1;

		for (int i = 1; i < step.cols() - 1; i++)
		{
			if (step[target_row][i] > -0.001f) continue;

			T min = MAXINT32;
			int x = -1, y = -1;
			for (int j = 1; j < step.rows() - 1; j++)
			{
				T temp = step[j][step.cols() - 1] / step[j][i];
				if (temp > 0 && temp <= min)
				{
					min = temp;
					x = i;
					y = j;
				}
			}
			if (x != -1)
				step.pivots.push_back({ x, y });
		}
		
	}

	// Make a simplex step from last state
	void make_step()
	{
		Simplex_step<T>& priv_step = steps.back();
		Simplex_step<T> step = priv_step;
		step.iteration++;
		step.pivots.clear();

		int ind = cPivot.choosed_index();
		if (ind < 0) return;
		pivot coord = priv_step.pivots[ind];
		T piv = step[coord.y][coord.x];

		// swap basis and free variables
		std::swap(step[0][coord.x], step[coord.y][0]);

		// flip the pivot
		step[coord.y][coord.x] = 1 / step[coord.y][coord.x];

		// divide by pivot
		for (int i = 1; i < step.cols(); i++)
			if (i != coord.x) step[coord.y][i] = step[coord.y][i] / piv;

		// divide by -pivot
		for (int i = 1; i < step.rows(); i++)
			if (i != coord.y) step[i][coord.x] = -step[i][coord.x] / piv;


		// substract rows with coef
		for (int i = 1; i < step.rows(); i++)
		{
			if (coord.y == i) continue;
			
			T coef = priv_step[i][coord.x];
			for (int j = 1; j < step.cols(); j++)
			{
				if (j == coord.x) continue;
				step[i][j] = step[i][j] - coef * step[coord.y][j];
			}
		}

		mat_output(step.mat);
		
		
		find_pivots(step);
		steps.push_back(step);

		table.clear();
		dump_steps();
	}


	void remove_step()
	{
		if (steps.size() == 1) return;
		steps.erase(steps.end()-1);
		table.clear();
		dump_steps();
	}


	// out information on the screen
	void dump_steps()
	{
		wchar_t buffer[32];
		
		for (Simplex_step<T>& step : steps)
		{
			mat_output(step.mat);

			for (int y = 0; y < step.rows(); y++)
			{
				std::vector<std::wstring> row;
				for (int x = 0; x < step.cols(); x++)
				{
					if (x == 0 && y == 0)
					{
						to_str(buffer, step.iteration);
						row.push_back(buffer);
						continue;
					}
					if (x == step.cols() - 1 && y == 0) continue;
					if (x == 0 && y == step.rows() - 1) { row.push_back(L""); continue; }

					if (x == 0 || y == 0)
					{
						wchar_t x_buffer[32] = L"X";

						to_str(buffer, (int)step[y][x]);
						wcscat_s(x_buffer, buffer);
						row.push_back(x_buffer);
					}
					else
					{
						to_str(buffer, step[y][x]);
						row.push_back(buffer);
					}
				}
				table.add_row(row);
			}
			table.space(2);
		}

		// set pivots
		cPivot.clear();
		for (pivot p : steps.back().pivots)
		{
			swprintf_s(buffer, L"(x - %d, y - %d)", p.x, p.y);
			cPivot.add(buffer);
		}
	}

};

template<typename T>
void mat_output(Mat<T>& mat)
{
#if _DEBUG
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
#endif
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
	Button bAuto;

	// amount of variables
	int vars = 0;

	Simplex_window(std::vector<T> target, std::vector<T>& basis, Mat<T>& limits, int type)
	{
		init(L"симплекс метод", 800, 600, [](HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, Args args)->LRESULT
			{
				Simplex_window* window = (Simplex_window*)args[0];

				switch (msg)
				{
					case WM_NOTIFY:
					{
						if (((LPNMHDR)lParam)->code == NM_CLICK && ((LPNMHDR)lParam)->hwndFrom == window->table.handle)
						{
							int col = ((LPNMLISTVIEW)lParam)->iSubItem;
							int row = ((LPNMLISTVIEW)lParam)->iItem;

							// find col and row in current table on the screen
							Simplex_step<T>& last_step = window->steps.back();
							int last_row = window->table.rows();
							row = row - (last_row - last_step.rows()) + 1;

							// find pivot in existed
							pivot* p = NULL;
							int ind = 0;
							for (auto& pivot : last_step.pivots)
							{
								if (pivot.x == col && pivot.y == row)
								{
									p = &pivot;
									break;
								}
								ind += 1;
							}

							if (p != NULL)
								window->cPivot.set_selected(ind);

							doutput("%d  %d\n", row, col);
						}
					}break;
					case WM_COMMAND:
					{
						if (LOWORD(wParam) == window->bNext.id)
							window->make_step();
						else if (LOWORD(wParam) == window->bPriv.id)
							window->remove_step();
						else if (LOWORD(wParam) == window->bAuto.id)
							window->auto_end();
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

		lPivot.init(getHWND(), L"Опорный", 0, 0.7f, 0.3f, 0.2f, 0.05f, RESIZABLE);
		set_font_size(lPivot.handle, 25);
		cPivot.init(getHWND(), 0, 0.7f, 0.35f, 0.15f, 0.05f, RESIZABLE);

		bPriv.init(getHWND(), L"Предидущий", 100, 0.7f, 0.4f, 0.15f, 0.05f, RESIZABLE);
		bNext.init(getHWND(), L"Следующий", 101, 0.85f, 0.4f, 0.15f, 0.05f, RESIZABLE);
		bAuto.init(getHWND(), L"Авто", 102, 0.7f, 0.45f, 0.15f, 0.05f, RESIZABLE);


		// make min if it's max
		if (type)
		{
			for (int i = 0; i < target.size(); i++)
				target[i] = -target[i];
		}

		vars = limits.column - 1;
		
		// amount of basis variables
		int amount = 0;
		for (int i = 0; i < basis.size(); i++)
			if (basis[i] > 0) amount++;

		if (amount != limits.row)
			if (!calculate_basis(target, basis, limits))
				return;


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
			c += simplex_mat[i][simplex_mat.column - 1] * target[simplex_mat[i][0]];
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

			T min = 10000;
			int x = -1, y = -1;
			for (int j = 1; j < step.rows() - 1; j++)
			{
				T temp = step[j][step.cols() - 1] / step[j][i];
				if (temp <= min && step[j][i] > 0)
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
	void make_step(bool auto_flag = false)
	{
		if (steps.size() == 0) return;
		Simplex_step<T>& priv_step = steps.back();
		Simplex_step<T> step = priv_step;
		step.iteration++;
		step.pivots.clear();

		int ind = auto_flag ? 0 : cPivot.choosed_index();
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
		if (steps.size() <= 1) return;
		steps.erase(steps.end()-1);
		table.clear();
		dump_steps();
	}

	void auto_end()
	{
		if (steps.size() < 1) return;
		while (steps.back().pivots.size())
			make_step(true);

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
			swprintf_s(buffer, L"col %d, row %d)", p.x, p.y);
			cPivot.add(buffer);
		}
	}


	// ==================  if basis is zeroes lets calculate it =======================

	bool calculate_basis(std::vector<T>& target, std::vector<T>& basis, Mat<T>& limits)
	{
		Simplex_step<T> step = bSimplex_zero_step(target, basis, limits);

		while (step.pivots.size() != 0)
			step = bmake_step(step);

		for (int i = 0; i < basis.size(); i++)
			basis[i] = T();

		for (int i = 1; i < step.rows() - 1; i++)
		{
		//	assert((int)step[i][0] < basis.size());
			if ((unsigned)step[i][0] >= basis.size()) return false;
			basis[(int)step[i][0]] = 1;
		}
		return true;
	}


	// Prepare Simplex table from input params
	Simplex_step<T> bSimplex_zero_step(std::vector<T>& target, std::vector<T>& basis, Mat<T> limits)
	{
		Simplex_step<T> first_step;
		Mat<T> simplex_mat(limits.row + 2, limits.column + 1);

		// put coefs into the simplex table
		for (int i = 0; i < limits.row; i++)
			for (int j = 0; j < limits.column; j++)
				simplex_mat[i + 1][j + 1] = limits[i][j];

		// free
		for (int i = 0; i < limits.column - 1; i++)
			simplex_mat[0][i + 1] = i;

		// basis
		for (int i = 0; i < limits.row; i++)
			simplex_mat[i + 1][0] = limits.column + i - 1;

		mat_output(simplex_mat);

		// add target line
		for (int i = 1; i < simplex_mat.column; i++)
		{
			T c = T();
			for (int j = 1; j < simplex_mat.row - 1; j++)
				c -= simplex_mat[j][i];

			simplex_mat[simplex_mat.row - 1][i] = c;
		}

		mat_output(simplex_mat);

		// find pivot
		first_step.mat = std::move(simplex_mat);
		find_pivots(first_step);

		mat_output(first_step.mat);

		return first_step;
	}


	// Make a simplex step from last state
	Simplex_step<T> bmake_step(Simplex_step<T>& priv_step)
	{
		Simplex_step<T> step = priv_step;
		step.iteration++;
		step.pivots.clear();

		pivot coord = priv_step.pivots[0];
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

		// delete column if it's artificial
		for (int i = 1; i < step.cols(); i++)
		{
			if (step[0][i] >= vars)
			{
				for (int a = i; a < step.cols() - 1; a++)
					for (int b = 0; b < step.rows(); b++)
						step[b][a] = step[b][a + 1];
				step.mat.column--;
				break;
			}
		}

		mat_output(step.mat);

		find_pivots(step);
		return step;
	}

};
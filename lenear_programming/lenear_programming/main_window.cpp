

struct Main_window : Window
{
	Label lMethod;
	ComboBox cMethod;
	
	Label lMin_max;
	ComboBox cMin_max;

	Label lNumbers_type;
	ComboBox cNumbers_type;

	Label lVars;
	Text tVars;

	Label lLimits;
	Text tLimits;

	Label lTarget;
	Table target;

	Label Limits;
	Table limits;

	Label lBasis;
	Table basis;

	int nVars = 0, nLimits = 0;

#define BTN_SOLVE 100
	Button bSolve;
#define BTN_CLEAR 101
	Button bClear;


	
	Main_window()
	{
		// window init
		init(L"linear programming", 1100, 600, [](HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, Args args)->LRESULT
			{
				Main_window* window = (Main_window*)args[0];

				switch (msg)
				{
					case WM_COMMAND:
					{
						if (LOWORD(wParam) == BTN_SOLVE)
						{
							std::vector<TCHAR*> limits_text = window->limits.get_data();
							std::vector<TCHAR*> basis_text = window->basis.get_data();
							std::vector<TCHAR*> target_text = window->target.get_data();

							if (window->cNumbers_type.choosed_index() == 0)
							{
								std::vector<float> target = window->text2float(target_text);
								std::vector<float> basis = window->text2float(basis_text);
								Mat<float> limits = window->vec2mat(window->text2float(limits_text), window->nVars, window->nLimits);

								if (window->cMethod.choosed_index() == 0)
								{
									if (window->nVars != 2) 
										MessageBox(window->getHWND(), L"Problem can be sove only for two variables", L"Message", MB_OK);
									else
										new Graph_window(target, limits, window->cMin_max.choosed_index());
								}
								else
									new Simplex_window(target, basis, limits, window->cMin_max.choosed_index());
							}
							else
							{
								std::vector<Fraction> target = window->text2fraction(target_text);
								std::vector<Fraction> basis = window->text2fraction(basis_text);
								Mat<Fraction> limits = window->vec2mat(window->text2fraction(limits_text), window->nVars, window->nLimits);
								
								// graphic method
								if (window->cMethod.choosed_index() == 0)
								{
									if (window->nVars != 2)
										MessageBox(window->getHWND(), L"Problem can be sove only for two variables", L"Message", MB_OK);
									else
										new Graph_window(target, limits, window->cMin_max.choosed_index());
								}
								// simplex method
								else
									new Simplex_window(target, basis, limits, window->cMin_max.choosed_index());
							}
						}
						// Button clear pushed
						if (LOWORD(wParam) == BTN_CLEAR)
						{
							window->limits.clear();
							window->basis.clear();
							window->target.clear();
						}

						// Data was put into the amount gap (Vars or limits)
						if ((HWND)lParam == window->tVars.handle || (HWND)lParam == window->tLimits.handle)
						{
							if (HIWORD(wParam) == 1024)
							{
								int columns = wtoi(window->tVars.getText());
								int rows = wtoi(window->tLimits.getText());
								window->show_fields(columns, rows);
								if (columns < 16)
									window->nVars = columns;
								if (rows < 16)
									window->nLimits = rows;
							}
						}
					}break;
					case WM_CLOSE:
					{
						PostQuitMessage(0);
					}return 0;

				}

				return DefWindowProc(hwnd, msg, wParam, lParam);
			});

		// set gui elements
		set_min_max_size(1100, 600, 1100, 600);
		UINT lStyle = WS_CHILD | WS_VISIBLE | SS_CENTER;
		UINT cStyle = CBS_DROPDOWN | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE;
		UINT tStyle = WS_VISIBLE | WS_CHILD | WS_BORDER | ES_CENTER | ES_NUMBER;

		lMethod.init(getHWND(), L"Method", 0, 0, 0.05f, 0.2f, 0.05f, RESIZABLE, lStyle);
		set_font_size(lMethod.handle, 25);
		
		cMethod.init(getHWND(), 1, 0.05f, 0.1f, 0.15f, 0.05f, RESIZABLE, cStyle);
		cMethod.add(L"graphic");
		cMethod.add(L"simplex");
		
		
		lMin_max.init(getHWND(), L"Type", 0, 0, 0.2f, 0.2f, 0.05f, RESIZABLE, lStyle);
		set_font_size(lMin_max.handle, 25);
		
		cMin_max.init(getHWND(), 1, 0.05f, 0.25f, 0.15f, 0.05f, RESIZABLE, cStyle);
		cMin_max.add(L"min");
		cMin_max.add(L"max");
		
		lNumbers_type.init(getHWND(), L"Numbers type", 0, 0, 0.35f, 0.2f, 0.05f, RESIZABLE, lStyle);
		set_font_size(lNumbers_type.handle, 25);
		
		cNumbers_type.init(getHWND(), 1, 0.05f, 0.4f, 0.15f, 0.05f, RESIZABLE, cStyle);
		cNumbers_type.add(L"decimal");
		cNumbers_type.add(L"fraction");
		
		
		lVars.init(getHWND(), L"Variabals amount", 0, 0, 0.5f, 0.2f, 0.05f, RESIZABLE, lStyle);
		set_font_size(lVars.handle, 23);
		tVars.init(getHWND(), 0, 0.05f, 0.55f, 0.15f, 0.05f, RESIZABLE, tStyle);
		
		lLimits.init(getHWND(), L"Limits amount", 0, 0, 0.6f, 0.2f, 0.05f, RESIZABLE, lStyle);
		set_font_size(lLimits.handle, 25);
		tLimits.init(getHWND(), 0, 0.05f, 0.65f, 0.15f, 0.05f, RESIZABLE, tStyle);
		
		bSolve.init(getHWND(), L"Solve", BTN_SOLVE, 0.01f, 0.75f, 0.19f, 0.2f, RESIZABLE);
		set_font_size(bSolve.handle, 25);

		bClear.init(getHWND(), L"clear", BTN_CLEAR, 0.01f, 0.95f, 0.1f, 0.05f, RESIZABLE);


		// create input fileds table
		Limits.init(getHWND(), L"Limits", 0, 0.2f, 0.15f, 0.1f, 0.05f);
		set_font_size(Limits.handle, 25);

		limits.init(getHWND(), 16, 17, 0.2f, 0.2f, 0.0445f, 0.035f, CELL_SIZE);

		lTarget.init(getHWND(), L"Target function", 0, 0.2f, 0.0f, 0.1f, 0.05f);
		set_font_size(lTarget.handle, 25);

		target.init(getHWND(), 1, 17, 0.2f, 0.05f, 0.0445f, 0.035f, CELL_SIZE);

		lBasis.init(getHWND(), L"Basis", 0, 0.2f, 0.85f, 0.1f, 0.05f);
		set_font_size(lBasis.handle, 25);

		basis.init(getHWND(), 1, 17, 0.2f, 0.9f, 0.0445f, 0.035f, CELL_SIZE);
	}

// ============== Layout =================

	// ======= Input fields table setup =========
	void show_fields(int x, int y)
	{
		if (x > 16 || y > 16) return;

		std::vector<std::wstring> rows;
		std::vector<std::wstring> cols;
		cols.reserve(x);
		rows.reserve(y);

		wchar_t buffer[5];

		for (int i = 0; i < x; i++)
		{
			swprintf_s(buffer, L"x%d", i);
			cols.push_back(buffer);
		}
		swprintf_s(buffer, L"b");
		cols.push_back(buffer);


		for (int i = 0; i < y; i++)
		{
			swprintf_s(buffer, L"%d", i);
			rows.push_back(buffer);
		}

		limits.create(rows, cols);

		cols.back()[0] = 'c';
		target.create(std::vector<std::wstring>(), cols);

		cols.erase(cols.end() - 1);
		basis.create(std::vector<std::wstring>(), cols);
	}

	
	// ====== Parse data ======
	std::vector<float> text2float(const std::vector<TCHAR*>& text)
	{
		std::vector<float> output;
		output.reserve(text.size());

		for (int i = 0; i < text.size(); i++)
			output.push_back(wtof(text[i]));

		return output;
	}

	std::vector<Fraction> text2fraction(const std::vector<TCHAR*>& text)
	{
		std::vector<Fraction> output;
		output.reserve(text.size());

		for (int i = 0; i < text.size(); i++)
		{
			Fraction fraction;
			int res = 0;
			if (text[i]) 
				res = swscanf_s(text[i], L"%d/%d", &fraction.top, &fraction.bottom);
			switch (res)
			{
				case 0:
				{
					output.push_back(Fraction());
				}break;
				case 1:
				{
					output.push_back(Fraction(fraction.top, 1));
				}break;
				case 2:
				{
					output.push_back(fraction);
				}break;
			}
		}

		return output;
	}


	template <typename T>
	Mat<T> vec2mat(const std::vector<T>& data, int cols, int rows)
	{
		cols++;
		Mat<T> mat(rows, cols);
		for (int i = 0; i < rows; i++)
			for (int j = 0; j < cols; j++)
				mat[i][j] = data[i * cols + j];
		return mat;
	}

};
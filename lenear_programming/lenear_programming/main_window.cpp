#include <filesystem>
#include <ctime>

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
					case WM_CREATE:
					{
						HMENU hMenuBar = CreateMenu();
						HMENU hFileMenu = CreateMenu();

						AppendMenu(hFileMenu, MF_STRING, 301, L"open");
						AppendMenu(hFileMenu, MF_STRING, 302, L"save");

						AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR)hFileMenu, L"File");
						AppendMenu(hMenuBar, MF_STRING, 303, L"About");

						SetMenu(hwnd, hMenuBar);
					}break;
					case WM_COMMAND:
					{
						// menu
						if (LOWORD(wParam) == 301)
						{
							window->open();
							break;
						}

						if (LOWORD(wParam) == 302)
						{
							window->save_to_file();
							break;
						}

						if (LOWORD(wParam) == 303)
						{
							new About_window();
							break;
						}

						// interface handle
						if (LOWORD(wParam) == BTN_SOLVE)
						{
							if (window->nVars < 2)
							{
								MessageBox(window->getHWND(), L"Few variables", L"Message", MB_OK);
								break;
							}

							if (window->nLimits < 1)
							{
								MessageBox(window->getHWND(), L"Few limits", L"Message", MB_OK);
								break;
							}

							std::vector<TCHAR*> limits_text = window->limits.get_data();
							std::vector<TCHAR*> basis_text = window->basis.get_data();
							std::vector<TCHAR*> target_text = window->target.get_data();

							
							if (window->cNumbers_type.choosed_index() == 0)
							{
								std::vector<float> target = window->text2float(target_text);
								std::vector<float> basis = window->text2float(basis_text);
								Mat<float> limits = window->vec2mat(window->text2float(limits_text), window->nVars, window->nLimits);

								//std::vector<float> target{1, 1, 1, 0};
								//std::vector<float> basis{0, 1, 1};
								//Mat<float> limits(2, 4);
								//limits[0][0] = -1; limits[0][1] = 1; limits[0][2] = 1; limits[0][3] = 2;
								//limits[1][0] = 3; limits[1][1] = -1; limits[1][2] = 1; limits[1][3] = 0;

								//std::vector<float> target{-2, -1, -3, -1};
								//std::vector<float> basis{0, 0, 1, 1};
								//Mat<float> limits(2, 5);
								//limits[0][0] = 1; limits[0][1] = 2; limits[0][2] = 5; limits[0][3] = -1; limits[0][4] = 4;
								//limits[1][0] = 1; limits[1][1] = -1; limits[1][2] = -1; limits[1][3] = 2; limits[1][4] = 1;


								//std::vector<float> target{-3, -2, 0};
								//std::vector<float> basis{0, 0};
								//Mat<float> limits(3, 3);
								//limits[0][0] = 1; limits[0][1] = 2; limits[0][2] = 7;
								//limits[1][0] = 2; limits[1][1] = 1; limits[1][2] = 8;
								//limits[2][0] = 0; limits[2][1] = 1; limits[2][2] = 3;


								int type = window->cMethod.choosed_index();
								if (type == 0)
								{
									if (window->nVars != 2)
										MessageBox(window->getHWND(), L"Problem can be sove only for two variables", L"Message", MB_OK);
									else
										new Graph_window(target, limits, window->cMin_max.choosed_index());
								}
								else if (type == 1)
									new Simplex_window(target, basis, limits, window->cMin_max.choosed_index());
								else
									new Artificiant_basis_window(target, basis, limits, window->cMin_max.choosed_index());
							}
							else
							{
								std::vector<Fraction> target = window->text2fraction(target_text);
								std::vector<Fraction> basis = window->text2fraction(basis_text);
								Mat<Fraction> limits = window->vec2mat(window->text2fraction(limits_text), window->nVars, window->nLimits);

								//std::vector<Fraction> target{ 1, 1, 1, 0 };
								//std::vector<Fraction> basis{ 0, 1, 1 };
								//Mat<Fraction> limits(2, 4);
								//limits[0][0] = -1; limits[0][1] = 1; limits[0][2] = 1; limits[0][3] = 2;
								//limits[1][0] = 3; limits[1][1] = -1; limits[1][2] = 1; limits[1][3] = 0;

								//std::vector<Fraction> target{ -2, -1, -3, -1 };
								//std::vector<Fraction> basis{ 0, 0, 1, 1 };
								//Mat<Fraction> limits(2, 5);
								//limits[0][0] = 1; limits[0][1] = 2; limits[0][2] = 5; limits[0][3] = -1; limits[0][4] = 4;
								//limits[1][0] = 1; limits[1][1] = -1; limits[1][2] = -1; limits[1][3] = 2; limits[1][4] = 1;

								//std::vector<Fraction> target{ -3, -2, 0 };
								//std::vector<Fraction> basis{ 0, 0 };
								//Mat<Fraction> limits(3, 3);
								//limits[0][0] = 1; limits[0][1] = 2; limits[0][2] = 7;
								//limits[1][0] = 2; limits[1][1] = 1; limits[1][2] = 8;
								//limits[2][0] = 0; limits[2][1] = 1; limits[2][2] = 3;
								
								// graphic method
								int type = window->cMethod.choosed_index();
								if (type == 0)
								{
									if (window->nVars != 2)
										MessageBox(window->getHWND(), L"Problem can be solve only for two variables", L"Message", MB_OK);
									else
										new Graph_window(target, limits, window->cMin_max.choosed_index());
								}
								// simplex method
								else if (type == 1)
									new Simplex_window(target, basis, limits, window->cMin_max.choosed_index());
								else
									new Artificiant_basis_window(target, basis, limits, window->cMin_max.choosed_index());
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
		cMethod.add(L"artificial basis");

		
		
		lMin_max.init(getHWND(), L"Type", 0, 0, 0.2f, 0.2f, 0.05f, RESIZABLE, lStyle);
		set_font_size(lMin_max.handle, 25);
		
		cMin_max.init(getHWND(), 1, 0.05f, 0.25f, 0.15f, 0.05f, RESIZABLE, cStyle);
		cMin_max.add(L"min");
		cMin_max.add(L"max");
		
		lNumbers_type.init(getHWND(), L"Numbers type", 0, 0, 0.35f, 0.2f, 0.05f, RESIZABLE, lStyle);
		set_font_size(lNumbers_type.handle, 25);
		
		cNumbers_type.init(getHWND(), 1, 0.05f, 0.4f, 0.15f, 0.05f, RESIZABLE, cStyle);
		cNumbers_type.add(L"floating point");
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



	// ========= File operations ==============

	void open()
	{
		wchar_t file_name[128];
		wchar_t buff[32];

		OPENFILENAME ofn;
		ZeroMemory(&ofn, sizeof(OPENFILENAME));

		ofn.lStructSize= sizeof(OPENFILENAME);
		ofn.hwndOwner = getHWND();
		ofn.lpstrFile = file_name;
		file_name[0] = L'\0';
		ofn.nMaxFile = 128;
		ofn.lpstrFilter = L"All Files\0*.*txt\0";
		ofn.nFilterIndex = 1;

		GetOpenFileName(&ofn);
		if (wcsnlen_s(file_name, 128) == 0)	return;

		//MessageBox(NULL, ofn.lpstrFile, L"", MB_OK);

		int file_size;
		wchar_t* data = open_file(file_name, file_size);
		if (data == NULL) return;

		int method, 
			problem_type,
			number_type, 
			vars, lims;
		
		int seek = 0, temp;
		swscanf_s(data, L"%d%n", &method, &temp);
		seek += temp;
		swscanf_s(data + seek, L"%d%n", &problem_type, &temp);
		seek += temp;
		swscanf_s(data + seek, L"%d%n", &number_type, &temp);
		seek += temp;

		swscanf_s(data + seek, L"%d%n", &vars, &temp);
		seek += temp;
		swscanf_s(data + seek, L"%d%n", &lims, &temp);
		seek += temp;
		
		// set program params 
		cMethod.set_selected(method);
		cMin_max.set_selected(problem_type);
		cNumbers_type.set_selected(number_type);
		
		to_str(buff, vars);
		tVars.SetText(buff);
		to_str(buff, lims);
		tLimits.SetText(buff);
		nVars = vars;
		nLimits = lims;


		show_fields(vars, lims);

		if (number_type == 0)
		{
			float in = 0.0f;
			
			// parse target float
			for (int i = 0; i < vars + 1; i++)
			{
				swscanf_s(data + seek, L"%f%n", &in, &temp);
				seek += temp;
				to_str(buff, in);
				target[0][i].SetText(buff);
			}

			// parse limits
			for (int i = 0; i < lims; i++)
			{
				for (int j = 0; j < vars + 1; j++)
				{
					swscanf_s(data + seek, L"%f%n", &in, &temp);
					seek += temp;
					to_str(buff, in);
					limits[i][j].SetText(buff);
				}
			}

			// parse basis
			for (int i = 0; i < vars; i++)
			{
				swscanf_s(data + seek, L"%f%n", &in, &temp);
				seek += temp;
				to_str(buff, in);
				basis[0][i].SetText(buff);
			}
		}
		else
		{
			Fraction in;

			// parse target float
			for (int i = 0; i < vars; i++)
			{
				swscanf_s(data + seek, L"%d/%d%n", &in.top, &in.bottom, &temp);
				seek += temp;
				to_str(buff, in);
				target[0][i].SetText(buff);
			}

			// parse limits
			for (int i = 0; i < lims; i++)
			{
				for (int j = 0; j < vars; j++)
				{
					swscanf_s(data + seek, L"%d/%d%n", &in.top, &in.bottom, &temp);
					seek += temp;
					to_str(buff, in);
					limits[i][j].SetText(buff);
				}
			}

			// parse basis
			for (int i = 0; i < vars; i++)
			{
				swscanf_s(data + seek, L"%d/%d%n", &in.top, &in.bottom, &temp);
				seek += temp;
				to_str(buff, in);
				basis[0][i].SetText(buff);
			}
		}

		delete[] data;
	}


	void save_to_file()
	{
		if (nVars < 1 || nLimits < 1) return;

		wchar_t buffer[512];
		int ind = 0;

		ind += swprintf_s(buffer, L"%d\n", cMethod.choosed_index());
		ind += swprintf_s(buffer + ind, 5, L"%d\n", cMin_max.choosed_index());
		ind += swprintf_s(buffer + ind, 5, L"%d\n", cNumbers_type.choosed_index());

		ind += swprintf_s(buffer + ind, 5, L"%d\n", nVars);
		ind += swprintf_s(buffer + ind, 5, L"%d\n", nLimits);


		std::vector<TCHAR*> limits_text = limits.get_data();
		std::vector<TCHAR*> basis_text = basis.get_data();
		std::vector<TCHAR*> target_text = target.get_data();

		if (cNumbers_type.choosed_index() == 0)
		{
			std::vector<float> target = text2float(target_text);
			std::vector<float> basis = text2float(basis_text);
			std::vector<float> limits = text2float(limits_text);
			
			// write target funtion for float
			for (int i = 0; i < nVars + 1; i++)
				ind += swprintf_s(buffer + ind, 10, L"%0.1f ", target[i]);

			// some gaps
			ind += swprintf_s(buffer + ind, 3, L"\n\n");

			// limits
			for (int i = 0; i < nLimits; i++)
			{
				for (int j = 0; j < nVars + 1; j++)
					ind += swprintf_s(buffer + ind, 10, L"%0.1f ", limits[i * (nVars + 1) + j]);
				ind += swprintf_s(buffer + ind, 3, L"\n");
			}

			ind += swprintf_s(buffer + ind, 3, L"\n");

			// basis
			for (int i = 0; i < nVars; i++)
				ind += swprintf_s(buffer + ind, 10, L"%0.1f ", basis[i]);

		}
		else
		{
			std::vector<Fraction> target = text2fraction(target_text);
			std::vector<Fraction> basis = text2fraction(basis_text);
			std::vector<Fraction> limits = text2fraction(limits_text);

			// write target funtion for Fraction
			for (int i = 0; i < nVars + 1; i++)
				ind += swprintf_s(buffer + ind, 10, L"%d/%d ", target[i].top, target[i].bottom);

			// some gaps
			ind += swprintf_s(buffer + ind, 3, L"\n\n");

			// limits
			for (int i = 0; i < nLimits; i++)
			{
				for (int j = 0; j < nVars + 1; j++)
					ind += swprintf_s(buffer + ind, 10, L"%d/%d ", limits[i * (nVars + 1) + j].top, limits[i * (nVars + 1) + j].bottom);
				ind += swprintf_s(buffer + ind, 3, L"\n");
			}

			ind += swprintf_s(buffer + ind, 3, L"\n");

			// basis
			for (int i = 0; i < nVars; i++)
				ind += swprintf_s(buffer + ind, 10, L"%d/%d ", basis[i].top, basis[i].bottom);
		}


		// get current time for name 
		time_t rawtime;
		tm timeinfo;
		wchar_t time_buffer[64];
		time(&rawtime);
		localtime_s(&timeinfo, &rawtime);
		wcsftime(time_buffer, sizeof(time_buffer), L"%d-%m-%Y %H_%M_%S", &timeinfo);
		wcscat_s(time_buffer, L".txt");

		write_file(time_buffer, buffer, ind);
	}
};


struct Main_window : Window
{
	Label lMethod;
	ComboBox cMethod;
	
	Label lMin_max;
	ComboBox cMin_max;

	Label lFraction_type;
	ComboBox cFraction_type;

	Label lVars;
	Text tVars;

	Label lLimits;
	Text tLimits;

	Table table;
	
	int nVars = 0, nLimits = 0;
	small::vector<Label> lMarkers;
	small::vector<Text> target_funtion;
	small::vector<small::vector<Text>> limits;

#define BTN_SOLVE 100
	Button bSolve;
	
	Main_window() : limits(1, small::vector<Text>(16)) , lMarkers(16), target_funtion(16)
	{
		// window init
		init(L"linear programming", 900, 600, [](HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, Args args)->LRESULT
			{
				Main_window* window = (Main_window*)args[0];

				switch (msg)
				{
					case WM_COMMAND:
					{
						if (LOWORD(wParam) == BTN_SOLVE)
						{
							static int a = 14;
							window->show_fields(a, a++);
						}

						if ((HWND)lParam == window->tVars.handle || (HWND)lParam == window->tLimits.handle)
							if (HIWORD(wParam) == 1024)
								doutput("lol %d\n", HIWORD(wParam));
					}break;
					case WM_PAINT:
					{
						RECT ClientRect;
						GetClientRect(hwnd, &ClientRect);
						PAINTSTRUCT ps;
						BeginPaint(hwnd, &ps);
						////draw_filled_rect(window->canvas, 0.0f, 0.0f, 1.0f, 1.0f, fColor(0.8f));
						//FillRect(window->hdc, &ClientRect, (HBRUSH)GetStockObject(WHITE_BRUSH));

						EndPaint(hwnd, &ps);
					}return 0;
					case WM_CLOSE:
					{
						PostQuitMessage(0);
					}return 0;

				}

				return DefWindowProc(hwnd, msg, wParam, lParam);
			});

		// set gui elements
		set_min_max_size(900, 600, 900, 600);
		UINT lStyle = WS_CHILD | WS_VISIBLE | SS_CENTER;
		UINT cStyle = CBS_DROPDOWN | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE;
		UINT tStyle = WS_VISIBLE | WS_CHILD | WS_BORDER | ES_CENTER | ES_NUMBER;

		lMethod.init(getHWND(), L"Method", 0, 0, 0.05f, 0.2f, 0.05f, RESIZABLE, lStyle);
		set_font_size(lMethod.handle, 25);
		
		cMethod.init(getHWND(), 1, 0.05f, 0.1f, 0.15f, 0.05f, RESIZABLE, cStyle);
		cMethod.add(L"graph");
		cMethod.add(L"simplex");
		
		
		lMin_max.init(getHWND(), L"Type", 0, 0, 0.2f, 0.2f, 0.05f, RESIZABLE, lStyle);
		set_font_size(lMin_max.handle, 25);
		
		cMin_max.init(getHWND(), 1, 0.05f, 0.25f, 0.15f, 0.05f, RESIZABLE, cStyle);
		cMin_max.add(L"min");
		cMin_max.add(L"max");
		
		lFraction_type.init(getHWND(), L"Fraction type", 0, 0, 0.35f, 0.2f, 0.05f, RESIZABLE, lStyle);
		set_font_size(lFraction_type.handle, 25);
		
		cFraction_type.init(getHWND(), 1, 0.05f, 0.4f, 0.15f, 0.05f, RESIZABLE, cStyle);
		cFraction_type.add(L"decimal");
		cFraction_type.add(L"usual");
		
		
		lVars.init(getHWND(), L"Variabals amount", 0, 0, 0.5f, 0.2f, 0.05f, RESIZABLE, lStyle);
		set_font_size(lVars.handle, 23);
		tVars.init(getHWND(), 0, 0.05f, 0.55f, 0.15f, 0.05f, RESIZABLE, tStyle);
		
		lLimits.init(getHWND(), L"Limits amount", 0, 0, 0.6f, 0.2f, 0.05f, RESIZABLE, lStyle);
		set_font_size(lLimits.handle, 25);
		tLimits.init(getHWND(), 0, 0.05f, 0.65f, 0.15f, 0.05f, RESIZABLE, tStyle);
		
		bSolve.init(getHWND(), L"Solve", BTN_SOLVE, 0.01f, 0.75f, 0.19f, 0.2f, RESIZABLE);
		set_font_size(bSolve.handle, 25);


		// create input fileds
		table.init(getHWND(), 16, 16, 0.2f, 0.2f, 0.8f, 0.6f);
		show_fields(2, 2);

		//std::vector<std::wstring> rows{ L"1", L"2", L"3", L"3", L"1", L"2", L"3", L"3", L"1", L"2", L"3", L"3", L"3", L"1", L"2", L"3" };
		//std::vector<std::wstring> cols{ L"1", L"2", L"3", L"3", L"1", L"2", L"3", L"3", L"1", L"2", L"3", L"3", L"3", L"1", L"2", L"3" };
		//table.create(rows, cols);

	}

	void show_fields(int x, int y)
	{
		std::vector<std::wstring> rows;
		std::vector<std::wstring> cols;
		rows.reserve(x);
		cols.reserve(y);

		wchar_t buffer[5];

		for (int i = 0; i < y; i++)
		{
			swprintf_s(buffer, L"x%d", i);
			cols.push_back(buffer);
		}

		for (int i = 0; i < x; i++)
		{
			swprintf_s(buffer, L"%d", i);
			rows.push_back(buffer);
		}

		table.create(rows, cols);
	}


};
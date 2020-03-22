
template <typename T>
void output(wchar_t buffer[], T a, T b);

template <>
void output<float>(wchar_t buffer[], float a, float b)
{
	swprintf_s(buffer, 32, L"X = (%.4f, %.4f)", a, b);
}

template <>
void output<Fraction>(wchar_t buffer[], Fraction a, Fraction b)
{
	swprintf_s(buffer, 32, L"X = (%d/%d, %d/%d)", a.top, a.bottom, b.top, b.bottom);
}

template <typename T>
struct Graph_window : Window
{
	fImage graph;
	
	Label lSolution;
	Label lTarget;
	Label lPoint;

	Graph_window(std::vector<T>& target, Mat<T>& limits, int type)
	{
		init(L"graphic methid", 800, 600, [](HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, Args args)->LRESULT
			{
				Graph_window* window = (Graph_window*)args[0];

				switch (msg)
				{
					case WM_PAINT:
					{
						PAINTSTRUCT ps;
						BeginPaint(hwnd, &ps);

						draw_image(window->canvas, window->graph, 0, 0, 1.0f, 0.8f);
						draw_filled_rect(window->canvas, 0, 0.8f, 1.0f, 0.2f, fColor(1.0f));
						window->render_canvas();

						EndPaint(hwnd, &ps);
					}return 0;
					case WM_CLOSE:
					{
						safe_release(window);
					}return 0;
				}

				return DefWindowProc(hwnd, msg, wParam, lParam);
			});


		lSolution.init(getHWND(), L"Solution", 0, 0.1f, 0.05f, 0.2f, 0.05f, RESIZABLE);
		set_font_size(lSolution.handle, 25);
		set_min_max_size(800, 600);

		auto [F, point] = solve(target, limits, type);

		wchar_t buffer[32];
		swprintf_s(buffer, L"F* = %d", F);
		lTarget.init(getHWND(), buffer, 0, 0.1f, 0.1f, 0.2f, 0.05f, RESIZABLE);

		//swprintf_s(buffer, L"X = (%f, %d)", point[0], point[1]);
		output(buffer, point[0], point[1]);
		lPoint.init(getHWND(), buffer, 0, 0.1f, 0.15f, 0.2f, 0.05f, RESIZABLE);

		redraw();
	}

	std::tuple<int, std::vector<T>> solve(std::vector<T>& target, Mat<T>& limits, int type)
	{
		std::vector<std::vector<T>> points;
		points.push_back({ T{}, T{} });

		Mat<T> temp(2, limits.column);

		for (int i = 0; i < limits.row; i++)
		{
			for (int j = i; j < limits.row; j++)
			{
				if (i == j) continue;

				for (int a = 0; a < temp.column; a++)
				{
					temp[0][a] = limits[i][a];
					temp[1][a] = limits[j][a];
				}

				auto [flag, intersection] = gausian_method(temp);

				if (flag > 0)
					points.push_back(intersection);
			}
		}

		int idx = 0;
		int F = target[0] * points[0][0] + target[1] * points[0][1] + target[2] + 0.0f;

		for (int i = 1; i < points.size(); i++)
		{
			int temp = target[0] * points[i][0] + target[1] * points[i][1] + target[2] + 0.0f;
			if (type == 0)
			{
				if (F > temp)
				{
					F = temp;
					idx = i;
				}
			}
			else
			{
				if (F < temp)
				{
					F = temp;
					idx = i;
				}
			}
		}

		make_graph(points[idx], limits, target);
		return { F, points[idx] };
	}


	void make_graph(std::vector<T>& point, Mat<T>& limits, std::vector<T>& target)
	{
		graph.resize(400, 400);
		int zero = 100;
		int x0 = -20, x1 = 20;
		int coef = 10;

		// draw axises
		drawLine(graph, 0, zero, graph.width, zero, fColor(1.0f));
		drawLine(graph, zero, 0, zero, graph.height, fColor(1.0f));

		for (int i = 0; i < graph.width; i++)
			drawPixel(graph, i * coef, zero + 1, fColor(1.0f));

		for (int i = 0; i < graph.height; i++)
			drawPixel(graph, zero + 1, i * coef, fColor(1.0f));


		for (int i = 0; i < limits.row; i++)
		{
			float y0 = 0;
			float y1 = 0;
			x0 = -20;
			x1 = 20;


			if (limits[i][1] + 0.0f != 0)
			{
				y0 = (limits[i][2] - limits[i][0] * x0) / limits[i][1] + 0.0f;
				y1 = (limits[i][2] - limits[i][0] * x1) / limits[i][1] + 0.0f;
			}
			else
			{
				x0 = x1 = limits[i][2] / limits[i][0] + 0.0f;
				y0 = -40;
				y1 = 100;
			}
			y0 *= coef;
			y1 *= coef;
			x0 *= coef;
			x1 *= coef;
			
			drawLine(graph, x0 + zero, y0 + zero, x1 + zero, y1 + zero, fColor(1.0f));
		}

		// draw target
		float y0 = 0;
		float y1 = 0;
		x0 = -20;
		x1 = 20;

		if (target[1] + 0.0f != 0)
		{
			y0 = (-target[2] - target[0] * x0) / target[1] + 0.0f;
			y1 = (-target[2] - target[0] * x1) / target[1] + 0.0f;
		}
		else
		{
			x0 = x1 = -target[2] / target[0] + 0.0f;
			y0 = -40;
			y1 = 100;
		}
		x0 += point[0] + 0.0f;
		x1 += point[0] + 0.0f;
		y0 += point[1] + 0.0f;
		y1 += point[1] + 0.0f;

		y0 *= coef;
		y1 *= coef;

		drawLine(graph, x0 * coef + zero, y0 + zero, x1 * coef + zero, y1 + zero, fColor(1.0f, 0 ,0));

	}


};




























//#pragma comment( lib, "comctl32.lib" )

//INITCOMMONCONTROLSEX icex;
//icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
//icex.dwICC = ICC_LISTVIEW_CLASSES;
//InitCommonControlsEx(&icex);

//RECT rcl;
//GetClientRect(getHWND(), &rcl);

//HWND hWndLV = CreateWindow(WC_LISTVIEW, L"",
//	WS_CHILD | LVS_REPORT | WS_VISIBLE,
//	0, 0, rcl.right - rcl.left, rcl.bottom - rcl.top - 50,
//	getHWND(), (HMENU)0, GetModuleHandle(NULL), NULL);

//// Чтобы определялись строка (item) и столбец (subitem) обязательно устанавливаем
//// расширенный стиль LVS_EX_FULLROWSELECT.
//ListView_SetExtendedListViewStyleEx(hWndLV, 0, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);



//WCHAR szText[] = L"dfsf";     // Temporary buffer.
//LVCOLUMN lvc;
//int iCol;

//// Initialize the LVCOLUMN structure.
//// The mask specifies that the format, width, text,
//// and subitem members of the structure are valid.
//lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM | LVIF_COLFMT;


//// Add the columns.
//for (iCol = 0; iCol < 5; iCol++)
//{
//	lvc.iSubItem = iCol;
//	lvc.pszText = szText;
//	lvc.cx = 100;               // Width of column in pixels.

//	if (iCol < 2)
//		lvc.fmt = LVCFMT_LEFT;  // Left-aligned column.
//	else
//		lvc.fmt = LVCFMT_RIGHT; // Right-aligned column.

//	// Load the names of the column headings from the string resources.
//	//LoadString(hInst,
//	//	iCol,
//	//	szText,
//	//	sizeof(szText) / sizeof(szText[0]));

//	// Insert the columns into the list view.
//	if (ListView_InsertColumn(hWndLV, iCol, &lvc) == -1);


//}



//int colNum = 3;
//int textMaxLen = 10;
//const wchar_t* item[] = { L"df", L"sdfads", L"dsf" };

//int iLastIndex = ListView_GetItemCount(hWndLV);


////SetListViewColumns(hWndLV, colNum, textMaxLen, header);
////AddListViewItems(hWndLV, colNum, textMaxLen, item[0]);
////AddListViewItems(hWndLV, colNum, textMaxLen, item[1]);
////AddListViewItems(hWndLV, colNum, textMaxLen, item[2]);

//for (int i = 0; i < 30; i++)
//{
//	LVITEM lvi;
//	lvi.mask = LVIF_TEXT;
//	lvi.cchTextMax = textMaxLen;
//	lvi.iItem = iLastIndex;
//	lvi.pszText = (LPWSTR)item[0];
//	lvi.iSubItem = 0;

//	if (ListView_InsertItem(hWndLV, &lvi) != -1)
//		for (int i = 1; i < colNum; i++)
//			ListView_SetItemText(hWndLV, iLastIndex, i, (LPWSTR)item[i]);

//}

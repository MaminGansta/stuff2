

template <typename T>
struct Graph_window : Window
{
	fImage graph;

	Graph_window(const std::vector<T>& target, Mat<T>& limits, int type)
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

		set_min_max_size(800, 600);
		solve(target, limits);
	}

	void solve(const std::vector<T>& target, Mat<T>& limits)
	{
		std::vector<std::vector<T>> points;
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

		make_graph(points, limits, target);
		redraw();
	}


	void make_graph(const std::vector<std::vector<T>>& points, Mat<T>& limits, const std::vector<T>& target)
	{
		graph.resize(800, 400);
		int zero = 200;
		int x0 = -200, x1 = 200;
		int coef = 10;

		// draw axises
		drawLine(graph, 0, zero, graph.width, zero, fColor(1.0f));
		drawLine(graph, zero, 0, zero, graph.height, fColor(1.0f));

		for (int i = 0; i < limits.row; i++)
		{
			int y0 = (limits[i][limits.column - 1] - limits[i][limits.column - 2] * x0) / limits[i][0] + 0.0f;
			int y1 = (limits[i][limits.column - 1] - limits[i][limits.column - 2] * x1) / limits[i][0] + 0.0f;
			
			x0 *= coef;
			x1 *= coef;
			y0 *= coef;
			y1 *= coef;

			drawLine(graph, x0 + zero, y0 + zero, x1 + zero, y1 + zero, fColor(1.0f));
		}

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

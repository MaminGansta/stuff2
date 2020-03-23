template <typename T>
struct Simplex_window : Window
{
	ListView table;

	Simplex_window(std::vector<T>& target, std::vector<T>& basis, Mat<T>& limits, int type)
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
		table.add_columns({L"aa", L"fff", L"fff" , L"fff" , L"fff" , L"fff" , L"fff" , L"fff" , L"fff" , L"fff" , L"fff" , L"fff" , L"fff" , L"fff" , L"fff" , L"fff" , L"fff" });
		table.add_rows({ {L"dsf", L"dsafadsf"}, {L"111", L"444"}, {L"dsf", L"222"} });


	}

};
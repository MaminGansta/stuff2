
struct About_window : Window
{
	Label lAbout;

	About_window()
	{
		init(L"About", 800, 600, [](HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, Args args)
			{
				About_window* window = (About_window*)args[0];

				return DefWindowProc(hwnd, msg, wParam, lParam);
			}, DEF_WINDOW);

		lAbout.init(getHWND(), L""
			"Here will be the disripiton about my program" 
			""
			""
			"", 0, 0.0f, 0.0f, 1.0f, 1.0f, RESIZABLE);
	}
};
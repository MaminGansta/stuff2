
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
			"Short discription\n" 
			""
			"In this program was implemented two solving methods (Artifitial basis method, Simplex method)\n\n"
			""
			"Menu:\n"
			"1) File provides save and open files(file saves in dir with .exe and it's name is current date)\n\n"
			""
			""
			"Interface:\n"
			"1) Method - means what kind of method is active now\n"
			"2) Type - meant what kind of problem (min, max)\n"
			"3) Number type - float(0.01), Fraction(3/6) also just integer is posible Fraction(3)\n"
			"4) Variables and Limits fileds (there is a limit <16 for both)\n\n"
			""
			"1) Artifical basis\n"
			"Here is ComboBox and 3 buttons. In CB are all posible pivots elements, (x - 1, y - 1) are indices in Simplex table.\n"
			"Button next and privius make or remove one step of algorithm. This method ignore basis input fieds\n\n"
			""
			"2) Simplex method\n"
			"The same with the privius one but here is posible to set basis.\n"
			"If basis is zeroes or incorect it will be calculated automaticly by A.B.\n\n"
			""
			"3) Also\n"
			"   1)Empty fields are zoroes"
			""
			"", 0, 0.0f, 0.0f, 1.0f, 1.0f, RESIZABLE);

		set_font_size(lAbout.handle, 20);
	}
};
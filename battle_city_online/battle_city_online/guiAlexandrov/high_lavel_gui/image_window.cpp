
template <typename Image_type>
struct Image_window : Window
{
	Image_type image;

	Image_window(const Image_type& image, int flags = DEF_WINDOW) : image(image)
	{
		init(L"window", image.width, image.height, [](HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, Args args)->LRESULT
			{
				Image_window* window = (Image_window*)args[0];

				switch (msg)
				{
					case WM_PAINT:
					{
						PAINTSTRUCT ps;
						BeginPaint(hwnd, &ps);
						draw_image(window->canvas, window->image, 0.0f, 0.0f, 1.0f, 1.0f);
						window->render_canvas(ps);
						EndPaint(hwnd, &ps);
					}break;
				}

				return DefWindowProc(hwnd, msg, wParam, lParam);
			});

		set_min_max_size(0, 0);
	}
};

template <typename Image_type>
int image_window(const Image_type& image, int flags = DEF_WINDOW)
{
	return (new Image_window<Image_type>(image, flags))->class_id;
}

int image_window(const wchar_t* name, int flags = DEF_WINDOW)
{
	return (new Image_window<Image>(name, flags))->class_id;
}
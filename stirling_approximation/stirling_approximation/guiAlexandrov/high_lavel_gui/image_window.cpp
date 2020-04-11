
template <typename Image_type>
struct Image_window : Window
{
	Image_type image;

	Image_window(const Image_type& image) : image(image)
	{
		init(L"window", image.width, image.height, [](HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, Args args)->LRESULT
			{
				Image_window* window = (Image_window*)args[0];

				switch (msg)
				{
					case WM_PAINT:
					{
						draw_image(window->canvas, window->image, 0.0f, 0.0f, 1.0f, 1.0f);
						window->render_canvas();
					}break;
				}

				return DefWindowProc(hwnd, msg, wParam, lParam);
			});
	}
};

template <typename Image_type>
Image_window<Image_type>* image_window(const Image_type& image)
{
	return new Image_window<Image_type>(image);
}

Image_window<Image>* image_window(const wchar_t* name)
{
	return new Image_window<Image>(name);
}
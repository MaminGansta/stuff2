
struct Canvas
{
	int height = 0, width = 0;
	int whole_size = 0;
	int capacity = 0;
	Color* data = nullptr;

	BITMAPINFO bitmap_info;

	Canvas()
	{
#ifdef FULL_SCREAN_CANVAS
		capacity = GetSystemMetrics(SM_CXSCREEN) * GetSystemMetrics(SM_CYSCREEN);
		data = new Color[capacity];
#endif
	}

	~Canvas() {
		safe_releaseArr(data);
	}

	void resize(HWND hwnd)
	{
		RECT rect;
		GetClientRect(hwnd, &rect);

		width = rect.right - rect.left;
		height = rect.bottom - rect.top;
		whole_size = width * height;

		bitmap_info.bmiHeader.biSize = sizeof(bitmap_info.bmiHeader);
		bitmap_info.bmiHeader.biWidth = width;
		bitmap_info.bmiHeader.biHeight = height;
		bitmap_info.bmiHeader.biPlanes = 1;
		bitmap_info.bmiHeader.biBitCount = 32;
		bitmap_info.bmiHeader.biCompression = BI_RGB;

		
		if (whole_size < capacity) return;

		capacity = width * height;
		delete[] data;
		data = new Color[capacity];
	}

	Color& operator [] (int inx)
	{
		assert((uint32_t)inx < whole_size);
		return data[inx];
	}

	void reserve(int capacity)
	{
		this->capacity = capacity;
		delete[] data;
		data = new Color[capacity];
	}
};
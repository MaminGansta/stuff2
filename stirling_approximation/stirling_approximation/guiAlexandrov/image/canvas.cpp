
struct Color
{
	union
	{
		struct { uint8_t b, g, r, a;};
		struct { uint8_t Y, U, V; };
		uint8_t raw[4];
		uint32_t whole;
	};

	inline Color() : r(0), g(0), b(0) {}
	inline Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) : r(r), g(g), b(b), a(a) {}
	inline Color(uint8_t color) : r(color), g(color), b(color), a(255) {}

	Color operator +(Color color)
	{
		return Color(min(r + color.r, 255), min(g + color.g, 255), min(b + color.b, 255));
	}

	Color operator - (const Color& color) const
	{
		return Color(r - color.r, g - color.g, b - color.b);
	}

	Color operator *(float f)
	{
		return Color(r * f, g * f, b * f);
	}

	Color operator /(float f)
	{
		return Color(r / f, g / f, b / f);
	}

	Color& operator += (const Color& other)
	{
		r = min(r + other.r, 255);
		g = min(g + other.g, 255);
		b = min(b + other.b, 255);
		return *this;
	}

	Color& operator *=(float f)
	{
		r *= f;
		g *= f;
		b *= f;
		return *this;
	}

};


struct Canvas
{
	int height, width;
	int whole_size;
	int capacity = 0;
	Color* memory = nullptr;

	BITMAPINFO bitmap_info;

	~Canvas() {
		safe_releaseArr(memory);
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
		delete[] memory;
		memory = new Color[capacity];
	}

	Color& operator [] (int inx)
	{
		assert((uint32_t)inx < whole_size);
		return memory[inx];
	}

	void reserve(int capacity)
	{
		this->capacity = capacity;
		delete[] memory;
		memory = new Color[capacity];
	}
};
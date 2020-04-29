
// ============= standart image ==================

struct Image
{
	int height = 0, width = 0;
	Color* data = NULL;
	bool invalid = true;

	Image() = default;
	Image(const wchar_t* filename_utf8)
	{
		open(filename_utf8);
	}

	Image(const Image& copy)
	{
		height = copy.height;
		width = copy.width;
		data = new Color[height * width];
		memmove(data, copy.data, sizeof(Color) * height * width);
		invalid = false;
	}

	Image(Image&& other)
	{
		data = other.data;
		width = other.width;
		height = other.height;
		other.data = NULL;
		other.invalid = true;
		invalid = false;
	}

	Image& operator= (const Image& copy)
	{
		delete[] data;
		height = copy.height;
		width = copy.width;
		data = new Color[height * width];
		memmove(data, copy.data, sizeof(Color) * height * width);
		invalid = false;
		return *this;
	}

	Image& operator = (Image&& other)
	{
		delete[] data;
		data = other.data;
		width = other.width;
		height = other.height;
		other.data = NULL;
		other.invalid = true;
		invalid = false;
		return *this;
	}

	void open(const wchar_t* filename_utf8)
	{
		delete[] data;

		int chanels;
		char filename[256];
		stbi_convert_wchar_to_utf8(filename, sizeof(filename), filename_utf8);

		uint8_t* raw = stbi_load(filename, &width, &height, &chanels, 0);

		if (raw == NULL) return;

		data = new Color[width * height];
		
		if (chanels == 3)
		{
			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{
					int pos = (y * width + x) * chanels;
					data[(height - y - 1) * width + x] = Color(raw[pos], raw[pos + 1], raw[pos + 2]);
				}
			}
		}
		else if (chanels == 4)
		{
			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{
					int pos = (y * width + x) * chanels;
					data[(height - y - 1) * width + x] = Color(raw[pos], raw[pos + 1], raw[pos + 2], raw[pos + 3]);
				}
			}
		}
		
		invalid = false;
		stbi_image_free(raw);
	}


	void resize(int width, int height)
	{
		this->width = width;
		this->height = height;
		delete[] data;
		data = new Color[width * height];
		invalid = false;
	}

	Image(int width, int height) : width(width), height(height)
	{
		data = new Color[width * height];
		invalid = false;
	}

	Color& get_pixel(int x, int y)
	{
		assert(((uint32_t)y < height) | ((uint32_t)x < width));
		return data[y * width + x];
	}
	
	Color& operator [] (int idx)
	{
		assert((uint32_t)idx < width * height);
		return data[idx];
	}

	const Color& operator [] (int idx) const
	{
		assert((uint32_t)idx < width* height);
		return data[idx];
	}

	Color& get_pixel_scaled(int x, int y, int screen_w, int screen_h)
	{
		y = y * height / screen_h;
		x = x * width / screen_w;
	
		assert(((uint32_t)y < height) | ((uint32_t)x < width));
		return data[y * width + x];
	}


	const Color& get_pixel_scaled(int x, int y, int screen_w, int screen_h) const
	{
		y = y * height / screen_h;
		x = x * width / screen_w;

		assert(((uint32_t)y < height) | ((uint32_t)x < width));
		return data[y * width + x];
	}

	~Image() { delete[] data; }
};


// =============== float Image  ==================

struct fImage
{
	int height = 0, width = 0;
	fColor* data = NULL;
	bool invalid = true;

	fImage() = default;
	fImage(const wchar_t* filename_utf8)
	{
		open(filename_utf8);
	}

	void open(const wchar_t* filename_utf8)
	{
		delete[] data;

		int chanels;
		char filename[256];
		stbi_convert_wchar_to_utf8(filename, sizeof(filename), filename_utf8);

		uint8_t* raw = stbi_load(filename, &width, &height, &chanels, 0);

		if (raw == NULL)
			return;

		data = new fColor[width * height];

		if (chanels == 3)
		{
			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{
					int pos = (y * width + x) * chanels;
					data[(height - y - 1) * width + x] = fColor(raw[pos], raw[pos + 1], raw[pos + 2]);
				}
			}
		}
		else if (chanels == 4)
		{
			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{
					int pos = (y * width + x) * chanels;
					data[(height - y - 1) * width + x] = fColor(raw[pos], raw[pos + 1], raw[pos + 2], raw[pos + 3]);
				}
			}
		}

		stbi_image_free(raw);
		invalid = false;
	}

	fImage(int width, int height) : width(width), height(height)
	{
		data = new fColor[width * height];
		invalid = false;
	}

	fImage(const fImage & copy)
	{
		height = copy.height;
		width = copy.width;
		data = new fColor[height * width];
		memmove(data, copy.data, sizeof(fColor) * height * width);
		invalid = false;
	}

	fImage(fImage&& other)
	{
		data = other.data;
		width = other.width;
		height = other.height;
		other.data = NULL;
		other.invalid = true;
		invalid = false;
	}

	fImage(const Image& image)
	{
		resize(image.width, image.height);
		for (int i = 0; i < height; i++)
			for (int j = 0; j < width; j++)
				data[i * width + j] = image[i * width + j];
	}

	fImage& operator = (const fImage& copy)
	{
		delete[] data;
		height = copy.height;
		width = copy.width;
		data = new fColor[height * width];
		memmove(data, copy.data, sizeof(fColor) * height * width);
		invalid = false;
		return *this;
	}

	fImage& operator = (fImage&& other)
	{
		delete[] data;
		data = other.data;
		width = other.width;
		height = other.height;
		other.data = NULL;
		other.invalid = true;
		invalid = false;
		return *this;
	}

	~fImage() { delete[] data; }


	void resize(int width, int height)
	{
		this->width = width;
		this->height = height;
		delete[] data;
		data = new fColor[width * height];
		invalid = false;
	}

	fColor& get_pixel(int x, int y)
	{
		assert(((uint32_t)y < height) | ((uint32_t)x < width));
		return data[y * width + x];
	}

	fColor& operator [] (int idx)
	{
		assert((uint32_t)idx < width * height);
		return data[idx];
	}

	const fColor& operator [] (int idx) const
	{
		assert((uint32_t)idx < width * height);
		return data[idx];
	}


	fColor& get_pixel_scaled(int x, int y, int screen_w, int screen_h)
	{
		y = y * height / screen_h;
		x = x * width / screen_w;

		assert(((uint32_t)y < height) | ((uint32_t)x < width));
		return data[y * width + x];
	}

	const fColor& get_pixel_scaled(int x, int y, int screen_w, int screen_h) const
	{
		y = y * height / screen_h;
		x = x * width / screen_w;

		assert(((uint32_t)y < height) | ((uint32_t)x < width));
		return data[y * width + x];
	}

	operator Image()
	{
		Image res(width, height);
		for (int i = 0; i < height; i++)
			for (int j = 0; j < width; j++)
				res[i * width + j] = data[i * width + j];
		return res;
	}

};
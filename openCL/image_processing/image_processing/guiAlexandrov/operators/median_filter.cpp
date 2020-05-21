template <typename Image_type, size_t size>
struct Median_filter;


template <size_t size>
struct Median_filter<fImage, size>
{
	fImage apply(fImage& original)
	{
		fImage res;
		res.resize(original.width, original.height);

		// main area
		int pad = size / 2;

		int x0 = pad;
		int y0 = pad;
		int width = original.width - pad;
		int height = original.height - pad;


		std::array<fColor, size * size> buffer;
		for (int y = y0; y < height; y++)
		{
			for (int x = x0; x < width; x++)
			{
				for (int i = 0; i < size; i++)
					for (int j = 0; j < size; j++)
						buffer[i * size + j] = original[(y - pad + i) * original.width + x - pad + j];
				
				std::sort(buffer.begin(), buffer.end(), [](const fColor& a, const fColor& b) { return (a.r + a.g + a.b) < (b.r + b.g + b.b); });
				res[y * res.width + x] = buffer[size * size / 2 + 1];
			}
		}


		// edges
		for (int i = 0; i < 2; i++)
		{
			for (int y = i * (original.height - pad); y < (1 - i) * pad + i * (original.height); y++)
			{
				for (int x = 0; x < original.width; x++)
				{
					for (int i = 0; i < size; i++)
					{
						for (int j = 0; j < size; j++)
						{
							int core_y = abs(y - pad + i);
							int core_x = abs(x - pad + j);
		
							if (core_x >= original.width) core_x = original.width - (core_x - original.width) - 1;
							if (core_y >= original.height) core_y = original.height - (core_y - original.height) - 1;
		
							buffer[i * size + j] = original[core_y * original.width + core_x];
						}
					}
					std::sort(buffer.begin(), buffer.end(), [](const fColor& a, const fColor& b) { return (a.r + a.g + a.b) < (b.r + b.g + b.b); });
					res[y * res.width + x] = buffer[size * size / 2 + 1];
				}
			}
		}
		
		for (int i = 0; i < 2; i++)
		{
			for (int y = pad; y < original.height - pad; y++)
			{
				for (int x = i * (original.width - pad); x < (1 - i) * pad + i * original.width; x++)
				{
					for (int i = 0; i < size; i++)
					{
						for (int j = 0; j < size; j++)
						{
							int core_y = abs(y - pad + i);
							int core_x = abs(x - pad + j);
		
							if (core_x >= original.width) core_x = original.width - (core_x - original.width) - 1;
							if (core_y >= original.height) core_y = original.height - (core_y - original.height) - 1;
		
							buffer[i * size + j] = original[core_y * original.width + core_x];
						}
					}
					std::sort(buffer.begin(), buffer.end(), [](const fColor& a, const fColor& b) { return (a.r + a.g + a.b) < (b.r + b.g + b.b); });
					res[y * res.width + x] = buffer[size * size / 2 + 1];
				}
			}
		}

		return res;
	}

};


template <size_t size>
struct Median_filter<Image, size>
{
	Image apply(Image& original)
	{
		Image res;
		res.resize(original.width, original.height);

		// main area
		int pad = size / 2;

		int x0 = pad;
		int y0 = pad;
		int width = original.width - pad;
		int height = original.height - pad;


		std::array<Color, size* size> buffer;
		for (int y = y0; y < height; y++)
		{
			for (int x = x0; x < width; x++)
			{
				for (int i = 0; i < size; i++)
					for (int j = 0; j < size; j++)
						buffer[i * size + j] = original[(y - pad + i) * original.width + x - pad + j];

				std::sort(buffer.begin(), buffer.end(), [](const Color& a, const Color& b) { return (a.r + a.g + a.b) < (b.r + b.g + b.b); });
				res[y * res.width + x] = buffer[size * size / 2 + 1];
			}
		}


		// edges
		for (int i = 0; i < 2; i++)
		{
			for (int y = i * (original.height - pad); y < (1 - i) * pad + i * (original.height); y++)
			{
				for (int x = 0; x < original.width; x++)
				{
					for (int i = 0; i < size; i++)
					{
						for (int j = 0; j < size; j++)
						{
							int core_y = abs(y - pad + i);
							int core_x = abs(x - pad + j);

							if (core_x >= original.width) core_x = original.width - (core_x - original.width) - 1;
							if (core_y >= original.height) core_y = original.height - (core_y - original.height) - 1;

							buffer[i * size + j] = original[core_y * original.width + core_x];
						}
					}
					std::sort(buffer.begin(), buffer.end(), [](const Color& a, const Color& b) { return (a.r + a.g + a.b) < (b.r + b.g + b.b); });
					res[y * res.width + x] = buffer[size * size / 2 + 1];
				}
			}
		}

		for (int i = 0; i < 2; i++)
		{
			for (int y = pad; y < original.height - pad; y++)
			{
				for (int x = i * (original.width - pad); x < (1 - i) * pad + i * original.width; x++)
				{
					for (int i = 0; i < size; i++)
					{
						for (int j = 0; j < size; j++)
						{
							int core_y = abs(y - pad + i);
							int core_x = abs(x - pad + j);

							if (core_x >= original.width) core_x = original.width - (core_x - original.width) - 1;
							if (core_y >= original.height) core_y = original.height - (core_y - original.height) - 1;

							buffer[i * size + j] = original[core_y * original.width + core_x];
						}
					}
					std::sort(buffer.begin(), buffer.end(), [](const Color& a, const Color& b) { return (a.r + a.g + a.b) < (b.r + b.g + b.b); });
					res[y * res.width + x] = buffer[size * size / 2 + 1];
				}
			}
		}

		return res;
	}

};


template <typename Image_type, size_t size = 3>
Image_type median_filter(Image_type& image)
{
	Median_filter<Image_type, size> mf;
	return mf.apply(image);
}
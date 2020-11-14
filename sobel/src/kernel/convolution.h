#pragma once

#include <omp.h>
#include <stdint.h>
#include <cstdlib>
#include <initializer_list>

#include "../image/image.h"


// ============== Kernel class ===================
template  <typename Image_type, size_t size>
struct Kernel;


template <size_t size>
struct Kernel<Image, size>
{
	int coef = 10000;
	int kernel[size][size];

	Kernel() = default;
	Kernel(std::initializer_list<float> coefs)
	{
		assert(size * size == coefs.size());
		auto coef_ptr = coefs.begin();
		float total = 0;
	
		for (int i = 0; i < size; i++)
		{
			for (int j = 0; j < size; j++)
			{
				total += *coef_ptr;
				kernel[i][j] = *coef_ptr++ * 10000.0f;
			}
		}
	}

	void Kernal_init(float kernal[size][size])
	{
		for (int i = 0; i < size; i++)
		{
			for (int j = 0; j < size; j++)
			{
				this->kernel[i][j] = kernal[i][j] * 10000.0f;
			}
		}
	}


	Image apply(const Image& original)
	{
		Image res;
		res.resize(original.width, original.height);

		// main area
		int pad = size / 2;

		int x0 = pad;
		int y0 = pad;
		int width = original.width - pad;
		int height = original.height - pad;

		for (int y = y0; y < height; y++)
		{
			for (int x = x0; x < width; x++)
			{
				int r = 0, g = 0, b = 0;
				for (int i = 0; i < size; i++)
				{
					for (int j = 0; j < size; j++)
					{
						const Color& pixel = original[(y - pad + i) * original.width + x - pad + j];
						r += pixel.r * kernel[i][j];
						g += pixel.g * kernel[i][j];
						b += pixel.b * kernel[i][j];
					}
				}
				res[y * res.width + x].r = chanel_clip(r / coef);
				res[y * res.width + x].g = chanel_clip(g / coef);
				res[y * res.width + x].b = chanel_clip(b / coef);
				res[y * res.width + x].a = original[y * res.width + x].a;
			}
		}

		// edges
		for (int i = 0; i < 2; i++)
		{
			for (int y = i * (original.height - pad); y < (1 - i) * pad + i * (original.height); y++)
			{
				for (int x = 0; x < original.width; x++)
				{
					int r = 0, g = 0, b = 0;
					for (int i = 0; i < size; i++)
					{
						for (int j = 0; j < size; j++)
						{
							int core_y = abs(y - pad + i);
							int core_x = abs(x - pad + j);

							if (core_x >= original.width) core_x = original.width - (core_x - original.width) - 1;
							if (core_y >= original.height) core_y = original.height - (core_y - original.height) - 1;

							const Color& pixel = original[core_y * original.width + core_x];

							r += pixel.r * kernel[i][j];
							g += pixel.g * kernel[i][j];
							b += pixel.b * kernel[i][j];
						}
					}
					res[y * res.width + x].r = chanel_clip(r / coef);
					res[y * res.width + x].g = chanel_clip(g / coef);
					res[y * res.width + x].b = chanel_clip(b / coef);
					res[y * res.width + x].a = original[y * res.width + x].a;
				}
			}
		}

		for (int i = 0; i < 2; i++)
		{
			for (int y = pad; y < original.height - pad; y++)
			{
				for (int x = i * (original.width - pad); x < (1 - i) * pad + i * original.width; x++)
				{
					int r = 0, g = 0, b = 0;
					for (int i = 0; i < size; i++)
					{
						for (int j = 0; j < size; j++)
						{
							int core_y = abs(y - pad + i);
							int core_x = abs(x - pad + j);

							if (core_x >= original.width) core_x = original.width - (core_x - original.width) - 1;
							if (core_y >= original.height) core_y = original.height - (core_y - original.height) - 1;

							const Color& pixel = original[core_y * original.width + core_x];
							r += pixel.r * kernel[i][j];
							g += pixel.g * kernel[i][j];
							b += pixel.b * kernel[i][j];
						}
					}
					res[y * res.width + x].r = chanel_clip(r / coef);
					res[y * res.width + x].g = chanel_clip(g / coef);
					res[y * res.width + x].b = chanel_clip(b / coef);
					res[y * res.width + x].a = original[y * res.width + x].a;
				}
			}
		}

		return res;
	}


	Image apply_async(const Image& original)
	{
		Image res;
		res.resize(original.width, original.height);

		// main area
		int pad = size / 2;

		int x0 = pad;
		int y0 = pad;
		int width = original.width - pad;
		int height = original.height - pad;

		#pragma omp parallel for
		for (int y = y0; y < height; y++)
		{
			for (int x = x0; x < width; x++)
			{
				int r = 0, g = 0, b = 0;
				for (int i = 0; i < size; i++)
				{
					for (int j = 0; j < size; j++)
					{
						const Color& pixel = original[(y - pad + i) * original.width + x - pad + j];
						r += pixel.r * kernel[i][j];
						g += pixel.g * kernel[i][j];
						b += pixel.b * kernel[i][j];
					}
				}
				res[y * res.width + x].r = chanel_clip(r / coef);
				res[y * res.width + x].g = chanel_clip(g / coef);
				res[y * res.width + x].b = chanel_clip(b / coef);
				res[y * res.width + x].a = original[y * res.width + x].a;
			}
		}

		// edges
		for (int i = 0; i < 2; i++)
		{
			for (int y = i * (original.height - pad); y < (1 - i) * pad + i * (original.height); y++)
			{
				for (int x = 0; x < original.width; x++)
				{
					int r = 0, g = 0, b = 0;
					for (int i = 0; i < size; i++)
					{
						for (int j = 0; j < size; j++)
						{
							int core_y = abs(y - pad + i);
							int core_x = abs(x - pad + j);

							if (core_x >= original.width) core_x = original.width - (core_x - original.width) - 1;
							if (core_y >= original.height) core_y = original.height - (core_y - original.height) - 1;

							const Color& pixel = original[core_y * original.width + core_x];

							r += pixel.r * kernel[i][j];
							g += pixel.g * kernel[i][j];
							b += pixel.b * kernel[i][j];
						}
					}
					res[y * res.width + x].r = chanel_clip(r / coef);
					res[y * res.width + x].g = chanel_clip(g / coef);
					res[y * res.width + x].b = chanel_clip(b / coef);
					res[y * res.width + x].a = original[y * res.width + x].a;
				}
			}
		}

		for (int i = 0; i < 2; i++)
		{
			for (int y = pad; y < original.height - pad; y++)
			{
				for (int x = i * (original.width - pad); x < (1 - i) * pad + i * original.width; x++)
				{
					int r = 0, g = 0, b = 0;
					for (int i = 0; i < size; i++)
					{
						for (int j = 0; j < size; j++)
						{
							int core_y = abs(y - pad + i);
							int core_x = abs(x - pad + j);

							if (core_x >= original.width) core_x = original.width - (core_x - original.width) - 1;
							if (core_y >= original.height) core_y = original.height - (core_y - original.height) - 1;

							const Color& pixel = original[core_y * original.width + core_x];
							r += pixel.r * kernel[i][j];
							g += pixel.g * kernel[i][j];
							b += pixel.b * kernel[i][j];
						}
					}
					res[y * res.width + x].r = chanel_clip(r / coef);
					res[y * res.width + x].g = chanel_clip(g / coef);
					res[y * res.width + x].b = chanel_clip(b / coef);
					res[y * res.width + x].a = original[y * res.width + x].a;
				}
			}
		}

		return res;
	}

	uint32_t* operator [] (int i) { return kernel[i]; }
};

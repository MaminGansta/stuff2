
#ifndef MAX
#define MAX(a, b) (a > b? a: b)
#endif

template <typename Image_type, typename Color_type>
void drawPixel(Image_type& surface, int x, int y, const Color_type& color)
{
	if ((uint32_t)y >= surface.height || (uint32_t)x >= surface.width) return;
	surface[y * surface.width + x] = color;
}

template <typename Image_type, typename Color_type>
void drawPixel(Image_type& surface, int x, int y, const Color_type& color, int width)
{
	int half_width = (width + 1) / 2;

	for (int i = -half_width; i < half_width; i++)
		for (int j = -half_width; j < half_width; j++)
			drawPixel(surface, x + j, y + i, color);
}


template <typename Image_type, typename Color_type>
void draw_filled_circle(Image_type& surface, int x, int y, const Color_type& color, int radius)
{
	for (int i = -radius; i < radius; i++)
	{
		for (int j = -radius; j < radius; j++)
		{
			if (i * i + j * j < radius * radius)
				drawPixel(surface, x + i, y + j, color);
		}
	}
}



template <typename Image_type, typename Color_type>
void drawLine(Image_type& surface, int x, int y, int x2, int y2, const Color_type& color, int width = 1)
{
	bool yLonger = false;
	int shortLen = y2 - y;
	int longLen = x2 - x;
	if (abs(shortLen) > abs(longLen)) {
		int swap = shortLen;
		shortLen = longLen;
		longLen = swap;
		yLonger = true;
	}
	int decInc;
	if (longLen == 0) decInc = 0;
	else decInc = (shortLen << 16) / longLen;

	if (yLonger) {
		if (longLen > 0) {
			longLen += y;
			for (int j = 0x8000 + (x << 16); y <= longLen; ++y) {
				drawPixel(surface, j >> 16, y, color, width);
				j += decInc;
			}
			return;
		}
		longLen += y;
		for (int j = 0x8000 + (x << 16); y >= longLen; --y) {
			drawPixel(surface, j >> 16, y, color, width);
			j -= decInc;
		}
		return;
	}

	if (longLen > 0) {
		longLen += x;
		for (int j = 0x8000 + (y << 16); x <= longLen; ++x) {
			drawPixel(surface, x, j >> 16, color, width);
			j += decInc;
		}
		return;
	}
	longLen += x;
	for (int j = 0x8000 + (y << 16); x >= longLen; --x) {
		drawPixel(surface, x, j >> 16, color, width);
		j -= decInc;
	}
}


template <typename Image_type, typename Color_type>
void draw_line(Image_type& surface, float fx0, float fy0, float fx1, float fy1, const Color_type& color, int width = 1)
{
	int x0 = fx0 * surface.width;
	int y0 = fy0 * surface.height;
	int x1 = fx1 * surface.width;
	int y1 = fy1 * surface.height;
	drawLine(surface, x0, y0, x1, y1, color, width);
}


template <typename Image_type, typename Color_type>
void draw_filled_circle(Image_type& surface, float fx, float fy, const Color_type& color, float fradius)
{
	int x = fx * surface.width;
	int y = fy * surface.height;
	int radius = (surface.width / 2) * fradius;
	if ((uint32_t)y >= surface.height || (uint32_t)x >= surface.width) return;

	for (int i = -radius; i < radius; i++)
		for (int j = -radius; j < radius; j++)
			if (i * i + j * j < radius * radius)
				drawPixel(surface, x + i, y + j, color);
}


template <typename Image_type, typename Color_type>
void draw_rect(Image_type& surface, float fx, float fy, float fwidth, float fheight, const Color_type& color, int pixel_width = 1)
{
	int x0 = surface.width * fx;
	int y0 = surface.height * fy;

	int width = surface.width * fwidth;
	int height = surface.height * fheight;

	drawLine(surface, x0, y0, x0 + width, y0, color, pixel_width);
	drawLine(surface, x0, y0, x0, y0 + height, color, pixel_width);
	drawLine(surface, x0 + width, y0, x0 + width, y0 + height, color, pixel_width);
	drawLine(surface, x0, y0 + height, x0 + width, y0 + height, color, pixel_width);
}

template <typename Image_type, typename Color_type>
void draw_filled_rect(Image_type& surface, float fx, float fy, float fwidth, float fheight, const Color_type& color)
{
	int x0 = surface.width * fx;
	int y0 = surface.height * fy;

	int width = surface.width * fwidth;
	int height = surface.height * fheight;

	for (int y = y0; y < y0 + height; y++)
		for (int x = x0; x < width; x++)
			drawPixel(surface, x, y, color);
}

template <typename Image_type, typename Color_type>
void draw_filled_rect_async(Image_type& surface, float fx, float fy, float fwidth, float fheight, const Color_type& color)
{
	int x0 = surface.width * fx;
	int y0 = surface.height * fy;

	int width = surface.width * fwidth;
	int height = surface.height * fheight;

	std::future<void> res[MAX_THREADS];

	for (int i = 0; i < workers.size; i++)
	{
		int from_x = x0 + (i)*width / workers.size;
		int to_x = x0 + (i + 1) * width / workers.size;

		res[i] = workers.add_task([y0, height, from_x, to_x, &surface, &color]() {
			for (int y = y0; y < y0 + height; y++)
				for (int x = from_x; x < to_x; x++)
					drawPixel(surface, x, y, color);
			});
	}

	for (int i = 0; i < workers.size; i++)
		res[i].get();
}



// ============= Draw Image to the surface ===================

template <typename Surface_type, typename Image_type>
void draw_image(Surface_type& surface, const Image_type& image,
	float fpos_x, float fpos_y, float fwidth, float fheight)
{
	if (fpos_x > 1.0f || fpos_y > 1.0f || fpos_x < 0.0f || fpos_y < 0.0f || image.invalid) return;

	int pos_x = surface.width * fpos_x;
	int pos_y = surface.height * fpos_y;

	fwidth = min(fwidth, 1.0f - fpos_x);
	fheight = min(fheight, 1.0f - fpos_y);

	int width = surface.width * fwidth;
	int height = surface.height * fheight;

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			assert(x < surface.width);
			Color color = image.get_pixel_scaled(x, y, width, height);
			surface[(y + pos_y) * surface.width + (x + pos_x)] = color;
		}
	}
}


template <typename Surface_type, typename Image_type>
void draw_image_async(Surface_type& surface, const Image_type& image,
	float fpos_x, float fpos_y, float fwidth, float fheight)
{
	if (fpos_x > 1.0f || fpos_y > 1.0f || fpos_x < 0.0f || fpos_y < 0.0f) return;

	int pos_x = surface.width * fpos_x;
	int pos_y = surface.height * fpos_y;

	fwidth = min(fwidth, 1.0f - fpos_x);
	fheight = min(fheight, 1.0f - fpos_y);

	int width = surface.width * fwidth;
	int height = surface.height * fheight;

	std::future<void> res[MAX_THREADS];

	for (int i = 0; i < workers.size; i++)
	{
		int from_x = i * width / workers.size;
		int to_x = (i + 1) * width / workers.size;

		res[i] = workers.add_task([from_x, to_x, pos_y, pos_x, height, width, &surface, &image]()
			{
				for (int y = 0; y < height; y++)
					for (int x = from_x; x < to_x; x++)
					{
						assert(x < surface.width);
						Color color = image.get_pixel_scaled(x, y, width, height);
						surface[(y + pos_y) * surface.width + (x + pos_x)] = color;
					}
			});
	}

	for (int i = 0; i < workers.size; i++)
		res[i].get();
}




// ======================= alpha blending ==============================

template <typename Image_type>
void drawPixel_a(Image_type& surface, int x, int y, const fColor& color)
{
	if ((uint32_t)y >= surface.height || (uint32_t)x >= surface.width) return;
	fColor dest = surface[y * surface.width + x];
	dest = color * color.a + dest * (dest.a * (1.0f - color.a));
	surface[y * surface.width + x] = dest;
}

template <typename Image_type>
void drawPixel_a(Image_type& surface, int x, int y, const fColor& color, int width)
{
	int half_width = (width + 1) / 2;

	for (int i = -half_width; i < half_width; i++)
		for (int j = -half_width; j < half_width; j++)
			drawPixel_a(surface, x + j, y + i, color);
}


template <typename Image_type>
void draw_filled_circle_a(Image_type& surface, float fx, float fy, const fColor& color, float fradius)
{
	int x = fx * surface.width;
	int y = fy * surface.height;
	int radius = (surface.width / 2) * fradius;
	for (int i = -radius; i < radius; i++)
	{
		for (int j = -radius; j < radius; j++)
		{
			if (i * i + j * j < radius * radius)
				drawPixel_a(surface, x + i, y + j, color);
		}
	}
}

template <typename Image_type>
void draw_filled_circle_a(Image_type& surface, int x, int y, const fColor& color, int radius)
{
	for (int i = -radius; i < radius; i++)
	{
		for (int j = -radius; j < radius; j++)
		{
			if (i * i + j * j < radius * radius)
				drawPixel_a(surface, x + i, y + j, color);
		}
	}
}
			

template <typename Image_type, typename Color_type>
void drawLine_a(Image_type& surface, int x, int y, int x2, int y2, const Color_type& color, int width = 1)
{
	//color.a /= (width / 3 + 1);
	bool yLonger = false;
	int shortLen = y2 - y;
	int longLen = x2 - x;
	if (abs(shortLen) > abs(longLen)) {
		int swap = shortLen;
		shortLen = longLen;
		longLen = swap;
		yLonger = true;
	}
	int decInc;
	if (longLen == 0) decInc = 0;
	else decInc = (shortLen << 16) / longLen;

	if (yLonger) {
		if (longLen > 0) {
			longLen += y;
			for (int j = 0x8000 + (x << 16); y <= longLen; ++y) {
				drawPixel_a(surface, j >> 16, y, color, width);
				j += decInc;
			}
			return;
		}
		longLen += y;
		for (int j = 0x8000 + (x << 16); y >= longLen; --y) {
			drawPixel_a(surface, j >> 16, y, color, width);
			j -= decInc;
		}
		return;
	}

	if (longLen > 0) {
		longLen += x;
		for (int j = 0x8000 + (y << 16); x <= longLen; ++x) {
			drawPixel_a(surface, x, j >> 16, color, width);
			j += decInc;
		}
		return;
	}
	longLen += x;
	for (int j = 0x8000 + (y << 16); x >= longLen; --x) {
		drawPixel_a(surface, x, j >> 16, color, width);
		j -= decInc;
	}
}

template <typename Image_type, typename Color_type>
void draw_line_a(Image_type& surface, float fx0, float fy0, float fx1, float fy1, const Color_type& color, int width = 1)
{
	int x0 = fx0 * surface.width;
	int y0 = fy0 * surface.height;
	int x1 = fx1 * surface.width;
	int y1 = fy1 * surface.height;
	drawLine_a(surface, x0, y0, x1, y1, color, width);
}




template <typename Image_type, typename Color_type>
void draw_rect_a(Image_type& surface, float fx, float fy, float fwidth, float fheight, const Color_type& color, int pixel_width = 1)
{
	int x0 = surface.width * fx;
	int y0 = surface.height * fy;

	int width = surface.width * fwidth;
	int height = surface.height * fheight;

	drawLine_a(surface, x0, y0, x0 + width, y0, color, pixel_width);
	drawLine_a(surface, x0, y0, x0, y0 + height, color, pixel_width);
	drawLine_a(surface, x0 + width, y0, x0 + width, y0 + height, color, pixel_width);
	drawLine_a(surface, x0, y0 + height, x0 + width, y0 + height, color, pixel_width);
}

template <typename Image_type, typename Color_type>
void draw_filled_rect_a(Image_type& surface, float fx, float fy, float fwidth, float fheight, const Color_type& color)
{
	int x0 = surface.width * fx;
	int y0 = surface.height * fy;

	int width = surface.width * fwidth;
	int height = surface.height * fheight;

	for (int y = y0; y < y0 + height; y++)
		for (int x = x0; x < width; x++)
			drawPixel_a(surface, x, y, color);
}

template <typename Image_type, typename Color_type>
void draw_filled_rect_async_a(Image_type& surface, float fx, float fy, float fwidth, float fheight, const Color_type& color)
{
	int x0 = surface.width * fx;
	int y0 = surface.height * fy;

	int width = surface.width * fwidth;
	int height = surface.height * fheight;

	std::future<void> res[MAX_THREADS];

	for (int i = 0; i < workers.size; i++)
	{
		int from_x = x0 + (i)*width / workers.size;
		int to_x = x0 + (i + 1) * width / workers.size;

		res[i] = workers.add_task([y0, height, from_x, to_x, &surface, &color]() {
			for (int y = y0; y < y0 + height; y++)
				for (int x = from_x; x < to_x; x++)
					drawPixel_a(surface, x, y, color);
			});
	}

	for (int i = 0; i < workers.size; i++)
		res[i].get();
}



// =============== image alpha blending ====================


template <typename Surface_type, typename Image_type>
void draw_image_a(Surface_type& surface, const Image_type& image,
	float fpos_x, float fpos_y, float fwidth, float fheight)
{
	if (fpos_x > 1.0f || fpos_y > 1.0f || fpos_x < 0.0f || fpos_y < 0.0f || image.invalid) return;

	int pos_x = surface.width * fpos_x;
	int pos_y = surface.height * fpos_y;

	fwidth = min(fwidth, 1.0f - fpos_x);
	fheight = min(fheight, 1.0f - fpos_y);

	int width = surface.width * fwidth;
	int height = surface.height * fheight;

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			assert(x < surface.width);
			fColor color = image.get_pixel_scaled(x, y, width, height);
			drawPixel_a(surface, x + pos_x, y + pos_y, color);
		}
	}
}


template <typename Surface_type, typename Image_type>
void draw_image_async_a(Surface_type& surface, const Image_type& image,
	float fpos_x, float fpos_y, float fwidth, float fheight)
{
	if (fpos_x > 1.0f || fpos_y > 1.0f || fpos_x < 0.0f || fpos_y < 0.0f) return;

	int pos_x = surface.width * fpos_x;
	int pos_y = surface.height * fpos_y;

	fwidth = min(fwidth, 1.0f - fpos_x);
	fheight = min(fheight, 1.0f - fpos_y);

	int width = surface.width * fwidth;
	int height = surface.height * fheight;

	std::future<void> res[MAX_THREADS];

	for (int i = 0; i < workers.size; i++)
	{
		int from_x = i * width / workers.size;
		int to_x = (i + 1) * width / workers.size;

		res[i] = workers.add_task([from_x, to_x, pos_y, pos_x, height, width, &surface, &image]()
			{
				for (int y = 0; y < height; y++)
					for (int x = from_x; x < to_x; x++)
					{
						assert(x < surface.width);
						fColor color = image.get_pixel_scaled(x, y, width, height);
						drawPixel_a(surface, x + pos_x, y + pos_y, color);
					}
			});
	}

	for (int i = 0; i < workers.size; i++)
		res[i].get();
}


template <typename Surface_type, typename Image_type>
void draw_image_a(Surface_type& surface, const Image_type& image,
	float fpos_x, float fpos_y, float fwidth, float fheight, float alpha)
{
	if (fpos_x > 1.0f || fpos_y > 1.0f || fpos_x < 0.0f || fpos_y < 0.0f || image.invalid) return;

	int pos_x = surface.width * fpos_x;
	int pos_y = surface.height * fpos_y;

	fwidth = min(fwidth, 1.0f - fpos_x);
	fheight = min(fheight, 1.0f - fpos_y);

	int width = surface.width * fwidth;
	int height = surface.height * fheight;

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			assert(x < surface.width);
			fColor color = image.get_pixel_scaled(x, y, width, height);
			color.a = alpha;
			drawPixel_a(surface, x + pos_x, y + pos_y, color);
		}
	}
}


template <typename Surface_type, typename Image_type>
void draw_image_async_a(Surface_type& surface, const Image_type& image,
	float fpos_x, float fpos_y, float fwidth, float fheight, float alpha)
{
	if (fpos_x > 1.0f || fpos_y > 1.0f || fpos_x < 0.0f || fpos_y < 0.0f) return;

	int pos_x = surface.width * fpos_x;
	int pos_y = surface.height * fpos_y;

	fwidth = min(fwidth, 1.0f - fpos_x);
	fheight = min(fheight, 1.0f - fpos_y);

	int width = surface.width * fwidth;
	int height = surface.height * fheight;

	std::future<void> res[MAX_THREADS];

	for (int i = 0; i < workers.size; i++)
	{
		int from_x = i * width / workers.size;
		int to_x = (i + 1) * width / workers.size;

		res[i] = workers.add_task([alpha, from_x, to_x, pos_y, pos_x, height, width, &surface, &image]()
			{
				for (int y = 0; y < height; y++)
					for (int x = from_x; x < to_x; x++)
					{
						assert(x < surface.width);
						fColor color = image.get_pixel_scaled(x, y, width, height);
						color.a = alpha;
						drawPixel_a(surface, x + pos_x, y + pos_y, color);
					}
			});
	}

	for (int i = 0; i < workers.size; i++)
		res[i].get();
}
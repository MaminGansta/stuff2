
#ifndef MAX
#define MAX(a, b) (a > b? a: b)
#endif

// ====================  Canvas draw ===========================

inline void drawPixel(Canvas& surface, int x, int y, fColor color)
{
	if ((uint32_t)y >= surface.height || (uint32_t)x >= surface.width) return;
	surface[y * surface.width + x] = color.get_uint();
}


inline void drawLine(Canvas& surface, int x, int y, int x2, int y2, fColor color)
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
				drawPixel(surface, j >> 16, y, color);
				j += decInc;
			}
			return;
		}
		longLen += y;
		for (int j = 0x8000 + (x << 16); y >= longLen; --y) {
			drawPixel(surface, j >> 16, y, color);
			j -= decInc;
		}
		return;
	}

	if (longLen > 0) {
		longLen += x;
		for (int j = 0x8000 + (y << 16); x <= longLen; ++x) {
			drawPixel(surface, x, j >> 16, color);
			j += decInc;
		}
		return;
	}
	longLen += x;
	for (int j = 0x8000 + (y << 16); x >= longLen; --x) {
		drawPixel(surface, x, j >> 16, color);
		j -= decInc;
	}
}

void fdraw_line(Canvas& surface, float fx0, float fy0, float fx1, float fy1, fColor color)
{
	int x0 = fx0 * surface.width;
	int y0 = fy0 * surface.height;
	int x1 = fx1 * surface.width;
	int y1 = fy1 * surface.height;
	drawLine(surface, x0, y0, x1, y1, color);
}

inline void draw_filled_rect(Canvas& surface, float fx, float fy, float fwidth, float fheight, fColor color)
{
	int x0 = surface.width * fx;
	int y0 = surface.height * fy;

	int width = surface.width * fwidth;
	int height = surface.height * fheight;

	for (int y = y0; y < y0 + height; y++)
		for (int x = x0; x < width; x++)
			drawPixel(surface, x, y, color);

}

inline void draw_filled_rect_async(Canvas& surface, float fx, float fy, float fwidth, float fheight, fColor color)
{
	int x0 = surface.width* fx;
	int y0 = surface.height * fy;

	int width = surface.width * fwidth;
	int height = surface.height * fheight;

	std::future<void> res[MAX_THREADS];

	for (int i = 0; i < workers.size; i++)
	{
		int from_x = x0 + (i) * width / workers.size;
		int to_x = x0 + (i + 1)*width / workers.size;

		res[i] = workers.add_task([y0, height, from_x, to_x, &surface, &color]() {
			for (int y = y0; y < y0 + height; y++)
				for (int x = from_x; x < to_x; x++)
					drawPixel(surface, x, y, color);
		});
	}

	for (int i = 0; i < workers.size; i++)
		res[i].get();
}




// ===================== fImage draw ==========================

inline void drawPixel(fImage& surface, int x, int y, fColor color)
{
	if ((uint32_t)y >= surface.height || (uint32_t)x >= surface.width) return;
	surface[y * surface.width + x] = color;
}

inline void addPixel(fImage& surface, int x, int y, fColor color)
{
	if ((uint32_t)y >= surface.height || (uint32_t)x >= surface.width) return;
	surface[y * surface.width + x] += color;
}


inline void drawLine(fImage& surface, int x, int y, int x2, int y2, fColor color)
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
				drawPixel(surface, j >> 16, y, color);
				j += decInc;
			}
			return;
		}
		longLen += y;
		for (int j = 0x8000 + (x << 16); y >= longLen; --y) {
			drawPixel(surface, j >> 16, y, color);
			j -= decInc;
		}
		return;
	}

	if (longLen > 0) {
		longLen += x;
		for (int j = 0x8000 + (y << 16); x <= longLen; ++x) {
			drawPixel(surface, x, j >> 16, color);
			j += decInc;
		}
		return;
	}
	longLen += x;
	for (int j = 0x8000 + (y << 16); x >= longLen; --x) {
		drawPixel(surface, x, j >> 16, color);
		j -= decInc;
	}
}

void fdraw_line(fImage& surface, float fx0, float fy0, float fx1, float fy1, fColor color)
{
	int x0 = fx0 * surface.width;
	int y0 = fy0 * surface.height;
	int x1 = fx1 * surface.width;
	int y1 = fy1 * surface.height;
	drawLine(surface, x0, y0, x1, y1, color);
}

inline void draw_filled_rect(fImage& surface, float fx, float fy, float fwidth, float fheight, fColor color)
{
	int x0 = surface.width * fx;
	int y0 = surface.height * fy;

	int width = surface.width * fwidth;
	int height = surface.height * fheight;

	for (int y = y0; y < y0 + height; y++)
		for (int x = x0; x < width; x++)
			drawPixel(surface, x, y, color);

}

inline void draw_filled_rect_aync(fImage& surface, float fx, float fy, float fwidth, float fheight, fColor color)
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
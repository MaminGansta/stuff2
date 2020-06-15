
#define roundf(x) floor(x + 0.5f)

void line_DDA(Canvas& surface, float x1, float y1, float x2, float y2)
{
	int iX1 = roundf(x1);
	int iY1 = roundf(y1);
	int iX2 = roundf(x2);
	int iY2 = roundf(y2);

	int deltaX = abs(iX1 - iX2);
	int deltaY = abs(iY1 - iY2);

	int length = max(deltaX, deltaY);
	if (length == 0)
	{
		drawPixel(surface, iX1, iY1, Color(255));
		return;
	}

	double dX = (x2 - x1) / length;
	double dY = (y2 - y1) / length;

	double x = x1;
	double y = y1;

	length++;
	while (length--)
	{
		x += dX;
		y += dY;
		drawPixel(surface, roundf(x), roundf(y), Color(255));
	}
}



//void line_Bresenham(Canvas& surface, int x0, int y0, int x1, int y1)
//{
//	bool steep = false;
//	if (std::abs(x0 - x1) < std::abs(y0 - y1))
//	{
//		std::swap(x0, y0);
//		std::swap(x1, y1);
//		steep = true;
//	}
//	if (x0 > x1)
//	{
//		std::swap(x0, x1);
//		std::swap(y0, y1);
//	}
//
//	int dx = x1 - x0;
//	int dy = y1 - y0;
//	float derror = std::abs(dy / float(dx));
//	float error = 0;
//	int y = y0;
//
//	for (int x = x0; x <= x1; x++)
//	{
//		if (steep)
//			drawPixel(surface, x, y, Color(255));
//		else
//			drawPixel(surface, y, x, Color(255));
//
//		error += derror;
//		if (error > 0.5f) {
//			y += (y1 > y0 ? 1 : -1);
//			error -= 1.0f;
//		}
//	}
//}

void line_Bresenham(Canvas& surface, int x0, int y0, int x1, int y1)
{
	bool steep = false;
	if (std::abs(x0 - x1) < std::abs(y0 - y1))
	{
		std::swap(x0, y0);
		std::swap(x1, y1);
		steep = true;
	}
	if (x0 > x1)
	{
		std::swap(x0, x1);
		std::swap(y0, y1);
	}

	int dx = x1 - x0;
	int dy = y1 - y0;
	int derror2 = std::abs(dy) * 2;
	int error2 = 0;
	int y = y0;
	
	for (int x = x0; x <= x1; x++)
	{
		if (steep)
			drawPixel(surface, y, x, Color(255));
		else
			drawPixel(surface, x, y, Color(255));
		
		error2 += derror2;

		if (error2 > dx)
		{
			y += (y1 > y0 ? 1 : -1);
			error2 -= dx * 2;
		}
	}
}




//void drawPixel8(Canvas& canvas, int xc, int yc, int x, int y)
//{	
//	drawPixel(canvas, xc + x, yc + y, Color(255));
//	drawPixel(canvas, xc - x, yc + y, Color(255));
//	drawPixel(canvas, xc + x, yc - y, Color(255));
//	drawPixel(canvas, xc - x, yc - y, Color(255));
//	drawPixel(canvas, xc + y, yc + x, Color(255));
//	drawPixel(canvas, xc - y, yc + x, Color(255));
//	drawPixel(canvas, xc + y, yc - x, Color(255));
//	drawPixel(canvas, xc - y, yc - x, Color(255));
//}
//
//void circleBres(Canvas& canvas, int xc, int yc, int r)
//{
//	int x = 0, y = r;
//	int d = 3 - 2 * r;
//	drawPixel8(canvas, xc, yc, x, y);
//	while (y >= x)
//	{
//		x++;
//		if (d > 0)
//		{
//			y--;
//			d = d + 4 * (x - y) + 10;
//		}
//		else
//			d = d + 4 * x + 6;
//
//		drawPixel8(canvas, xc, yc, x, y);
//	}
//}


void BresenhamCircle(Canvas& canvas, int x0, int y0, int radius, const Color& clr)
{
	int x = 0, y = radius, gap = 0, delta = (2 - 2 * radius);
	while (y >= 0)
	{
		drawPixel(canvas, x0 + x, y0 + y, clr);
		drawPixel(canvas, x0 + x, y0 - y, clr);
		drawPixel(canvas, x0 - x, y0 - y, clr);
		drawPixel(canvas, x0 - x, y0 + y, clr);
		gap = 2 * (delta + y) - 1;
		if (delta < 0 && gap <= 0)
		{
			x++;
			delta += 2 * x + 1;
			continue;
		}
		if (delta > 0 && gap > 0)
		{
			y--;
			delta -= 2 * y + 1;
			continue;
		}
		x++;
		delta += 2 * (x - y);
		y--;
	}
}




// global buffer
vec2* __bezier_buffer = NULL;
int __bezier_capacity = 0;


vec2 getBezierPoint(const std::vector<vec2>& points, float t)
{
	// reallocate buffer if neccessary
	if (__bezier_capacity < points.size())
	{
		delete[] __bezier_buffer;
		__bezier_capacity = points.size();
		__bezier_buffer = new vec2[__bezier_capacity];
	}

	vec2* tmp = __bezier_buffer;
	memmove(tmp, points.data(), points.size() * sizeof(vec2));

	for (int i = points.size() - 1; i > 0; i--)
		for (int k = 0; k < i; k++)
			tmp[k] = tmp[k] + t * (tmp[k + 1] - tmp[k]);

	return tmp[0];
}


void drawBezier(Canvas& surface, const std::vector<vec2>& points)
{
	vec2 priv = getBezierPoint(points, 0);
	for (float i = 0.05f; i < 1; i += 0.05f)
	{
		vec2 point = getBezierPoint(points, i);
		//drawPixel(surface, point.x * surface.width, point.y * surface.height, Color(255));
		drawLine(surface, priv.x * surface.width, priv.y * surface.height,
			point.x * surface.width, point.y * surface.height, Color(255));
		priv = point;
	}
}
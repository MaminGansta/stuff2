
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
	float derror = std::abs(dy / float(dx));
	float error = 0;
	int y = y0;

	for (int x = x0; x <= x1; x++)
	{
		if (steep)
			drawPixel(surface, x, y, Color(255));
		else
			drawPixel(surface, y, x, Color(255));

		error += derror;
		if (error > 0.5f) {
			y += (y1 > y0 ? 1 : -1);
			error -= 1.0f;
		}
	}
}



void drawCircle(Canvas& canvas, int xc, int yc, int x, int y)
{	
	drawPixel(canvas, xc + x, yc + y, Color(255));
	drawPixel(canvas, xc - x, yc + y, Color(255));
	drawPixel(canvas, xc + x, yc - y, Color(255));
	drawPixel(canvas, xc - x, yc - y, Color(255));
	drawPixel(canvas, xc + y, yc + x, Color(255));
	drawPixel(canvas, xc - y, yc + x, Color(255));
	drawPixel(canvas, xc + y, yc - x, Color(255));
	drawPixel(canvas, xc - y, yc - x, Color(255));
}

void circleBres(Canvas& canvas, int xc, int yc, int r)
{
	int x = 0, y = r;
	int d = 3 - 2 * r;
	drawCircle(canvas, xc, yc, x, y);
	while (y >= x)
	{
		x++;
		if (d > 0)
		{
			y--;
			d = d + 4 * (x - y) + 10;
		}
		else
			d = d + 4 * x + 6;

		drawCircle(canvas, xc, yc, x, y);
	}
}
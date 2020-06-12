

enum class FigureType
{
	line_dda,
	line_bresenham,
	circle,
	bezier,
	none
};


struct Figure
{
	FigureType type;
	float x0 = 0, y0 = 0, x1 = 0, y1 = 0;

};

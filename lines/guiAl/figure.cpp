

struct vec2 {
    float x = 0, y = 0;

	vec2() = default;
    vec2(float x, float y) : x(x), y(y) {}
};  

vec2 operator + (vec2 a, vec2 b)
{
	return vec2(a.x + b.x, a.y + b.y);
}

vec2 operator - (vec2 a, vec2 b)
{
	return vec2(a.x - b.x, a.y - b.y);
}

vec2 operator * (float s, vec2 a)
{
	return vec2(s * a.x, s * a.y);
}




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

	std::vector<vec2> bezier;
};

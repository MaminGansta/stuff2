
template <typename T>
T chanel_clip(T color);

template <>
float chanel_clip<float>(float color) { return MIN(MAX(color, 0.0f), 1.0f); }

template <>
int chanel_clip<int>(int color) { return MIN(MAX(color, 0), 255); }

template <>
uint8_t chanel_clip<uint8_t>(uint8_t color) { return MIN(MAX(color, 0), 255); }



struct Color
{
	union
	{
		struct { uint8_t b, g, r, a; };
		struct { uint8_t Y, U, V; };
		uint8_t raw[4];
		uint32_t whole;
	};

	inline Color() : r(0), g(0), b(0), a(255) {}
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

	Color operator *(float f) const
	{
		return Color(r * f, g * f, b * f);
	}

	Color operator /(float f) const
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

	static uint8_t clip_chanel(int chanel)
	{
		return max(min(chanel, 255), 0);
	}

};


struct fColor
{
	union
	{
		struct { float b, g, r, a; };
		struct { float Y, U, V; };
		float raw[4];
	};

	fColor() : r(0.0f), g(0.0f), b(0.0f), a(1.0f) {}
	fColor(float r, float g, float b, float a = 1.0f) : r(r), g(g), b(b), a(a) {}
	fColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) : r(float(r) / 255.0f), g(float(g) / 255.0f), b(float(b) / 255.0f), a(float(a) / 255.0f) {}
	fColor(float color) : r(color), g(color), b(color), a(1.0f) {}
	fColor(Color color) : r(float(color.r) / 255.0f), g(float(color.g) / 255.0f), b(float(color.b) / 255.0f), a(float(color.a) / 255.0f) {}


	fColor operator +(const fColor& f) const
	{
		return fColor(r + f.r, g + f.g, b + f.b);
	}

	fColor& operator +=(const fColor& f)
	{
		r = min(1.0f, r + f.r);
		g = min(1.0f, g + f.g);
		b = min(1.0f, b + f.b);
		return *this;
	}

	fColor operator /(float f) const
	{
		return fColor(r / f, g / f, b / f);
	}

	fColor operator * (float f) const
	{
		return fColor(min(1.0f, r * f), min(1.0f, g * f), min(1.0f, b * f));
	}

	fColor& operator *= (float f)
	{
		r = min(1.0f, r * f);
		g = min(1.0f, g * f);
		b = min(1.0f, b * f);
		return *this;
	}

	static float chanel_clip(float chanel)
	{
		return max(min(chanel, 1.0f), 0.0f);
	}

	operator Color() const
	{
		return Color(min(r * 255.0f, 255.0f), min(g * 255.0f, 255.0f), min(b * 255.0f, 255.0f), min(a * 255.0f, 255.0f));
	}
};
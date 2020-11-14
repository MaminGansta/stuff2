#pragma once

#include <cstdint>
#include <stdlib.h>



template <typename T>
T chanel_clip(T color);

template <>
float chanel_clip<float>(float color);

template <>
int chanel_clip<int>(int color);

template <>
uint8_t chanel_clip<uint8_t>(uint8_t color);


namespace color
{

	template <typename T>
	T max_val() { return T(); }

	template <>
	float max_val<float>();

	template <>
	uint8_t max_val<uint8_t>();


#define clip(chanel)  __min(__max((chanel), 0), gui::color::max_val<T>())
}


template <typename T>
struct Color_base
{
	union
	{
		struct { T b, g, r, a; };
		struct { T Y, U, V; };
		T raw[4];
	};

	Color_base() : r(0), g(0), b(0), a(color::max_val<T>()) {}
	Color_base(T r, T g, T b, T a = color::max_val<T>()) : r(r), g(g), b(b), a(a) {}
	Color_base(T color) : r(color), g(color), b(color), a(color::max_val<T>()) {}


	Color_base<T> operator +(const Color_base<T>& color) const
	{
		return Color_base<T>(__min(r + color.r, color::max_val<T>()),
							 __min(g + color.g, color::max_val<T>()), 
							 __min(b + color.b, color::max_val<T>()));
	}

	Color_base<T> operator- (const Color_base<T>& color) const
	{
		return Color_base<T>(r - color.r, g - color.g, b - color.b);
	}

	Color_base<T> operator*(float f) const
	{
		return Color_base<T>(r * f, g * f, b * f);
	}

	Color_base<T> operator /(float f) const
	{
		return Color_base<T>(r / f, g / f, b / f);
	}

	Color_base<T>& operator += (const Color_base<T>& other)
	{
		r = __min(r + other.r, color::max_val<T>());
		g = __min(g + other.g, color::max_val<T>());
		b = __min(b + other.b, color::max_val<T>());
		return *this;
	}

	Color_base<T>& operator *=(float f)
	{
		r *= f;
		g *= f;
		b *= f;
		return *this;
	}

	uint8_t clip_chanel(int chanel)
	{
		return __max(__min(chanel, color::max_val<T>()), 0);
	}

	operator Color_base<uint8_t> () const
	{
		return Color_base<uint8_t>(r * 255, g * 255, b * 255, a * 255);
	}

	operator Color_base<float>() const
	{
		return Color_base<float>(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
	}
};


typedef Color_base<uint8_t> Color;
typedef Color_base<float> fColor;


// Color
extern template struct Color_base<uint8_t>;


// fColor
extern template struct Color_base<float>;

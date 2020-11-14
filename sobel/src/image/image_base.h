#pragma once

#include <cassert>
#include <memory.h>
#include "color.h"


template <typename T>
struct Image_base
{
	Color_base<T>* data = nullptr;
	int height = 0, width = 0;
	int whole_size = 0;

	Image_base() = default;

	Image_base(int width, int height)
	:
		width(width),
		height(height),
		whole_size(height* width)
	{
		data = new Color_base<T>[whole_size];
	}


	Image_base(const Image_base<T>& copy)
	{
		height = copy.height;
		width = copy.width;
		whole_size = copy.whole_size;
		data = new Color_base<T>[height * width];
		memmove(data, copy.data, sizeof(Color_base<T>) * height * width);
	}

	Image_base(Image_base<T>&& other)
	{
		data = other.data;
		width = other.width;
		height = other.height;
		whole_size = other.whole_size;
		other.data = NULL;
	}

	Image_base<T>& operator= (const Image_base<T>& copy)
	{
		delete[] data;
		height = copy.height;
		width = copy.width;
		whole_size = copy.whole_size;
		data = new Color_base<T>[height * width];
		memmove(data, copy.data, sizeof(Color_base<T>) * height * width);
		return *this;
	}

	Image_base<T>& operator = (Image_base<T>&& other)
	{
		delete[] data;
		data = other.data;
		width = other.width;
		height = other.height;
		whole_size = other.whole_size;
		other.data = NULL;
		return *this;
	}

	~Image_base()
	{
		delete[] data;
	}

	bool valid() const
	{
		return width != 0 && height != 0 && data != NULL;
	}

	void resize(int width, int height)
	{
		this->width = width;
		this->height = height;
		whole_size = width * height;
		delete[] data;
		data = new Color_base<T>[whole_size];
	}

	Color_base<T>& get_pixel(int x, int y)
	{
		assert(((uint32_t)y < height) | ((uint32_t)x < width));
		return data[y * width + x];
	}

	Color_base<T>& operator[] (int idx)
	{
		assert((uint32_t)idx < whole_size);
		return data[idx];
	}

	const Color_base<T>& operator[] (int idx) const
	{
		assert((uint32_t)idx < whole_size);
		return data[idx];
	}


	Color_base<T>& get_pixel_scaled(int x, int y, int screen_w, int screen_h)
	{
		y = y * height / screen_h;
		x = x * width / screen_w;

		assert(((uint32_t)y < height) | ((uint32_t)x < width));
		return data[y * width + x];
	}

	const Color_base<T>& get_pixel_scaled(int x, int y, int screen_w, int screen_h) const
	{
		y = y * height / screen_h;
		x = x * width / screen_w;

		assert(((uint32_t)y < height) | ((uint32_t)x < width));
		return data[y * width + x];
	}

};


extern template struct Image_base<uint8_t>;
extern template struct Image_base<float>;


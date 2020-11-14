#pragma once

#include "../image/image.h"


template <typename T>
Image_base<T>  RGB2YCbCr(const Image_base<T>& origin)
{
	Image_base<T> res(origin.width, origin.height);

	for (int i = 0; i < origin.width * origin.height; i++)
	{
		fColor pixel = origin.data[i];
		fColor temp;

		temp.Y = 0.2126f * pixel.r + 0.7152f * pixel.g + 0.0722 * pixel.b;
		temp.U = -0.0999f * pixel.r - 0.336f * pixel.g + 0.436f * pixel.b;
		temp.V = 0.615f * pixel.r - 0.5586f * pixel.g - 0.0563 * pixel.b;

		res[i] = temp;
	}

	return res;
}


template <typename T>
Image_base<T> YCbCr2RGB(const Image_base<T>& origin)
{
	Image_base<T> res(origin.width, origin.height);

	for (int i = 0; i < origin.width * origin.height; i++)
	{
		fColor pixel = origin.data[i];
		fColor temp;

		temp.r = chanel_clip(pixel.Y + 1.2803f * pixel.V);
		temp.g = chanel_clip(pixel.Y - 0.2148f * pixel.U - 0.4805f * pixel.V);
		temp.b = chanel_clip(pixel.Y + 2.1279f * pixel.U);
		res[i] = temp;
	}

	return res;
}


template <typename T>
Image_base<T> RGB2BW(const Image_base<T>& origin)
{
	Image_base<T> res(origin.width, origin.height);

	for (int i = 0; i < origin.width * origin.height; i++)
	{
		fColor pixel = origin.data[i];

		float Y = 0.2126f * pixel.r + 0.7152f * pixel.g + 0.0722 * pixel.b;;
		res[i] = fColor(Y);
	}

	return res;
}

#pragma once

#include "image_cpu.h"

namespace cpu
{
	inline Image RGB2YCbCr(const Image& origin)
	{
		Image res( Texture2DSpecification{ origin.GetWidth(), origin.GetHeight() } );

		for (int i = 0; i < origin.GetWidth() * origin.GetHeight(); i++)
		{
			const uint8_t* pixel = origin.GetPixel(i);
			uint8_t temp[4];

			temp[0] = 0.2126f * pixel[0] + 0.7152f * pixel[1] + 0.0722 * pixel[2];
         temp[1] = temp[2] = temp[0];
         temp[3] = 255;
			//temp[1] = -0.0999f * pixel[0] - 0.336f  * pixel[1] + 0.436f * pixel[2];
			//temp[2] =  0.615f  * pixel[0] - 0.5586f * pixel[1] - 0.0563 * pixel[2];
			res.SetPixel(temp, i);
		}

		return res;
	}

}
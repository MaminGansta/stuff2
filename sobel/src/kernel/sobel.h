
#include "convolution.h"
#include "image_conversion.h"



template <typename Image_type>
struct Sobel_y : public Kernel<Image_type, 3>
{
	Sobel_y() : Kernel<Image_type, 3>({ 1, 0 , -1, 2, 0, -2, 1, 0 , -1 }) {}
};

template <typename Image_type>
struct Sobel_x : public Kernel<Image_type, 3>
{
	Sobel_x() : Kernel<Image_type, 3>({ 1, 2 , 1, 0, 0, 0, -1, -2 , -1 }) {}
};


template <typename Image_type>
Image_type sobel(const Image_type& origin)
{
	Image_type res(origin.width, origin.height);
	Sobel_y<Image_type> sobel_y;
	Sobel_x<Image_type> sobel_x;

	Image_type sob_x = sobel_y.apply_async(RGB2BW(origin));
	Image_type sob_y = sobel_x.apply_async(RGB2BW(origin));

	for (int i = 0; i < origin.width * origin.height; i++)
	{
		res[i] = sob_x[i] + sob_y[i];
	}
	return res;
}

template <typename Image_type>
Image_type sobel_sigle_core(const Image_type& origin)
{
	Image_type res(origin.width, origin.height);
	Sobel_y<Image_type> sobel_y;
	Sobel_x<Image_type> sobel_x;

	Image_type sob_x = sobel_y.apply(RGB2BW(origin));
	Image_type sob_y = sobel_x.apply(RGB2BW(origin));

	for (int i = 0; i < origin.width * origin.height; i++)
	{
		res[i] = sob_x[i] + sob_y[i];
	}
	return res;
}

template <typename Image_type>
Image_type sobel_avg(const Image_type& origin)
{
	Image_type res(origin.width, origin.height);
	Sobel_y<Image_type> sobel_y;
	Sobel_x<Image_type> sobel_x;

	Image_type sob_x = sobel_y.apply_async(RGB2BW(origin));
	Image_type sob_y = sobel_x.apply_async(RGB2BW(origin));

	for (int i = 0; i < origin.width * origin.height; i++)
	{
		res[i] = (sob_x[i] / 2 + sob_y[i] / 2);
	}
	return res;
}
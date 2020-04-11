
template <typename Image_type>
Image_type gray_world(Image_type& image, const Histogram<Image_type>& hist)
{
	Image_type res;
	res.resize(image.width, image.height);

	uint32_t R = 0;
	uint32_t G = 0;
	uint32_t B = 0;
	uint32_t AVG = 0;
	uint32_t count = 0;

	for (int i = 0; i < 256; i++)
	{
		count += hist.cred[i];
		R += i * hist.cred[i];
		G += i * hist.cgreen[i];
		B += i * hist.cblue[i];
	}

	AVG = (R + G + B) / 3;

	float rw = float(AVG) / R;
	float gw = float(AVG) / G;
	float bw = float(AVG) / B;

	for (int i = 0; i < image.height * image.width; i++)
	{
		res[i].r = chanel_clip(image[i].r * rw);
		res[i].g = chanel_clip(image[i].g * gw);
		res[i].b = chanel_clip(image[i].b * bw);
	}

	return res;
}


template <typename Image_type>
Image_type gray_world(Image_type& image)
{
	Histogram<Image_type> hist(image);

	Image_type res;
	res.resize(image.width, image.height);

	uint32_t R = 0;
	uint32_t G = 0;
	uint32_t B = 0;
	uint32_t AVG = 0;
	uint32_t count = 0;

	for (int i = 0; i < 256; i++)
	{
		count += hist.cred[i];
		R += i * hist.cred[i];
		G += i * hist.cgreen[i];
		B += i * hist.cblue[i];
	}

	AVG = (R + G + B) / 3;

	float rw = float(AVG) / R;
	float gw = float(AVG) / G;
	float bw = float(AVG) / B;

	for (int i = 0; i < image.height * image.width; i++)
	{
		res[i].r = chanel_clip(int(image[i].r * rw));
		res[i].g = chanel_clip(int(image[i].g * gw));
		res[i].b = chanel_clip(int(image[i].b * bw));
	}

	return res;
}